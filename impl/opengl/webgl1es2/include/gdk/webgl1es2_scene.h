// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_SCENE_H
#define GDK_GFX_SCENE_H

#include <gdk/webgl1es2_camera.h>
#include <gdk/webgl1es2_model.h>
#include <gdk/webgl1es2_material.h>

#include <set>

// TODO think about implementation. currently doing a nested datastructure thing. maybe have multiple structures, 
// to both linearize and nest items. toplevel vec<entity>, then nested maps that.. index? point? into the vec? hm.
// index. list of deletions? on draw if find deletions != end, remove this, break. ? that works.. however not concurrency friendly
namespace gdk
{
    //! render scene.
    class webgl1es2_scene
    {
    public:
        //! cameras can be shared across webgl1es2_scenes
        using camera_ptr_type = std::shared_ptr<webgl1es2_camera>;
        //! entities can be shared across webgl1es2_scenes
        using entity_ptr_type = std::shared_ptr<webgl1es2_entity>;
        //! materials can be shared across webgl1es2_scenes
        using material_ptr_type = std::shared_ptr<webgl1es2_material>;
        //! models can be shared across webgl1es2_scenes
        using model_ptr_type = std::shared_ptr<webgl1es2_model>;
        //! a camera instance can only appear once in a given webgl1es2_scene
        using camera_collection_type = std::set<camera_ptr_type>;
        //! associative collection: Models to collections of Entities - Used to optimize GL calls
        using model_to_entity_collection = std::unordered_map<model_ptr_type, std::vector<entity_ptr_type>>;
        //! associative collection: Materials to {Models to collections of Entities} - Used to optimize GL calls
        using material_to_model_to_entity_collection_collection = std::unordered_map<material_ptr_type, model_to_entity_collection>;

    private:
        //! cameras used to render this webgl1es2_scene. cameras can be shared among multiple webgl1es2_scenes simultaneously
        camera_collection_type m_cameras;

        //! Associates Materials used in webgl1es2_scene with associations of Models to Entities
        material_to_model_to_entity_collection_collection m_MaterialToModelToEntityCollectionCollection;

    public:
        //! add a camera to the webgl1es2_scene
        void add_camera(camera_ptr_type pCamera);

        //! add an entity to the webgl1es2_scene. 
        /// \warn Quite time complex! Inserts the entity into a nested collection, in order to optimize opengl interactions
        void add_entity(entity_ptr_type pEntity);

        //! remove an entity from the webgl1es2_scene.
        /// \warn unimplemented.
        void remove_entity(entity_ptr_type pEntity) {} //hmmmmm

        //! draws the webgl1es2_scene
        void draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const;
    };
}

#endif
