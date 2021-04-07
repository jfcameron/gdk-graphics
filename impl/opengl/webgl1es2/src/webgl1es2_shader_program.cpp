// © 2018 Joseph Cameron - All Rights Reserved

#include <gdkgraphics/buildinfo.h>

#include <gdk/glh.h>
#include <gdk/webgl1es2_shader_program.h>

#include <atomic>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace gdk;

static constexpr char TAG[] = "shader_program";

//! map of active textures. Allows to overwrite textures with the same names to the same units, since units are very limited.
static std::unordered_map<std::string, GLint> s_ActiveTextureUniformNameToUnit;

static short s_ActiveTextureUnitCounter(0);

//TODO: Consider moving this to the context. Context is the correct OO solution, but what is the meaning of two separate GL
// contexts? they will affect eachother's state.
static std::atomic<GLint> s_CurrentShaderProgramHandle(-1);

const jfc::shared_proxy_ptr<gdk::webgl1es2_shader_program> webgl1es2_shader_program::PinkShaderOfDeath([]()
{
    const std::string vertexShaderSource(R"V0G0N(    
    //Uniforms
    uniform mat4 _MVP;

    //VertexIn
    attribute highp vec3 a_Position;

    void main()
    {
        gl_Position = _MVP * vec4(a_Position,1.0);
    }
    )V0G0N");

    const std::string fragmentShaderSource(R"V0G0N(
    const vec4 DEATHLY_PINK = vec4(1,0.2,0.8,1);

    void main()
    {
        gl_FragColor = DEATHLY_PINK;
    }
    )V0G0N");

    return new gdk::webgl1es2_shader_program(vertexShaderSource, fragmentShaderSource);
});

const jfc::shared_proxy_ptr<gdk::webgl1es2_shader_program> webgl1es2_shader_program::AlphaCutOff([]()
{
    const std::string vertexShaderSource(R"V0G0N(
    //Uniforms
    uniform mat4 _MVP;
    uniform mat4 _Model;
    uniform mat4 _View;
    uniform mat4 _Projection;

    //VertexIn
    attribute highp vec3 a_Position;
    attribute mediump vec2 a_UV;
    
    //FragmentIn
    varying mediump vec2 v_UV;

    void main ()
    {
        gl_Position = _MVP * vec4(a_Position,1.0);

        v_UV = a_UV;
    }
    )V0G0N");

    const std::string fragmentShaderSource(R"V0G0N(
    //Uniforms
    uniform sampler2D _Texture;
    uniform vec2 _UVOffset;
    uniform vec2 _UVScale; 

    //FragmentIn
    varying lowp vec2 v_UV;
    lowp vec2 uv;

    void main()
    {
        uv = v_UV;  
        uv += _UVOffset;
        uv *= _UVScale;

        vec4 frag = texture2D(_Texture, uv);

        if (frag[3] < 1.0) discard;

        gl_FragColor = frag;                        
    }
    )V0G0N");

    auto p = new gdk::webgl1es2_shader_program(vertexShaderSource, fragmentShaderSource);

    return p;
});

static inline void setUpFaceCullingMode(webgl1es2_shader_program::FaceCullingMode a)
{
    if (a == webgl1es2_shader_program::FaceCullingMode::None)
    {
        glDisable(GL_CULL_FACE);

        return;
    }

    glEnable(GL_CULL_FACE);

    switch(a)
    {
        case webgl1es2_shader_program::FaceCullingMode::Front: glCullFace(GL_FRONT); break;
        case webgl1es2_shader_program::FaceCullingMode::Back: glCullFace(GL_BACK); break;
        case webgl1es2_shader_program::FaceCullingMode::FrontAndBack: glCullFace(GL_FRONT_AND_BACK); break;

        case webgl1es2_shader_program::FaceCullingMode::None: break;
    }
}

static void perform_shader_code_preprocessing_done_to_both_vertex_and_fragment_stages(std::string &aSource)
{
    aSource.insert(0, std::string("#define ").append(gdkgraphics_BuildInfo_TargetPlatform).append("\n"));
#if defined JFC_TARGET_PLATFORM_Emscripten 
    // version must be the first line in source. version must be present for WebGL platforms
    aSource.insert(0, std::string("#version 100\n"));
#else
   // remove precison prefixes (required by wasm)
   aSource.insert(0, std::string("#define highp\n"));
   aSource.insert(0, std::string("#define mediump\n"));
   aSource.insert(0, std::string("#define lowp\n"));
#endif
}

