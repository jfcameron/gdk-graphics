// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SCENE_H
#define GDK_GFX_SCENE_H

#include <gdk/graphics/color.h>
#include <gdk/graphics/types.h>

#include <memory>
#include <string_view>

namespace gdk::graphics {
    class entity;
    class screen_camera;
    class texture_camera;

    /// \brief a 3d environment within which cameras draw entities
    class scene {
    public:
        //! add a screen camera to the scene
        virtual void add(const std::shared_ptr<const screen_camera> &pCamera) = 0;
        //! remove a screen camera from the scene
        virtual void remove(const std::shared_ptr<const screen_camera> &pCamera) = 0;

        //! add a texture camera to the scene
        virtual void add(const std::shared_ptr<const texture_camera> &pCamera) = 0;
        //! remove a texture camera from the scene
        virtual void remove(const std::shared_ptr<const texture_camera> &pCamera) = 0;
        
        /// \brief add an entity to the scene
        ///
        /// Adding the same entity twice does nothing the second time.
        virtual void add(const std::shared_ptr<const entity> &pEntity) = 0;
        /// \brief remove an entity from the scene
        ///
        /// note this is only necessary if you want to remove an entity from the scene while
        /// keeping the instance alive. the scene only weakly owns the entities inside of it
        virtual void remove(const std::shared_ptr<const entity> &pEntity) = 0;

        //! draws the scene
        virtual void draw(const gdk::graphics::intvector2_type &aFrameBufferSize) const = 0;

        /// \name Uniforms for everything the scene draws
        /// @{
        ///
        /// **A value set here reaches every material drawn in this scene whose shader declares the
        /// name**, so a value that belongs to the whole scene eg the sun, the time, is set once
        /// rather than on every material that uses it. 
        ///
        /// **A material's own value for the same name wins.** The scene's is the general case
        /// and the material's the specific one, so a material can override; the scene's values
        /// go up first and the material's over them.
        ///
        virtual void set_float(const std::string_view aName, float aValue) = 0;
        virtual void set_vector2(const std::string_view aName, vector2_type aValue) = 0;
        virtual void set_vector3(const std::string_view aName, vector3_type aValue) = 0;
        virtual void set_vector4(const std::string_view aName, vector4_type aValue) = 0;
        virtual void set_vector4(const std::string_view aName, const color &aValue) = 0;
        virtual void set_integer(const std::string_view aName, int aValue) = 0;
        /// @}

        virtual ~scene() = default;
    };
}

#endif

