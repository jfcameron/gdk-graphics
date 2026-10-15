// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_GL_STATE_H
#define GDK_GFX_WEBGL1ES2_GL_STATE_H

#include <cstddef>
#include <gdk/graphics/string_keyed.h>
#include <gdk/graphics/opengl.h>

#include <string>
#include <string_view>
#include <unordered_map>

namespace gdk::graphics {
    class gl_state final {
    public:
        /// \brief make aHandle the program in use if it is not already
        bool use_program(const GLuint aHandle) {
            if (mCurrentProgram == aHandle) return false;

            mCurrentProgram = aHandle;
            ++mCounted.programs;

            mTextureUnits.clear();
            mTextureUnitCounter = 0;

            glUseProgram(aHandle);

            return true;
        }

        [[nodiscard]] GLuint current_program() const { return mCurrentProgram; }

        [[nodiscard]] GLint assigned_texture_unit(const std::string_view aName) const {
            const auto found = mTextureUnits.find(aName);

            return found == mTextureUnits.end() ? -1 : found->second;
        }

        [[nodiscard]] GLint assign_texture_unit(const std::string_view aName) {
            const GLint unit = mTextureUnitCounter++;

            mTextureUnits.emplace(aName, unit);

            return unit;
        }

        [[nodiscard]] short assigned_texture_unit_count() const { return mTextureUnitCounter; }

        struct counted final {
            std::size_t draws = 0;
            std::size_t programs = 0;
        };

        void count_draw() { ++mCounted.draws; }

        [[nodiscard]] counted taken() { const auto out = mCounted; mCounted = {}; return out; }

        void invalidate() {
            mCurrentProgram = 0;
            mTextureUnits.clear();
            mTextureUnitCounter = 0;
        }

    private:
        GLuint mCurrentProgram{0};
        string_keyed<GLint> mTextureUnits;
        short mTextureUnitCounter{0};
        counted mCounted;
    };
}

#endif
