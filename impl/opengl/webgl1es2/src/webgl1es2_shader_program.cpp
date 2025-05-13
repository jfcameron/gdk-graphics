// © Joseph Cameron - All Rights Reserved

#include <gdk/glh.h>
#include <gdk/graphics_exception.h>
#include <gdk/webgl1es2_shader_program.h>
#include <gdkgraphics/buildinfo.h>

#include <atomic>
#include <iostream>
#include <sstream>
#include <vector>

using namespace gdk;

static constexpr char TAG[] = "shader_program";

//! map of active textures. Allows to overwrite textures with the same names to the same units, since units are very limited.
static std::unordered_map<std::string, GLint> s_ActiveTextureUniformNameToUnit;

static short s_ActiveTextureUnitCounter(0);

const jfc::lazy_ptr<gdk::webgl1es2_shader_program> webgl1es2_shader_program::PinkShaderOfDeath([]() {
    const std::string vertexShaderSource(R"V0G0N(
    uniform mat4 _MVP;
    attribute highp vec3 a_Position;

    void main() {
        gl_Position = _MVP * vec4(a_Position,1.0);
    }
    )V0G0N");

    const std::string fragmentShaderSource(R"V0G0N(
    void main() {
        gl_FragColor = vec4(1,0.2,0.8,1);
    }
    )V0G0N");

    return new gdk::webgl1es2_shader_program(vertexShaderSource, fragmentShaderSource);
});

const jfc::lazy_ptr<gdk::webgl1es2_shader_program> webgl1es2_shader_program::AlphaCutOff([]() {
    const std::string vertexShaderSource(R"V0G0N(
    uniform mat4 _MVP;
    uniform mat4 _Model;
    uniform mat4 _Projection;
    uniform mat4 _View;

    attribute highp vec3 a_Position;
    attribute mediump vec2 a_UV;
    
    varying mediump vec2 v_UV;

    void main () {
        gl_Position = _MVP * vec4(a_Position,1.0);
        v_UV = a_UV;
    }
    )V0G0N");

    const std::string fragmentShaderSource(R"V0G0N(
    uniform sampler2D _Texture;
    uniform vec2 _UVOffset;
    uniform vec2 _UVScale; 

    varying lowp vec2 v_UV;

    void main() {
        lowp vec2 uv = v_UV + _UVOffset * _UVScale;  

        vec4 texel = texture2D(_Texture, uv);
        if (texel[3] < 1.0) discard;

        gl_FragColor = vec4(texel.xyz, 0.5);                        
    }
    )V0G0N");

    return new gdk::webgl1es2_shader_program(vertexShaderSource, fragmentShaderSource);
});

