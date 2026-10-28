// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXT_SPRITE_ANIMATION_H
#define GDK_GRAPHICS_EXT_SPRITE_ANIMATION_H

#include <gdk/graphics/data_types.h>

#include <cstddef>
#include <span>
#include <vector>

namespace gdk::graphics {
    /// \brief non-interpolated 2d sprite animation
    class sprite_animation final {
    public:
        using normalized_texture_coordinate_type = float;
        using time_type = float;
        using texel_coordinate_type = int;

        /// \brief what happens when the animation reaches its end
        enum class play_mode {
            loop, //!< start again. The default, and what an idle or walk cycle wants.
            once  //!< stop on the last frame. has_finished() then reports true.
        };

        /// \brief animation frame position and size in texels
        struct frame {
            texel_coordinate_type x;
            texel_coordinate_type y;
            texel_coordinate_type w;
            texel_coordinate_type h;
            time_type length;
        };

        /// \brief animation frame position and size in normalized texture coordinate space
        struct normalized_frame {
            normalized_texture_coordinate_type u;
            normalized_texture_coordinate_type v;
            normalized_texture_coordinate_type w;
            normalized_texture_coordinate_type h;
        };

        /// \brief the frame showing at the given time, in normalized texture coordinates.
        ///
        /// aTime may be any value. A looping animation wraps it, so a caller can pass the total
        /// time elapsed without doing the arithmetic itself; negative times wrap too. A once
        /// animation clamps instead: before it starts it shows the first frame, after it ends it
        /// holds the last.
        [[nodiscard]] normalized_frame at(time_type aTime) const;

        /// \brief whether a once animation has played out by the given time.
        ///
        /// Always false for a looping animation, which never finishes. True from the moment the
        /// last frame has been shown for its full length, which is when the animation's length has
        /// elapsed.
        [[nodiscard]] bool has_finished(time_type aTime) const;

        /// \brief how long one cycle of the animation lasts, the total of its frames' lengths
        [[nodiscard]] time_type length() const;

        /// \brief how many frames the animation has
        [[nodiscard]] std::size_t frame_count() const;

        /// \brief builds an animation from frames cut out of a sprite sheet.
        ///
        /// \param aFrameData the frames, in the order they play, positioned in texels
        /// \param aTextureWidth the sheet's width in texels
        /// \param aTextureHeight the sheet's height in texels
        ///
        /// \param aPlayMode whether it loops or stops on the last frame
        ///
        /// Throws if there are no frames, if any frame's length is not greater than zero, if the
        /// sheet's dimensions are not positive, or if a frame does not fit inside the sheet.
        sprite_animation(std::span<const frame> aFrameData,
            texel_coordinate_type aTextureWidth,
            texel_coordinate_type aTextureHeight,
            play_mode aPlayMode = play_mode::loop);

    private:
        //! when each frame starts, summed in double so that a short frame after long ones keeps a start of its own
        std::vector<double> m_Starts;

        //! already in normalized coordinates: the conversion depends only on the sheet, which cannot change
        std::vector<normalized_frame> m_Frames;

        double m_Length = 0;

        play_mode m_PlayMode = play_mode::loop;
    };
}

#endif