static void perform_shader_code_preprocessing_done_to_only_fragment_stages(std::string &aSource)
{
#if defined JFC_TARGET_PLATFORM_Emscripten 
    // sets float precision, required by webgl
    aSource.insert(0, std::string("precision mediump float;\n"));
#endif
}

webgl1es2_shader_program::webgl1es2_shader_program(std::string aVertexSource, std::string aFragmentSource)
: m_VertexShaderHandle([&aVertexSource]()
{
    perform_shader_code_preprocessing_done_to_both_vertex_and_fragment_stages(aVertexSource);

    // Compile vertex stage
    const GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *const vertex_shader = aVertexSource.c_str();
    glShaderSource(vs, 1, &vertex_shader, 0);
    glCompileShader(vs);

    return decltype(m_VertexShaderHandle)(vs, [](const GLuint handle)
    {
        glDeleteShader(handle);
    });
}())
, m_FragmentShaderHandle([&aFragmentSource]()
{
    perform_shader_code_preprocessing_done_to_only_fragment_stages(aFragmentSource);
    perform_shader_code_preprocessing_done_to_both_vertex_and_fragment_stages(aFragmentSource);
    
    // Compile fragment stage
    const char *const fragment_shader = aFragmentSource.c_str();
    const GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, 0);
    glCompileShader(fs);

    return decltype(m_FragmentShaderHandle)(fs, [](const GLuint handle)
    {
        glDeleteShader(handle);
    });
}())
, m_ProgramHandle([this]()
{
    const auto vs = m_VertexShaderHandle.get();
    const auto fs = m_FragmentShaderHandle.get();

    // Link the program
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vs);
    glAttachShader(programHandle, fs);
    glLinkProgram(programHandle);

    // Confirm no compilation/link errors
    GLint status(-1);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
    
    if (status == GL_FALSE)
    {
        std::ostringstream message;
        
        message << "A shader could not be linked!\n";
        
        static const auto decorator = [](std::ostringstream& ss, std::string log_header, 
            std::string &&log_msg)
        {
            ss  << log_header << "\n" << (log_msg.size() ? log_msg : "clear") << "\n\n";
        };

        decorator(message, "vertex log", glh::GetShaderInfoLog(vs));
        decorator(message, "fragment log", glh::GetShaderInfoLog(fs));
        decorator(message, "program log", glh::GetProgramInfoLog(programHandle));

        throw std::runtime_error(std::string(TAG).append(": ").append(message.str()));
    }

    return jfc::unique_handle<GLuint>(programHandle,
        [](const GLuint handle)
        {
            glDeleteProgram(handle);
        });
}())
{
    const auto programHandle = m_ProgramHandle.get();

    GLint count;

    GLsizei currentNameLength;
  
    // poll active attributes, record their names and locations for attribute enabling & creating vertex attrib pointers
    {
        // (c string bookkeeping) create a buffer big enough to contain the longest active attrib's name
        GLint maxAttribNameLength(0);
        glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
        
        std::vector<GLchar> attrib_name_buffer(maxAttribNameLength); 

        glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &count);

        for (decltype(count) i(0); i < count; ++i)
        {
            GLint component_count;
            GLenum component_type;

            glGetActiveAttrib(programHandle, 
                i, 
                attrib_name_buffer.size(),    // (c string bookkeeping) max size the gl can safely write into my buffer
                &currentNameLength,           // (c string bookkeeping) actual char count for the current attribute's name 
                &component_count,             // e.g: 3 //TODO: RENAME! This isnt what I tohught it was. 
                                              // Size is the number of gltypes.. so vec2 would == 1 as in 1 vec 2, not 2 floats,
                                              // being the components... need to rename all this stuff...
                &component_type,              // e.g: float
                &attrib_name_buffer.front()); // e.g: "a_Position"

            webgl1es2_shader_program::active_attribute_info info;
            info.location = i;
            info.type = component_type;
            info.count = component_count;

            m_ActiveAttributes[std::string(attrib_name_buffer.begin(), 
                attrib_name_buffer.begin() + currentNameLength)] = std::move(info);
        }
    }

    // poll active uniforms, record their names and locations for uniform value assignments
    {
        // (c string bookkeeping) create a buffer likely big enough to contain the longest active attrib's name
        // There is a GL_ACTIVE_UNIFORM_MAX_LENGTH (equivalent of above attrib code), but I have found recent posts online
        // (2010s) of programmers complaining that it is fairly common for drivers to return incorrect values,
        // so I am allocating a large buffer instead.
        std::vector<GLchar> uniform_name_buffer(256); 

        glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &count);

        for (decltype(count) i(0); i < count; ++i)
        {
            GLint attribute_size;
            GLenum attribute_type;

            glGetActiveUniform(programHandle, 
                i, 
                uniform_name_buffer.size(),    // (c string bookkeeping) max size the gl can safely write into my buffer
                &currentNameLength,            // (c string bookkeeping) actual char count for the current attribute's name
                &attribute_size,               // e.g: "1"
                &attribute_type,               // e.g: "texture"
                &uniform_name_buffer.front()); // e.g: "u_Diffuse" 

            webgl1es2_shader_program::active_uniform_info info;
            info.location = i;
            info.type = attribute_type;
            info.size = attribute_size;

            m_ActiveUniforms[std::string(uniform_name_buffer.begin(), uniform_name_buffer.begin() + currentNameLength)] 
                = std::move(info);
        }
    }
}

