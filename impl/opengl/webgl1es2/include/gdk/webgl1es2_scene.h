// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_SCENE
#define GDK_GFX_WEBGL1ES2_SCENE

#include <gdk/scene.h>
#include <gdk/webgl1es2_camera.h>
#include <gdk/webgl1es2_material.h>
#include <gdk/webgl1es2_model.h>

#include <unordered_set>

// TODO handle entity material & model changes. -> Will need to implement signals... vec<functor> likely. Maybe. This adds bookkeeping complexity, runtime complexity. it may be preferrable for the user to "change" an entities properties by removing the one you no longer want and inserting a new one with new properties.
namespace gdk
{
    //! render scene.
    class webgl1es2_scene final : public scene
    {
    public:
        //! materials can be shared across webgl1es2_scenes
        using material_ptr_type = std::shared_ptr<webgl1es2_material>;
        //! models can be shared across webgl1es2_scenes
        using model_ptr_type = std::shared_ptr<webgl1es2_model>;

        //! a camera instance can only appear once in a given webgl1es2_scene
        using camera_collection_type = std::unordered_set<camera_ptr_type>;
        //! associative collection: Models to collections of Entities - Used to optimize GL calls
        using model_to_entity_collection = std::unordered_map<model_ptr_type, std::unordered_set<entity_ptr_type>>;
        //! associative collection: Materials to {Models to collections of Entities} - Used to optimize GL calls
        using material_to_model_to_entity_collection_collection = std::unordered_map<material_ptr_type, model_to_entity_collection>;

    private:
        //! cameras used to render this webgl1es2_scene.
        camera_collection_type m_cameras;

        //! Nested associative array, used to optimize gl calls.
        material_to_model_to_entity_collection_collection m_MaterialToModelToEntityCollection;

    public:
        //! impl
        virtual bool contains_camera(camera_ptr_type pCamera) const override;
        
        //! impl
        virtual void remove_camera(camera_ptr_type pCamera) override;

        //! add a camera to the webgl1es2_scene
        virtual void add_camera(camera_ptr_type pCamera) override;

        //! add an entity to the webgl1es2_scene. 
        /// \warn Quite time complex! Inserts the entity into a nested collection, in order to optimize opengl interactions
        virtual void add_entity(entity_ptr_type pEntity) override;

        //! remove an entity from the webgl1es2_scene.
        virtual void remove_entity(entity_ptr_type pEntity) override;

        //! draws the webgl1es2_scene
        virtual void draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const override;
    };
}

#endif
