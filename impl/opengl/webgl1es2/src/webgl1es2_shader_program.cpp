// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/glh.h>
#include <gdk/graphics/exception.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdkgraphics/buildinfo.h>

#include <atomic>
#include <iostream>
#include <sstream>
#include <vector>

using namespace gdk;
using namespace gdk::graphics;

static constexpr char TAG[] = "shader_program";

size_t webgl1es2_shader_program::MAX_TEXTURE_UNITS() {
    //! 8 is the guaranteed minimum across all es2/web1 implementations. 
    /// Can check against max but that invites the possibility of shaders working on some impls 
    /// and not others.. want to avoid that, 
    /// therefore define the "max" as the guaranteed minimum.
    ///TODO: ask the api for the number, return 8 if it says something less than that.
    ///TODO: once thats done also cache the result
    return 8;
}
size_t webgl1es2_shader_program::MAX_FRAGMENT_SHADER_INSTRUCTIONS() {
    GLint instructionLimit;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &instructionLimit);
    return instructionLimit; 
}

size_t webgl1es2_shader_program::MAX_VERTEX_SHADER_INSTRUCTIONS() {
    GLint instructionLimit;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &instructionLimit);
    return instructionLimit; 
}

std::shared_ptr<webgl1es2_shader_program> webgl1es2_shader_program::make_pink_shader_of_death() {
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

    return std::make_shared<webgl1es2_shader_program>(vertexShaderSource, fragmentShaderSource);
}

std::shared_ptr<webgl1es2_shader_program> webgl1es2_shader_program::make_alpha_cutoff() {
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
        lowp vec2 uv = (v_UV + _UVOffset) * _UVScale;  

        vec4 texel = texture2D(_Texture, uv);
        if (texel[3] < 1.0) discard;

        gl_FragColor = vec4(texel.xyz, 0.5);                        
    }
    )V0G0N");

    return std::make_shared<webgl1es2_shader_program>(vertexShaderSource, fragmentShaderSource);
}

static void perform_shader_code_preprocessing_done_to_both_vertex_and_fragment_stages(std::string &aSource) {
    aSource.insert(0, R"V0G0N(
        // access volumetric data using a 3d index that has been packed into a 2d texture
        // WARN: the length, width and height of the volumetric data must be equal and must be a power of 2!
        // aIndexOffset2d: nudge where the sampling takes place. initially set to 0,0 but if you see color issues at
        // the edges between voxels, playing with this will help to hide them
        lowp vec4 gdk_texture3D(sampler2D aSampler2D, float aVolumetricDataLength, vec2 aIndexOffset2d, ivec3 aIndex3d) {
            highp float oneDimensionIndex = float(aIndex3d.x) +
                (float(aIndex3d.y) * aVolumetricDataLength) +
                (float(aIndex3d.z) * aVolumetricDataLength * aVolumetricDataLength);

            float size2D = sqrt(aVolumetricDataLength * aVolumetricDataLength * aVolumetricDataLength);

            lowp vec2 normalizedLightVoxelCoordinates = vec2(
                (mod(oneDimensionIndex, size2D) + aIndexOffset2d.x) / size2D,
                (floor(oneDimensionIndex / size2D) + aIndexOffset2d.y) / size2D
            );
            return texture2D(aSampler2D, normalizedLightVoxelCoordinates);
        }
    )V0G0N");

    aSource.insert(0, std::string("#define ").append(gdkgraphics_BuildInfo_TargetPlatform).append("\n"));
#if defined JFC_TARGET_PLATFORM_Emscripten 
    // version must be the first line in source. version must be present for WebGL platforms
    aSource.insert(0, std::string("#version 100\n"));
#else
   // remove precison prefixes (required by wasm but not needed by GLES2)
   aSource.insert(0, std::string("#define highp\n"));
   aSource.insert(0, std::string("#define mediump\n"));
   aSource.insert(0, std::string("#define lowp\n"));
#endif
}