static void perform_shader_code_preprocessing_done_to_both_vertex_and_fragment_stages(std::string &aSource) {
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

static void perform_shader_code_preprocessing_done_to_only_fragment_stages(std::string &aSource) {
#if defined JFC_TARGET_PLATFORM_Emscripten 
    // sets float precision, required by webgl
    aSource.insert(0, std::string("precision mediump float;\n"));
#endif
}

webgl1es2_shader_program::webgl1es2_shader_program(std::string aVertexSource, std::string aFragmentSource)
: m_VertexShaderHandle([&aVertexSource]() {
    perform_shader_code_preprocessing_done_to_both_vertex_and_fragment_stages(aVertexSource);

    const GLuint hVertexShader(glCreateShader(GL_VERTEX_SHADER));
    const auto *const arrayOfSourceCodeStrings(&aVertexSource.front()); 
    glShaderSource(hVertexShader, 1, &arrayOfSourceCodeStrings, 0);
    glCompileShader(hVertexShader);

    return decltype(m_VertexShaderHandle)(hVertexShader, [](const GLuint handle) {
        glDeleteShader(handle);
    });
}())
, m_FragmentShaderHandle([&aFragmentSource]() {
    perform_shader_code_preprocessing_done_to_only_fragment_stages(aFragmentSource);
    perform_shader_code_preprocessing_done_to_both_vertex_and_fragment_stages(aFragmentSource);
    
    const GLuint hFragmentShader(glCreateShader(GL_FRAGMENT_SHADER));
    const auto *const arrayOfSourceCodeStrings(&aFragmentSource.front()); 
    glShaderSource(hFragmentShader, 1, &arrayOfSourceCodeStrings, 0);
    glCompileShader(hFragmentShader);

    return decltype(m_FragmentShaderHandle)(hFragmentShader, [](const GLuint handle) {
        glDeleteShader(handle);
    });
}())
, m_ProgramHandle([this]() {
    const auto hVertexShader(m_VertexShaderHandle.get());
    const auto hFragmentShader(m_FragmentShaderHandle.get());

    const auto hShaderProgram(glCreateProgram());
    glAttachShader(hShaderProgram, hVertexShader);
    glAttachShader(hShaderProgram, hFragmentShader);
    glLinkProgram(hShaderProgram);

    GLint status;
    glGetProgramiv(hShaderProgram, GL_LINK_STATUS, &status);
    
    if (status == GL_FALSE) {
        std::ostringstream message;
        
        message << "A shader could not be linked!\n";
        
        static const auto decorator = [](std::ostringstream& ss, std::string log_header, std::string &&log_msg) {
            ss  << log_header << "\n" << (log_msg.size() ? log_msg : "clear") << "\n\n";
        };

        decorator(message, "vertex log", glh::GetShaderInfoLog(hVertexShader));
        decorator(message, "fragment log", glh::GetShaderInfoLog(hFragmentShader));
        decorator(message, "program log", glh::GetProgramInfoLog(hShaderProgram));

        throw gdk::graphics_exception(std::string(TAG).append(": ").append(message.str()));
    }

    return jfc::unique_handle<GLuint>(hShaderProgram, [](const GLuint handle) {
        glDeleteProgram(handle);
    });
}()) {
    const auto hShaderProgram(m_ProgramHandle.get());

    // poll active attributes, record their names and locations for attribute enabling & creating vertex attrib pointers
    {
        auto attributeNameBuffer = [&]() {
            GLint maxAttribNameLength(0);
            glGetProgramiv(hShaderProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
            return std::vector<GLchar>(maxAttribNameLength);
        }();

        GLint activeAttributeCount;
        glGetProgramiv(hShaderProgram, GL_ACTIVE_ATTRIBUTES, &activeAttributeCount);

        for (decltype(activeAttributeCount) indexToCurrentAttribute(0); indexToCurrentAttribute < activeAttributeCount; ++indexToCurrentAttribute) {
            GLint attributeCount;
            GLenum attributeType;
            GLsizei attributeNameLength;

            glGetActiveAttrib(hShaderProgram,
                indexToCurrentAttribute,
                attributeNameBuffer.size(), 
                &attributeNameLength, 
                &attributeCount,
                &attributeType,
                &attributeNameBuffer.front());

            webgl1es2_shader_program::active_attribute_info info;
            info.location = indexToCurrentAttribute;
            info.type = attributeType;
            info.count = attributeCount;

            m_ActiveAttributes[std::string(attributeNameBuffer.begin(), 
                attributeNameBuffer.begin() + attributeNameLength)] = std::move(info);
        }
    }

    // poll active uniforms, record their names and locations for uniform value assignments
    {
        // GL_ACTIVE_UNIFORM_MAX_LENGTH is known to return wrong values on many implementations, so I am using a large constant instead
        std::vector<GLchar> uniform_name_buffer(256); 

        GLint activeAttributeCount;
        glGetProgramiv(hShaderProgram, GL_ACTIVE_UNIFORMS, &activeAttributeCount);

        for (decltype(activeAttributeCount) currentAttributeIndex(0); currentAttributeIndex < activeAttributeCount; ++currentAttributeIndex) {
            GLenum attributeType;
            GLint attributeSize;
            GLsizei attributeNameLength;

            glGetActiveUniform(hShaderProgram, 
                currentAttributeIndex, 
                uniform_name_buffer.size(),    // (c string bookkeeping) max size the gl can safely write into my buffer
                &attributeNameLength,            // (c string bookkeeping) actual char activeAttributeCount for the current attribute's name
                &attributeSize,               // e.g: "1"
                &attributeType,               // e.g: "texture"
                &uniform_name_buffer.front()); // e.g: "u_Diffuse" 

            webgl1es2_shader_program::active_uniform_info info;
            info.location = currentAttributeIndex;
            info.type = attributeType;
            info.size = attributeSize;

            m_ActiveUniforms[std::string(uniform_name_buffer.begin(), uniform_name_buffer.begin() + attributeNameLength)] = std::move(info);
        }
    }
}

void webgl1es2_shader_program::useProgram() const {
    static GLint s_CurrentShaderProgramHandle(-1);

    if (const auto handle(m_ProgramHandle.get()); 
        s_CurrentShaderProgramHandle != handle) {
        s_CurrentShaderProgramHandle = handle;

        s_ActiveTextureUniformNameToUnit.clear();
        s_ActiveTextureUnitCounter = 0;

        glUseProgram(handle);
    }
}

bool webgl1es2_shader_program::operator==(const webgl1es2_shader_program &b) const {
    return m_ProgramHandle == b.m_ProgramHandle;
}
bool webgl1es2_shader_program::operator!=(const webgl1es2_shader_program &b) const {return !(*this == b);}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const GLfloat aValue) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform1f(search->second.location, aValue);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_vector2_type &aValue) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform2f(search->second.location, aValue.x, aValue.y);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_vector3_type &aValue) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform3f(search->second.location, aValue.x, aValue.y, aValue.z);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_vector4_type &aValue) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform4f(search->second.location, aValue.x, aValue.y, aValue.z, aValue.w);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<GLfloat> &avalue) const {
    if (!avalue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform1fv(search->second.location, avalue.size(), &avalue[0]);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_vector2_type> &avalue) const {
    if (!avalue.size()) return false;
    
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<graphics_vector2_type::component_type> data;
        data.reserve(avalue.size() * 2);

        for (const auto &vec : avalue) {
            data.push_back(vec.x);
            data.push_back(vec.y);
        }

        glUniform2fv(search->second.location, avalue.size(), &data[0]);

        return true;
    }

    return false;
} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_vector3_type> &avalue) const {
    if (!avalue.size()) return false;
    
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<graphics_vector3_type::component_type> data;

        data.reserve(avalue.size() * 3);

        for (const auto &vec : avalue) {
            data.push_back(vec.x);
            data.push_back(vec.y);
            data.push_back(vec.z);
        }

        glUniform3fv(search->second.location, avalue.size(), &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_vector4_type> &avalue) const {
    if (!avalue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<graphics_vector4_type::component_type> data;
        data.reserve(avalue.size() * 4);

        for (const auto &vec : avalue) {
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

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const GLint aValue) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform1i(search->second.location, aValue);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string aName, const integer2_uniform_type &a) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform2i(search->second.location, a[0], a[1]);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string aName, const integer3_uniform_type &a) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform3i(search->second.location, a[0], a[1], a[2]);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string aName, const integer4_uniform_type &a) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform4i(search->second.location, a[0], a[1], a[2], a[3]);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<GLint> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform1iv(search->second.location, aValue.size(), &aValue[0]);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<integer2_uniform_type> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<integer2_uniform_type::value_type> data;
        data.reserve(aValue.size() * 2);

        for (const auto &vec : aValue) {
            data.push_back(vec[0]);
            data.push_back(vec[1]);
        }

        glUniform2iv(search->second.location, aValue.size(), &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<integer3_uniform_type> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<integer3_uniform_type::value_type> data;
        data.reserve(aValue.size() * 3);

        for (const auto &vec : aValue) {
            data.push_back(vec[0]);
            data.push_back(vec[1]);
            data.push_back(vec[2]);
        }

        glUniform3iv(search->second.location, aValue.size(), &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<integer4_uniform_type> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<integer4_uniform_type::value_type> data;
        data.reserve(aValue.size() * 4);

        for (const auto &vec : aValue) {
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

/*bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_mat2x2_type &avalue) const {

} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_mat2x2_type> &avalue) const {

} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_mat3x3_type &avalue) const {

} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_mat3x3_type> &avalue) const {

}*/

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const graphics_mat4x4_type &a) const {
    if (const auto& search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        auto b = a;
        //b.transpose();
        glUniformMatrix4fv(search->second.location, 1, GL_FALSE, &b.m[0][0]);
        return true;
    }

    return false;
} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<graphics_mat4x4_type> &a) const {
    if (!a.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        static constexpr auto order(graphics_mat4x4_type::order);

        std::vector<graphics_mat4x4_type::component_type> data;
        data.reserve(a.size() * order * order);

        for (const auto &mat : a) for (int y(0); y < order; ++y) for (int x(0); x < order; ++x) {
            data.push_back(mat.m[y][x]);
            glUniformMatrix4fv(search->second.location, 1, GL_FALSE, &data[0]);
            return true;
        }

        return false;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const gdk::webgl1es2_texture &aTexture) const {
    if (const auto &activeUniformSearch = m_ActiveUniforms.find(aName); activeUniformSearch != m_ActiveUniforms.end()) {   
        const auto &activeTextureSearch = s_ActiveTextureUniformNameToUnit.find(aName);
        
        const GLint unit = activeTextureSearch == s_ActiveTextureUniformNameToUnit.end()
            ? [&aName]() {
                decltype(unit) next_unit = s_ActiveTextureUnitCounter++;

                s_ActiveTextureUniformNameToUnit[aName] = next_unit;

                return next_unit;
            }()
            : activeTextureSearch->second;

        if (s_ActiveTextureUnitCounter < webgl1es2_shader_program::MAX_TEXTURE_UNITS) {
            aTexture.activateAndBind(unit);
            glUniform1i(activeUniformSearch->second.location, unit);
        }
        else throw gdk::graphics_exception(std::string("GLES2.0/WebGL1.0 only provide 8 texture units; "
            "you are trying to bind too many simultaneous textures to the context: ") + 
            std::to_string(s_ActiveTextureUnitCounter));

        return true;
    }

    return false;
}

std::optional<webgl1es2_shader_program::active_attribute_info> webgl1es2_shader_program::tryGetActiveAttribute(const std::string &aAttributeName) const {
    if (auto found = m_ActiveAttributes.find(aAttributeName); found != m_ActiveAttributes.end()) 
        return found->second;

    return {};
}

