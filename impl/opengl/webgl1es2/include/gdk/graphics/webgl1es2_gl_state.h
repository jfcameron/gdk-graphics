// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_GL_STATE_H
#define GDK_GFX_WEBGL1ES2_GL_STATE_H

#include <gdk/graphics/string_keyed.h>
#include <gdk/graphics/opengl.h>

#include <string>
#include <string_view>
#include <unordered_map>

namespace gdk::graphics {
    /// \brief what one gl context currently has set, so redundant calls can be skipped
    class gl_state final {
    public:
        /// \brief make aHandle the program in use if it is not already
        bool use_program(const GLuint aHandle) {
            if (mCurrentProgram == aHandle) return false;

            mCurrentProgram = aHandle;

            mTextureUnits.clear();
            mTextureUnitCounter = 0;

            glUseProgram(aHandle);

            return true;
        }

        //! the program this context has in use or 0 for none yet
        [[nodiscard]] GLuint current_program() const { return mCurrentProgram; }

        //! the unit already assigned to aName in the program in use or -1
        [[nodiscard]] GLint assigned_texture_unit(const std::string_view aName) const {
            const auto found = mTextureUnits.find(aName);

            return found == mTextureUnits.end() ? -1 : found->second;
        }

        //! take the next free unit for aName
        [[nodiscard]] GLint assign_texture_unit(const std::string_view aName) {
            const GLint unit = mTextureUnitCounter++;

            mTextureUnits.emplace(aName, unit);

            return unit;
        }

        [[nodiscard]] short assigned_texture_unit_count() const { return mTextureUnitCounter; }

        /// \brief clears all cached values
        void invalidate() {
            mCurrentProgram = 0;
            mTextureUnits.clear();
            mTextureUnitCounter = 0;
        }

    private:
        GLuint mCurrentProgram{0};
        string_keyed<GLint> mTextureUnits;
        short mTextureUnitCounter{0};
    };
}

#endif
