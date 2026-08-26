// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_SCENE
#define GDK_GFX_WEBGL1ES2_SCENE

#include <gdk/graphics/webgl1es2_frustum.h>
#include <gdk/graphics/webgl1es2_gl_state.h>

#include <memory>
#include <gdk/graphics/scene.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_screen_camera.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>

#include <unordered_set>
#include <vector>

// TODO handle entity material & model changes. -> Will need to implement signals... vec<functor> likely. Maybe. This adds bookkeeping complexity, runtime complexity. it may be preferrable for the user to "change" an entities properties by removing the one you no longer want and inserting a new one with new properties.
namespace gdk::graphics {
    /// \brief set of objects to render grouped so that gl state changes as little as possible
    class render_set {
    public:
        using entity_ptr_type = std::shared_ptr<const entity>;
        using entity_weak_ptr_type = std::weak_ptr<const entity>;
        using material_ptr_type = std::shared_ptr<webgl1es2_material>;
        using model_ptr_type = std::shared_ptr<webgl1es2_model>;

        using model_to_entity_collection = std::unordered_map<model_ptr_type,
            std::vector<entity_weak_ptr_type>>;

        using material_to_model_to_entity_collection_collection =
            std::unordered_map<material_ptr_type, model_to_entity_collection>;

        virtual void draw(const webgl1es2_camera *r, gl_state &aState,
            const frustum &aFrustum) const;

        virtual void try_add(entity_ptr_type);

        virtual void remove(const entity *apEntity);

        [[nodiscard]] std::size_t entity_count() const;

        virtual ~render_set() = default;

    protected:
        /// \brief remove dead pointers
        void prune() const;

        mutable std::unordered_map<const entity *, entity_weak_ptr_type> m_unique_entities;

        mutable material_to_model_to_entity_collection_collection
            m_MaterialToModelToEntityCollection;
    };

    class sorted_render_set final : public render_set {
    public:
        virtual void draw(const webgl1es2_camera *r, gl_state &aState,
            const frustum &aFrustum) const override;

        virtual void try_add(entity_ptr_type) override;

        sorted_render_set() = default;

    private:
        
    };

    class webgl1es2_scene final : public scene {
    public:
        /// \param apState the gl context's cache, owned by the webgl1es2_context that made this scene
        explicit webgl1es2_scene(std::shared_ptr<gl_state> apState);

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

        virtual void draw(const gdk::graphics::intvector2_type &aFrameBufferSize) const override;
    ///@}

    private:
        //! screen_cameras used to render this webgl1es2_scene.
        //! cameras are held weakly for the same reason entities are. \see render_set
        mutable std::vector<std::weak_ptr<const webgl1es2_screen_camera>> m_screen_cameras;
        
        //! texture_cameras used to render this webgl1es2_scene.
        mutable std::vector<std::weak_ptr<const webgl1es2_texture_camera>> m_texture_cameras;

        //! Nested associative array, used to optimize gl calls.
        material_to_model_to_entity_collection_collection m_MaterialToModelToEntityCollection;

        //! the gl context this scene draws through. \see gl_state
        std::shared_ptr<gl_state> m_pState;

        render_set m_opaque_set;

        sorted_render_set m_translucent_set;
    };
}

#endif
