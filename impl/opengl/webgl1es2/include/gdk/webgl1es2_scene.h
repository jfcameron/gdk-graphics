// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_SCENE
#define GDK_GFX_WEBGL1ES2_SCENE

#include <gdk/scene.h>
#include <gdk/webgl1es2_screen_camera.h>
#include <gdk/webgl1es2_texture_camera.h>
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

        //! associative collection: Models to collections of Entities - 
        /// Used to optimize GL calls
        using model_to_entity_collection = std::unordered_map<model_ptr_type, 
            std::unordered_set<entity_ptr_type>>;
        //! associative collection: Materials to {Models to collections of Entities} - 
        /// Used to optimize GL calls
        using material_to_model_to_entity_collection_collection = 
            std::unordered_map<material_ptr_type, model_to_entity_collection>;

    /// \name factory methods
    ///@{
    //
        virtual void add_camera(std::shared_ptr<screen_camera> pCamera) override;
        virtual void add_camera(std::shared_ptr<texture_camera> pCamera) override;
        
        virtual bool contains_camera(std::shared_ptr<screen_camera> pCamera) const override;
        virtual bool contains_camera(std::shared_ptr<texture_camera> pCamera) const override;
        
        virtual void remove_camera(std::shared_ptr<screen_camera> pCamera) override;
        virtual void remove_camera(std::shared_ptr<texture_camera> pCamera) override;

        virtual void add_entity(entity_ptr_type pEntity) override;

        virtual void remove_entity(entity_ptr_type pEntity) override;

        virtual void draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const override;
    ///@}

    private:
        //! a camera instance can only appear once in a given webgl1es2_scene
        using camera_collection_type = std::unordered_set<std::shared_ptr<webgl1es2_screen_camera>>;

        //! screen_cameras used to render this webgl1es2_scene.
        camera_collection_type m_screen_cameras;
        
        //! texture_cameras used to render this webgl1es2_scene.
        std::unordered_set<std::shared_ptr<webgl1es2_texture_camera>> m_texture_cameras;

        //! Nested associative array, used to optimize gl calls.
        material_to_model_to_entity_collection_collection m_MaterialToModelToEntityCollection;
    };
}

#endif