GLuint webgl1es2_shader_program::useProgram() const
{
    const auto handle(m_ProgramHandle.get());

    if (s_CurrentShaderProgramHandle != handle)
    {
        s_CurrentShaderProgramHandle = handle;

        s_ActiveTextureUniformNameToUnit.clear();
        s_ActiveTextureUnitCounter = 0;

        setUpFaceCullingMode(m_FaceCullingMode);

        glUseProgram(handle);
    }

    return handle;
}

bool webgl1es2_shader_program::operator==(const webgl1es2_shader_program &b) const
{
    return m_ProgramHandle == b.m_ProgramHandle;
}
bool webgl1es2_shader_program::operator!=(const webgl1es2_shader_program &b) const {return !(*this == b);}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const GLfloat aValue) const 
{
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform1f(search->second.location, aValue);
        
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_vector2_type &aValue) const 
{

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform2f(search->second.location, aValue.x, aValue.y);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_vector3_type &aValue) const 
{
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform3f(search->second.location, aValue.x, aValue.y, aValue.z);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_vector4_type &aValue) const 
{
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform4f(search->second.location, aValue.x, aValue.y, aValue.z, aValue.w);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<GLfloat> &avalue) const 
{
    if (!avalue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform1fv(search->second.location, avalue.size(), &avalue[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_vector2_type> &avalue) const 
{
    if (!avalue.size()) return false;
    
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        std::vector<graphics_vector2_type::component_type> data;

        data.reserve(avalue.size() * 2);

        for (const auto &vec : avalue) 
        {
            data.push_back(vec.x);
            data.push_back(vec.y);
        }

        glUniform2fv(search->second.location, avalue.size(), &data[0]);

        return true;
    }

    return false;
} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_vector3_type> &avalue) const 
{
    if (!avalue.size()) return false;
    
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        std::vector<graphics_vector3_type::component_type> data;

        data.reserve(avalue.size() * 3);

        for (const auto &vec : avalue) 
        {
            data.push_back(vec.x);
            data.push_back(vec.y);
            data.push_back(vec.z);
        }

        glUniform3fv(search->second.location, avalue.size(), &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_vector4_type> &avalue) const 
{
    if (!avalue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        std::vector<graphics_vector4_type::component_type> data;

        data.reserve(avalue.size() * 4);

        for (const auto &vec : avalue) 
        {
            data.push_back(vec.x);
            data.push_back(vec.y);
            data.push_back(vec.z);
            data.push_back(vec.w);
        }

        glUniform4fv(search->second.location, avalue.size(), &data[0]);

        return true;
    }

    return false;
} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const GLint aValue) const 
{
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform1i(search->second.location, aValue);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string aName, const integer2_uniform_type &a) const 
{
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform2i(search->second.location, a[0], a[1]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string aName, const integer3_uniform_type &a) const 
{
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform3i(search->second.location, a[0], a[1], a[2]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string aName, const integer4_uniform_type &a) const 
{
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform4i(search->second.location, a[0], a[1], a[2], a[3]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<GLint> &aValue) const 
{
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniform1iv(search->second.location, aValue.size(), &aValue[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<integer2_uniform_type> &aValue) const 
{
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        std::vector<integer2_uniform_type::value_type> data;

        data.reserve(aValue.size() * 2);

        for (const auto &vec : aValue) 
        {
            data.push_back(vec[0]);
            data.push_back(vec[1]);
        }

        glUniform2iv(search->second.location, aValue.size(), &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<integer3_uniform_type> &aValue) const 
{
    if (!aValue.size()) return false;


    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        std::vector<integer3_uniform_type::value_type> data;

        data.reserve(aValue.size() * 3);

        for (const auto &vec : aValue) 
        {
            data.push_back(vec[0]);
            data.push_back(vec[1]);
            data.push_back(vec[2]);
        }

        glUniform3iv(search->second.location, aValue.size(), &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<integer4_uniform_type> &aValue) const 
{
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        std::vector<integer4_uniform_type::value_type> data;

        data.reserve(aValue.size() * 4);

        for (const auto &vec : aValue) 
        {
            data.push_back(vec[0]);
            data.push_back(vec[1]);
            data.push_back(vec[2]);
            data.push_back(vec[3]);
        }

        glUniform3iv(search->second.location, aValue.size(), &data[0]);

        return true;
    }

    return false;
}

/*bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_mat2x2_type &avalue) const 
{

} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_mat2x2_type> &avalue) const 
{

} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_mat3x3_type &avalue) const 
{

} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_mat3x3_type> &avalue) const 
{

}*/

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_mat4x4_type &a) const 
{
    if (const auto& search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        glUniformMatrix4fv(search->second.location, 1, GL_FALSE, &a.m[0][0]);

        return true;
    }

    return false;
} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_mat4x4_type> &a) const 
{
    if (!a.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) 
    {
        static constexpr auto magnitude(graphics_mat4x4_type::RowOrColumnCount);

        std::vector<graphics_mat4x4_type::component_type> data;

        data.reserve(a.size() * magnitude * magnitude);

        for (const auto &mat : a) 
        {
            for (int y(0); y < magnitude; ++y) for (int x(0); x < magnitude; ++x)
            {
                data.push_back(mat.m[y][x]);
            }
        }

        glUniformMatrix4fv(search->second.location, 1, GL_FALSE, &data[0]);

        return true;
    }

    return false;
} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const gdk::webgl1es2_texture &aTexture) const
{
    if (const auto &activeUniformSearch = m_ActiveUniforms.find(aName); activeUniformSearch != m_ActiveUniforms.end())
    {   
        const auto &activeTextureSearch = s_ActiveTextureUniformNameToUnit.find(aName);
        
        const GLint unit = activeTextureSearch == s_ActiveTextureUniformNameToUnit.end()
            ? [&aName]() {
                decltype(unit) next_unit = s_ActiveTextureUnitCounter++;

                s_ActiveTextureUniformNameToUnit[aName] = next_unit;

                return next_unit;
            }()
            : activeTextureSearch->second;

        if (s_ActiveTextureUnitCounter < webgl1es2_shader_program::MAX_TEXTURE_UNITS) 
        {
            //TODO: parameterize! Improve texture as well to support non2ds. 
            // The type (2d or cube) should be a property of the texture abstraction.
            const GLenum target(GL_TEXTURE_2D); 

            glActiveTexture(GL_TEXTURE0 + unit);

            glBindTexture(target, aTexture.getHandle());

            glUniform1i(activeUniformSearch->second.location, unit);
        }
        else throw std::invalid_argument(std::string("GLES2.0/WebGL1.0 only provide 8 texture units; "
            "you are trying to bind too many simultaneous textures to the context: ") + 
            std::to_string(s_ActiveTextureUnitCounter));

        return true;
    }

    return false;
}

std::optional<webgl1es2_shader_program::active_attribute_info> webgl1es2_shader_program::tryGetActiveAttribute(const std::string &aAttributeName) const
{
    if (auto found = m_ActiveAttributes.find(aAttributeName); found != m_ActiveAttributes.end()) 
        return found->second;

    return {};
}

