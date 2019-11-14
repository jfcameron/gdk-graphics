// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_UNIFORM_MANAGER_H
#define GDK_GFX_UNIFORM_MANAGER_H

#include <unordered_map>
#include <string>

#include <gdk/shader_program.h>

namespace gdk
{
    class uniform_manager final
    {
    public:

    private:
        //1,2,3,4f. 1,2,3,4i
        std::unordered_map<std::string, float> m_FloatUniforms;

    public:
        /// \brief attempts to assign uniform values to the currently in-use shader program
        /// data is buffered. hmm. argnhnerngh.
        void bind(const GLuint aProgramHandle) const;

        uniform_manager() = default;
    };
}

#endif
