// © Joseph Cameron - All Rights Reserved

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
    //! set of objects to render
    class render_set
    {
    public:
        using entity_ptr_type = std::shared_ptr<const entity>;
        using material_ptr_type = std::shared_ptr<webgl1es2_material>;
        using model_ptr_type = std::shared_ptr<webgl1es2_model>;
        using model_to_entity_collection = std::unordered_map<model_ptr_type, 
            std::unordered_set<entity_ptr_type>>;
        using material_to_model_to_entity_collection_collection = 
            std::unordered_map<material_ptr_type, model_to_entity_collection>;

        virtual void draw(const webgl1es2_camera *r) const;
        
        virtual void try_add(entity_ptr_type);

        virtual ~render_set() = default;

    protected:
        material_to_model_to_entity_collection_collection m_MaterialToModelToEntityCollection;

        std::unordered_set<entity_ptr_type> m_unique_entities;
    };

    class sorted_render_set final : public render_set
    {
    public:
        virtual void draw(const webgl1es2_camera *r) const;

        virtual void try_add(entity_ptr_type) override;

        sorted_render_set() = default;

    private:
        
    };

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
            std::unordered_set<std::shared_ptr<const entity>>>;
        //! associative collection: Materials to {Models to collections of Entities} - 
        /// Used to optimize GL calls
        using material_to_model_to_entity_collection_collection = 
            std::unordered_map<material_ptr_type, model_to_entity_collection>;

    /// \name external interface
    ///@{
    //
        //TODO: these fail silently on nullptr. should probably throw?
        virtual void add(const std::shared_ptr<const screen_camera> &pCamera) override;
        virtual void add(const std::shared_ptr<const texture_camera> &pCamera) override;
        virtual void add(const std::shared_ptr<const entity> &pEntity) override;
        
        virtual void remove(const std::shared_ptr<const screen_camera> &pCamera) override;
        virtual void remove(const std::shared_ptr<const texture_camera> &pCamera) override;
        virtual void remove(const std::shared_ptr<const entity> &pEntity) override;

        virtual void draw(const gdk::graphics_intvector2_type &aFrameBufferSize) const override;
    ///@}

    private:
        //! screen_cameras used to render this webgl1es2_scene.
        std::unordered_set<std::shared_ptr<const webgl1es2_screen_camera>> m_screen_cameras;
        
        //! texture_cameras used to render this webgl1es2_scene.
        std::unordered_set<std::shared_ptr<const webgl1es2_texture_camera>> m_texture_cameras;

        //! Nested associative array, used to optimize gl calls.
        material_to_model_to_entity_collection_collection m_MaterialToModelToEntityCollection;

        render_set m_opaque_set;

        sorted_render_set m_translucent_set;
    };
}

#endif
