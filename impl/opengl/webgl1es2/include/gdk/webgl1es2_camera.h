// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_CAMERA_H
#define GDK_GFX_WEBGL1ES2_CAMERA_H

#include <gdk/camera.h>
#include <gdk/color.h>
#include <gdk/graphics_types.h>

#include <memory>
#include <vector>

namespace gdk
{
    class entity;

    /// \brief webgl1es2_camera implementation of camera
    class webgl1es2_camera final : public camera
    {
        /// \brief Describes camera clear behaviour: which buffers in the current FBO should be cleared?
        enum class ClearMode 
        {
            Nothing, //!< Do not clear any buffers
            ColorAndDepth, //!< Clear the color and depth buffers
            DepthOnly //!< Clear the Depth buffer
        };

    public: //private: //TODO: set this back to private
        /// \brief position of the camera viewport within the device viewport
        graphics_vector2_type m_ViewportPosition = graphics_vector2_type::Zero;

        /// \brief size of camera viewport within the device viewport
        graphics_vector2_type m_ViewportSize = graphics_vector2_type(1, 1); 
        
        /// \brief World position of camera
        graphics_mat4x4_type m_ViewMatrix = graphics_mat4x4_type::Identity; 

        /// \brief Projection of the camera
        graphics_mat4x4_type m_ProjectionMatrix = graphics_mat4x4_type::Identity; 

        /// \brief Determines which buffers in the FBO to clear before drawing
        ClearMode m_ClearMode = ClearMode::ColorAndDepth;

        /// \brief The color to replace all data in the color buffer with on clear
        gdk::color m_ClearColor = color::CornflowerBlue;

        //TODO: support render texture
        //Rendertexture m_Rendertexture;

	public:
		/// \brief sets the projection matrix to a perspective projection
		virtual void set_projection(const float aFieldOfView,
			const float aNearClippingPlane,
			const float aFarClippingPlane,
			const float aViewportAspectRatio) override;

		/// \brief sets the normalized size and position of the viewport within of the window
		virtual void set_viewport(const float aX,
			const float aY,
			const float aWidth,
			const float aHeight) override;

		//! gets the view matrix
		virtual graphics_mat4x4_type getViewMatrix() const override;

		//! gets the projection matrix
		virtual graphics_mat4x4_type getProjectionMatrix() const override;

		virtual void set_clear_color(const gdk::color& acolor) override;

        //TODO: implement a user-defined way to throw away entities that will not contribute frags (ie behind the camera) before they are uploaded.
        // Would save the pipeline a great deal of unnecessary work.
		// I think the default functor would be a noop, as its hard to decide what a universally useful one would be.
		// The best general ones i can think of would require a physics engine, to do bounds checking on shapes that roughly
		// correspond to the model. This is way out of scope for this graphics renderer, so I think the cull functor should be
		// entirely user defined.
        //void setCullFunctor(std::function<bool(pModel, ... ?, pUserData?)> cullFunc)

        /// \brief set clear color
		/// \warn Marked for deletion
        void setClearcolor(const gdk::color &acolor);

        /// \brief sets the top left position of the viewport within the screen
        void setViewportPosition(const graphics_vector2_type &a);
        /// \brief override
        void setViewportPosition(const float x, const float y);

        /// \brief set size of viewport in surface
        void setViewportSize(const graphics_vector2_type &a);
        /// \brief override
        void setViewportSize(const float x, const float y);

        /// \brief rebuilds the view matrix from a 3d position and rotation
        virtual void set_view_matrix(const gdk::graphics_vector3_type &aWorldPos, const gdk::graphics_quaternion_type &aRotation) override;

        /// \brief sets projection matrix via copy
        void setProjection(const graphics_mat4x4_type &matrix);

        /// \brief set projection matrix from orthographic bounds
        //void setProject(height, width, depth);

        /// \brief activates this camera; mutates gl according to this camera's state
        //TODO: although technically `const`able, this does mutate gl state. not logically const.
        void activate(const gdk::graphics_intvector2_type &aFrameBufferSize) const;

        /// \brief copy semantics
        webgl1es2_camera(const webgl1es2_camera &a) = default;
        /// \brief copy semantics
        webgl1es2_camera &operator=(const webgl1es2_camera &a) = default;
       
        /// \brief move semantics
        webgl1es2_camera(webgl1es2_camera &&) = default;
        /// \brief move semantics
        webgl1es2_camera &operator=(webgl1es2_camera &&a) = default;

        /// \brief constructs a webgl1es2_camera with reasonable default values
        webgl1es2_camera();
    };
}

#endif
