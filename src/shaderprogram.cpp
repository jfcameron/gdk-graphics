// © 2018 Joseph Cameron - All Rights Reserved

#include <gdkgraphics/buildinfo.h>

#include <gdk/glh.h>
#include <gdk/nlohmann_json_util.h>
#include <gdk/shaderprogram.h>

#include <nlohmann/json.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace gdk;

static constexpr char TAG[] = "ShaderProgram";

const gdk::lazy_ptr<gdk::ShaderProgram> ShaderProgram::PinkShaderOfDeath([]()
{
    const std::string vertexShaderSource = R"V0G0N(    
    //Uniforms
    uniform mat4 _MVP;

#if defined Emscripten
    //VertIn
    attribute highp vec3 a_Position;
    
#elif defined Darwin || defined Windows || defined Linux
    //VertIn
    attribute vec3 a_Position;
#endif

    void main ()
    {
        gl_Position = _MVP * vec4(a_Position,1.0);
    }
)V0G0N";

    const std::string fragmentShaderSource = R"V0G0N(
#if defined Emscripten
    precision mediump float;
#endif
            
    const vec4 DEATHLY_PINK = vec4(1,0.2,0.8,1);

    void main()
    {
        gl_FragColor = DEATHLY_PINK;
    }
)V0G0N";

    return new gdk::ShaderProgram("PinkShaderOfDeath", vertexShaderSource, fragmentShaderSource);
});

const gdk::lazy_ptr<gdk::ShaderProgram> ShaderProgram::AlphaCutOff([]()
{
    const std::string vertexShaderSource = R"V0G0N(
    //Uniforms
    uniform mat4 _MVP;
    uniform mat4 _Model; //separate mats arent used. should probably delete. they are useful though
    uniform mat4 _View;
    uniform mat4 _Projection;
    
    // Programmable stage input formats. Consider how to clean this up.. the webgl requirement of precison prefixes.
#if defined Emscripten
    //VertIn
    attribute highp   vec3 a_Position;
    attribute mediump vec2 a_UV;

    //FragIn
    varying mediump vec2 v_UV;

#elif defined Darwin || defined Windows || defined Linux
    //VertIn
    attribute vec3 a_Position;
    attribute vec2 a_UV;

    //FragIn
    varying vec2 v_UV;
#endif

    void main ()
    {
        gl_Position = _MVP * vec4(a_Position,1.0);
               
        v_UV = a_UV;
    }
)V0G0N";

    const std::string fragmentShaderSource = R"V0G0N(
#if defined Emscripten
    precision mediump float;
#endif
            
    //Uniforms
    uniform sampler2D _Texture;

#if defined Emscripten
    //FragIn
    varying lowp vec2 v_UV;

#elif defined Darwin || defined Windows || defined Linux
    //FragIn
    varying vec2 v_UV;
#endif
    
    void main()
    {
        vec4 frag = texture2D(_Texture, v_UV);
                
        if (frag[3] < 1.0) discard;
                
        gl_FragColor = frag;                        
    }
)V0G0N";
    return new gdk::ShaderProgram("AlphaCutOff", vertexShaderSource, fragmentShaderSource);
});

std::ostream &gdk::operator<<(std::ostream &s, const ShaderProgram &a) 
{
    return s << nlohmann::json
    {
        {"Type", TAG},
        {"Debug Info", //This part is expensive. Should only be written if some symbol is defined etc. "Debug Info" should also be standardized.
            {"Active Attributes", [&]()
                {
                    GLint activeAttribs = 0;

                    glGetProgramiv(a.m_ProgramHandle, GL_ACTIVE_ATTRIBUTES, &activeAttribs);

                    return activeAttribs;
                }()
            },
            {"Active Uniforms", [&]()
                {
                    GLint activeUniforms = 0;

                    glGetProgramiv(a.m_ProgramHandle, GL_ACTIVE_UNIFORMS, &activeUniforms);

                    return activeUniforms;
                }()
            },
        },
        
        {"m_Name", jfc::insertion_operator_to_nlohmann_json_object(a.m_Name)},
        {"m_ProgramHandle", jfc::insertion_operator_to_nlohmann_json_object(a.m_ProgramHandle)},
    }
    .dump();
}

ShaderProgram::ShaderProgram(const std::string &aName, std::string aVertexSource, std::string aFragmentSource)
: m_Name(aName)
, m_ProgramHandle([&]()
{
    aVertexSource.insert  (0, std::string("#define ").append(gdkgraphics_BuildInfo_TargetPlatform).append("\n"));
    aFragmentSource.insert(0, std::string("#define ").append(gdkgraphics_BuildInfo_TargetPlatform).append("\n"));

#if defined JFC_TARGET_PLATFORM_Emscripten // version must be the first line in source. version must be present for WebGL platforms
    aVertexSource.insert  (0, std::string("#version 100\n"));
    aFragmentSource.insert(0, std::string("#version 100\n"));
#endif

    // Compile vertex stage
    const char *const vertex_shader = aVertexSource.c_str();
    const GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, 0);
    glCompileShader(vs);
 
    // Compile fragment stage
    const char *const fragment_shader = aFragmentSource.c_str();
    const GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, 0);
    glCompileShader(fs);
    
    // Link the program
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vs);
    glAttachShader(programHandle, fs);
    glLinkProgram(programHandle);
    
    GLint status = -1;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
    
    if (status == GL_FALSE)
    {
        std::ostringstream message;
        
        message << "The shader: \"" << aName << "\" has failed to compile!" << std::endl
        << std::endl << "program compilation log: " <<         glh::GetProgramInfoLog(programHandle) << std::endl
        << std::endl << "vertex shader compilation log: " <<   glh::GetShaderInfoLog(vs) << std::endl
        << std::endl << "fragment shader compilation log: " << glh::GetShaderInfoLog(fs);
        
        //gdk::log(TAG, glh::GetShaderInfoLog(vs), ", ", glh::GetShaderInfoLog(fs)); // ??????????????

        throw std::runtime_error(std::string(TAG).append(message.str()));
    }

    return programHandle;
}())
{}

ShaderProgram::ShaderProgram(ShaderProgram &&aShaderProgram) 
{
    m_Name =          std::move(aShaderProgram.m_Name);
    m_ProgramHandle = std::move(aShaderProgram.m_ProgramHandle);
    
    aShaderProgram.m_ProgramHandle = 0;
}

ShaderProgram::~ShaderProgram() 
{
    if (m_ProgramHandle > 0) glDeleteProgram(m_ProgramHandle);
}

GLuint ShaderProgram::useProgram() const 
{
    glUseProgram(m_ProgramHandle);
    //glDrawCalls();
    
    return m_ProgramHandle;
}

std::string ShaderProgram::getName() const 
{
    return m_Name;
}
    
GLuint ShaderProgram::getHandle() const 
{
    return m_ProgramHandle;
}
