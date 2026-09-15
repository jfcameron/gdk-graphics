// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_WEBGL1ES2_SCENE
#define GDK_GFX_WEBGL1ES2_SCENE

#include <gdk/graphics/webgl1es2_frustum.h>
#include <gdk/graphics/webgl1es2_gl_state.h>

#include <memory>
#include <gdk/graphics/scene.h>
#include <gdk/graphics/string_keyed.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_screen_camera.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>

#include <unordered_set>
#include <vector>

namespace gdk::graphics {
    /// \brief values a scene gives every program it draws with. \see scene::set_vector3
    class scene_uniforms final {
    public:
        //! uploads every value the program declares, and skips the rest
        void apply(const webgl1es2_shader_program &aProgram) const;

        string_keyed<float> floats;
        string_keyed<vector2_type> vector2s;
        string_keyed<vector3_type> vector3s;
        string_keyed<vector4_type> vector4s;
        string_keyed<int> integers;
    };

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

        /// `aViewProjection` is projection * view for this camera, built once by the caller
        /// rather than per entity
        ///
        /// `aUniforms` go up before each material's own, so a material's value for a name wins
        virtual void draw(const webgl1es2_camera *r, gl_state &aState,
            const frustum &aFrustum, const matrix4x4_type &aViewProjection,
            const scene_uniforms &aUniforms) const;

        virtual void try_add(entity_ptr_type);

        virtual void remove(const entity *apEntity);

        [[nodiscard]] std::size_t entity_count() const;

        virtual ~render_set() = default;

    protected:
        /// \brief remove dead pointers
        /// \brief drop entries whose entity is gone
        void prune() const;

        static constexpr std::size_t PRUNE_INTERVAL{64};

        /// \brief take the slot for an entity, over a dead one at the same address if need be
        [[nodiscard]] bool claim(const entity_ptr_type &apEntity) const;

        mutable std::size_t m_SinceLastPrune{0};

        mutable std::unordered_map<const entity *, entity_weak_ptr_type> m_unique_entities;

        mutable material_to_model_to_entity_collection_collection
            m_MaterialToModelToEntityCollection;
    };

    class sorted_render_set final : public render_set {
    public:
        virtual void draw(const webgl1es2_camera *r, gl_state &aState,
            const frustum &aFrustum, const matrix4x4_type &aViewProjection,
            const scene_uniforms &aUniforms) const override;

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
        virtual void add(const std::shared_ptr<const screen_camera> &pCamera) override;
        virtual void add(const std::shared_ptr<const texture_camera> &pCamera) override;
        virtual void add(const std::shared_ptr<const entity> &pEntity) override;
        
        virtual void remove(const std::shared_ptr<const screen_camera> &pCamera) override;
        virtual void remove(const std::shared_ptr<const texture_camera> &pCamera) override;
        virtual void remove(const std::shared_ptr<const entity> &pEntity) override;

        virtual void draw(const gdk::graphics::intvector2_type &aFrameBufferSize) const override;

        virtual void set_float(const std::string_view aName, float aValue) override;
        virtual void set_vector2(const std::string_view aName, vector2_type aValue) override;
        virtual void set_vector3(const std::string_view aName, vector3_type aValue) override;
        virtual void set_vector4(const std::string_view aName, vector4_type aValue) override;
        virtual void set_vector4(const std::string_view aName, const color &aValue) override;
        virtual void set_integer(const std::string_view aName, int aValue) override;
    ///@}

    private:
        mutable std::vector<std::weak_ptr<const webgl1es2_screen_camera>> m_screen_cameras;
        
        mutable std::vector<std::weak_ptr<const webgl1es2_texture_camera>> m_texture_cameras;

        material_to_model_to_entity_collection_collection m_MaterialToModelToEntityCollection;

        std::shared_ptr<gl_state> m_pState;

        scene_uniforms m_Uniforms;

        render_set m_opaque_set;

        sorted_render_set m_translucent_set;
    };
}

#endif
