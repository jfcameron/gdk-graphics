// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SCENE_H
#define GDK_GFX_SCENE_H

namespace gdk
{
    class camera;

    /// \brief a 3d environment within which cameras rasterize entities
    ///
    /// manages api calls to render a scene
    class scene
    {
    public:
        using camera_ptr_type = std::shared_ptr<camera>;

        //! add a camera to the webgl1es2_scene
        virtual void add_camera(camera_ptr_type pCamera) = 0;
    };
}

#endif