//! aSource is only touched on emscripten, where webgl requires an explicit float precision
static void perform_shader_code_preprocessing_done_to_only_fragment_stages(
    [[maybe_unused]] std::string &aSource) {
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

        throw gdk::graphics::exception(std::string(TAG).append(": ").append(message.str()));
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
                static_cast<GLsizei>(attributeNameBuffer.size()), 
                &attributeNameLength, 
                &attributeCount,
                &attributeType,
                &attributeNameBuffer.front());

            const std::string name(attributeNameBuffer.begin(),
                attributeNameBuffer.begin() + attributeNameLength);

            webgl1es2_shader_program::active_attribute_info info;
            info.location = glGetAttribLocation(hShaderProgram, name.c_str());
            info.type = attributeType;
            info.count = attributeCount;

            m_ActiveAttributes[name] = std::move(info);
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
                static_cast<GLsizei>(uniform_name_buffer.size()), // (c string bookkeeping) max size the gl can safely write into my buffer
                &attributeNameLength, // (c string bookkeeping) actual char activeAttributeCount for the current attribute's name
                &attributeSize, // e.g: "1"
                &attributeType, // e.g: "texture"
                &uniform_name_buffer.front()); // e.g: "u_Diffuse" 

            const std::string name(uniform_name_buffer.begin(),
                uniform_name_buffer.begin() + attributeNameLength);

            webgl1es2_shader_program::active_uniform_info info;
            info.location = glGetUniformLocation(hShaderProgram, name.c_str());
            info.type = attributeType;
            info.size = attributeSize;

            m_ActiveUniforms[name] = std::move(info);
        }

        // Resolved once rather than by string on every entity of every frame.
        const auto location_of = [this](const char *const aName) {
            const auto found = m_ActiveUniforms.find(aName);

            return found == m_ActiveUniforms.end() ? -1 : found->second.location;
        };

        m_ModelLocation = location_of("_Model");
        m_ViewLocation = location_of("_View");
        m_ProjectionLocation = location_of("_Projection");
        m_MVPLocation = location_of("_MVP");
    }
}

void webgl1es2_shader_program::set_standard_matrices(const matrix4x4_type &aModel,
    const matrix4x4_type &aView, const matrix4x4_type &aProjection,
    const matrix4x4_type &aMVP) const {
    if (m_ModelLocation != -1) glUniformMatrix4fv(m_ModelLocation, 1, GL_FALSE, &aModel.front());
    if (m_ViewLocation != -1) glUniformMatrix4fv(m_ViewLocation, 1, GL_FALSE, &aView.front());
    if (m_ProjectionLocation != -1) glUniformMatrix4fv(m_ProjectionLocation, 1, GL_FALSE, &aProjection.front());
    if (m_MVPLocation != -1) glUniformMatrix4fv(m_MVPLocation, 1, GL_FALSE, &aMVP.front());
}

void webgl1es2_shader_program::useProgram(gl_state &aState) const {
    aState.use_program(m_ProgramHandle.get());
}

