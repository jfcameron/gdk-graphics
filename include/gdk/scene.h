// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SCENE_H
#define GDK_GFX_SCENE_H

#include <gdk/camera.h>
#include <gdk/model.h>

#include <set>

namespace gdk
{
    //! render scene.
    class scene
    {
    public:
        using entity_ptr_type = std::shared_ptr<entity>;

        using camera_ptr_type = std::shared_ptr<camera>;

        using camera_collection_type = std::set<camera_ptr_type>;

        /*using model_to_entity_collection = std::unsorted_map<model, std::vector<entity>>;

        using material_to_model_to_entity_collection_collection = std::unsorted_map<material, model_to_entity_collection>;*/

    private:
        //! cameras used to render this scene. cameras can be shared among multiple scenes simultaneously
        camera_collection_type m_cameras;

        //! add a camera to the scene
        void add_camera(camera_ptr_type pCamera);

        //! add an entity to the scene. 
        /// \warn Quite time complex!
        void add_entity(entity_ptr_type pEntity);

        //! draws the scene
        void draw();
    };
}

#endif