bool webgl1es2_shader_program::operator==(const webgl1es2_shader_program &b) const {
    return m_ProgramHandle == b.m_ProgramHandle;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const GLfloat aValue) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform1f(search->second.location, aValue);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const vector2_type &aValue) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform2f(search->second.location, aValue.x, aValue.y);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const vector3_type &aValue) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform3f(search->second.location, aValue.x, aValue.y, aValue.z);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const vector4_type &aValue) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform4f(search->second.location, aValue.x, aValue.y, aValue.z, aValue.w);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const color &aValue) const {
    return try_set_uniform(aName, vector4_type(aValue.r, aValue.g, aValue.b, aValue.a));
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<GLfloat> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform1fv(search->second.location, static_cast<GLsizei>(aValue.size()), &aValue[0]);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<vector2_type> &aValue) const {
    if (!aValue.size()) return false;
    
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<vector2_type::component_type> data;
        data.reserve(aValue.size() * 2);

        for (const auto &vec : aValue) {
            data.push_back(vec.x);
            data.push_back(vec.y);
        }

        glUniform2fv(search->second.location, static_cast<GLsizei>(aValue.size()), &data[0]);

        return true;
    }

    return false;
} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<vector3_type> &aValue) const {
    if (!aValue.size()) return false;
    
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<vector3_type::component_type> data;

        data.reserve(aValue.size() * 3);

        for (const auto &vec : aValue) {
            data.push_back(vec.x);
            data.push_back(vec.y);
            data.push_back(vec.z);
        }

        glUniform3fv(search->second.location, static_cast<GLsizei>(aValue.size()), &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<vector4_type> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<vector4_type::component_type> data;
        data.reserve(aValue.size() * 4);

        for (const auto &vec : aValue) {
            data.push_back(vec.x);
            data.push_back(vec.y);
            data.push_back(vec.z);
            data.push_back(vec.w);
        }

        glUniform4fv(search->second.location, static_cast<GLsizei>(aValue.size()), &data[0]);

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
        glUniform2i(search->second.location, a.x, a.y);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string aName, const integer3_uniform_type &a) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform3i(search->second.location, a.x, a.y, a.z);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string aName, const integer4_uniform_type &a) const {
    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform4i(search->second.location, a.x, a.y, a.z, a.w);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<GLint> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        glUniform1iv(search->second.location, static_cast<GLsizei>(aValue.size()), &aValue[0]);
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<integer2_uniform_type> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<integer2_uniform_type::component_type> data;
        data.reserve(aValue.size() * 2);

        for (const auto &vec : aValue) {
            data.push_back(vec.x);
            data.push_back(vec.y);
        }

        glUniform2iv(search->second.location, static_cast<GLsizei>(aValue.size()), &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<integer3_uniform_type> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<integer3_uniform_type::component_type> data;
        data.reserve(aValue.size() * 3);

        for (const auto &vec : aValue) {
            data.push_back(vec.x);
            data.push_back(vec.y);
            data.push_back(vec.z);
        }

        glUniform3iv(search->second.location, static_cast<GLsizei>(aValue.size()), &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<integer4_uniform_type> &aValue) const {
    if (!aValue.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        std::vector<integer4_uniform_type::component_type> data;
        data.reserve(aValue.size() * 4);

        for (const auto &vec : aValue) {
            data.push_back(vec.x);
            data.push_back(vec.y);
            data.push_back(vec.z);
            data.push_back(vec.w);
        }

        glUniform4iv(search->second.location, static_cast<GLsizei>(aValue.size()), &data[0]);

        return true;
    }

    return false;
}

/*bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const mat2x2_type &avalue) const {

} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<mat2x2_type> &avalue) const {

} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const mat3x3_type &avalue) const {

} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<mat3x3_type> &avalue) const {

}*/

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const mat3x3_type &a) const {
    if (const auto& search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        auto b = a;
        glUniformMatrix3fv(search->second.location, 1, GL_FALSE, &b.front());
        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<mat3x3_type> &a) const {
    if (!a.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        static constexpr auto order(mat3x3_type::order);

        std::vector<mat3x3_type::component_type> data;
        data.reserve(a.size() * order * order);

        for (const auto &mat : a) for (std::size_t column(0); column < order; ++column)
            for (std::size_t row(0); row < order; ++row) data.push_back(mat.get(column, row));

        glUniformMatrix3fv(search->second.location, static_cast<GLsizei>(a.size()), GL_FALSE, &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const matrix4x4_type &a) const {
    if (const auto& search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        auto b = a;
        glUniformMatrix4fv(search->second.location, 1, GL_FALSE, &b.front());
        return true;
    }

    return false;
} 

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName, const std::vector<matrix4x4_type> &a) const {
    if (!a.size()) return false;

    if (const auto &search = m_ActiveUniforms.find(aName); search != m_ActiveUniforms.end()) {
        static constexpr auto order(matrix4x4_type::order);

        std::vector<matrix4x4_type::component_type> data;
        data.reserve(a.size() * order * order);

        for (const auto &mat : a) for (std::size_t column(0); column < order; ++column)
            for (std::size_t row(0); row < order; ++row) data.push_back(mat.get(column, row));

        glUniformMatrix4fv(search->second.location, static_cast<GLsizei>(a.size()), GL_FALSE, &data[0]);

        return true;
    }

    return false;
}

bool webgl1es2_shader_program::try_set_uniform(const std::string &aName,
    const gdk::graphics::webgl1es2_texture &aTexture, gl_state &aState) const {
    if (const auto &activeUniformSearch = m_ActiveUniforms.find(aName);
        activeUniformSearch != m_ActiveUniforms.end()) {
        const GLint unit = [&]() {
            if (const auto assigned = aState.assigned_texture_unit(aName); assigned >= 0)
                return assigned;

            if (aState.assigned_texture_unit_count() >= static_cast<short>(
                webgl1es2_shader_program::MAX_TEXTURE_UNITS()))
                throw gdk::graphics::exception(std::string("GLES2.0/WebGL1.0 only provide ")
                    .append(std::to_string(webgl1es2_shader_program::MAX_TEXTURE_UNITS()))
                    .append(" texture units; you are trying to bind too many simultaneous textures "
                        "to the context: ").append(aName));

            return aState.assign_texture_unit(aName);
        }();

        aTexture.activateAndBind(unit);
        glUniform1i(activeUniformSearch->second.location, unit);

        return true;
    }

    return false;
}

std::optional<webgl1es2_shader_program::active_attribute_info> webgl1es2_shader_program::tryGetActiveAttribute(const std::string &aAttributeName) const {
    if (auto found = m_ActiveAttributes.find(aAttributeName); found != m_ActiveAttributes.end()) 
        return found->second;

    return {};
}

