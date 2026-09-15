// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_NULL_CONTEXT_H
#define GDK_GFX_NULL_CONTEXT_H

#include <gdk/graphics/context.h>
#include <gdk/graphics/texture_data.h>

#include <array>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace gdk::graphics {
    class null_context final : public graphics::context {
    public:
        [[nodiscard]] static std::shared_ptr<null_context> make();

        [[nodiscard]] virtual scene_ptr_type make_scene() override;
        [[nodiscard]] virtual camera_ptr_type make_camera() override;
        [[nodiscard]] virtual texture_camera_ptr_type make_texture_camera() override;

        [[nodiscard]] virtual entity_ptr_type make_entity(const const_model_ptr_type pModel,
            const const_material_ptr_type pMaterial) override;

        [[nodiscard]] virtual model_ptr_type make_model() override;

        [[nodiscard]] virtual model_ptr_type make_model(const gdk::graphics::model::usage_hint,
            const model_data &aModelData) override;

        [[nodiscard]] virtual material_ptr_type make_material(const const_shader_ptr_type pShader,
            const material::render_mode aRenderMode,
            const material::face_culling_mode aFaceCullingMode) override;

        [[nodiscard]] virtual material_ptr_type make_material(
            const const_material_ptr_type &aPrototype) override;

        [[nodiscard]] virtual texture_ptr_type make_texture(
            const texture_data::view &aTextureDataView,
            const texture::wrap_mode aWrapModeU,
            const texture::wrap_mode aWrapModeV,
            const texture::filter_mode aFilterMode = texture::filter_mode::sharp) override;

        [[nodiscard]] virtual texture_ptr_type make_texture() override;

        /// \brief how many textures this context has been asked to create.
        [[nodiscard]] std::size_t texture_count() const;

        [[nodiscard]] virtual model_ptr_type make_cube_model() const override;
        [[nodiscard]] virtual model_ptr_type make_sphere_model() const override;

        [[nodiscard]] virtual shader_ptr_type make_alpha_cutoff_shader() const override;

        [[nodiscard]] virtual shader_ptr_type make_alpha_blend_shader() const override;

        [[nodiscard]] virtual size_t max_texture_size() const override;

        //! accepts any source at all, because nothing compiles it
        [[nodiscard]] shader_ptr_type make_shader(const std::string_view aVertexSource,
            const std::string_view aFragmentSource);

        //! how many of each resource this context has handed out
        [[nodiscard]] std::size_t resources_made() const;

    private:
        std::size_t mTextureCount{0};
        null_context() = default;

        std::size_t mResourcesMade{0};
    };

    //! \see null_context
    class null_shader final : public shader_program {
    public:
        null_shader(std::string aVertexSource = {}, std::string aFragmentSource = {});

        [[nodiscard]] const std::string &vertex_source() const;
        [[nodiscard]] const std::string &fragment_source() const;

    private:
        std::string mVertexSource;
        std::string mFragmentSource;
    };

    //! \see null_context
    class null_texture final : public texture {
    public:
        null_texture() = default;
        null_texture(const texture_data::view &aView, const wrap_mode aU, const wrap_mode aV);

        virtual void update_data(const texture_data::view &aView) override;

        virtual void update_data(const texture_data::view &aView, const size_t aOffsetX,
            const size_t aOffsetY) override;

        [[nodiscard]] std::size_t width() const;
        [[nodiscard]] std::size_t height() const;

        //! how many times data has been handed to this texture, including at construction
        [[nodiscard]] std::size_t upload_count() const;

        /// \brief the texture's content: every upload applied, a row at a time from the first
        [[nodiscard]] const texture_data::channel_data &data() const;

    private:
        std::size_t mWidth{0};
        std::size_t mHeight{0};
        std::size_t mUploadCount{0};
        std::size_t mChannels{0};
        texture_data::channel_data mData;
        wrap_mode mWrapU{wrap_mode::repeat};
        wrap_mode mWrapV{wrap_mode::repeat};
    };

    //! \see null_context
    class null_model final : public model {
    public:
        null_model() = default;
        null_model(const usage_hint aUsage, const model_data &aModelData);

        virtual void upload(const usage_hint &aUsage, const model_data &aModelData) override;

        //! vertices in the most recent upload
        [[nodiscard]] std::size_t vertex_count() const;

        //! indices in the most recent upload; zero if it was not indexed
        [[nodiscard]] std::size_t index_count() const;

        [[nodiscard]] std::size_t upload_count() const;

        /// \brief the most recent upload, kept whole
        [[nodiscard]] const model_data &last_upload() const;

    private:
        std::size_t mVertexCount{0};
        std::size_t mIndexCount{0};
        std::size_t mUploadCount{0};
        model_data mLastUpload{};
    };

    //! \see null_context
    class null_material final : public material {
    public:
        explicit null_material(const_shader_ptr_type pShader = nullptr,
            const render_mode aRenderMode = render_mode::opaque,
            const face_culling_mode aFaceCullingMode = face_culling_mode::none);

        virtual void set_texture(const std::string_view aName, texture_ptr_type aValue) override;
        virtual void set_float(const std::string_view aName, float aValue) override;
        virtual void set_vector2(const std::string_view aName, vector2_type aValue) override;
        virtual void set_vector3(const std::string_view aName, vector3_type aValue) override;
        virtual void set_vector4(const std::string_view aName, vector4_type aValue) override;
        virtual void set_vector4(const std::string_view aName, const color &aValue) override;
        virtual void set_integer(const std::string_view aName, int aValue) override;
        virtual void set_integer2(const std::string_view aName, int a1, int a2) override;
        virtual void set_integer3(const std::string_view aName, int a1, int a2, int a3) override;
        virtual void set_integer4(const std::string_view aName, int a1, int a2, int a3, int a4) override;

        virtual void set_int_vector2_array(const std::string_view aName,
            const std::vector<intvector2_type> &aValue) override;

        [[nodiscard]] std::optional<float> float_at(const std::string &aName) const;
        [[nodiscard]] std::optional<vector4_type> vector4_at(const std::string &aName) const;
        [[nodiscard]] texture_ptr_type texture_at(const std::string &aName) const;

        //! every uniform name this material has been given a value for, in a stable order
        [[nodiscard]] std::vector<std::string> uniforms() const;

        [[nodiscard]] render_mode mode() const;

    private:
        const_shader_ptr_type mpShader;
        render_mode mRenderMode;
        face_culling_mode mFaceCullingMode;

        std::map<std::string, float> mFloats;
        std::map<std::string, vector4_type> mVector4s;
        std::map<std::string, texture_ptr_type> mTextures;
        std::set<std::string> mNames;
    };

    //! \see null_context
    class null_entity final : public entity {
    public:
        null_entity(const_model_ptr_type pModel, const_material_ptr_type pMaterial);

        virtual void hide() override;
        virtual void show() override;
        [[nodiscard]] virtual bool is_hidden() const override;

        virtual void set_transform(const vector3_type &aWorldPos,
            const quaternion_type &aRotation, const vector3_type &aScale) override;

        virtual void set_transform(const matrix4x4_type &aTransform) override;

        [[nodiscard]] const matrix4x4_type &transform() const;
        [[nodiscard]] const_model_ptr_type model() const;
        [[nodiscard]] const_material_ptr_type material() const;

    private:
        const_model_ptr_type mpModel;
        const_material_ptr_type mpMaterial;

        matrix4x4_type mTransform;
        bool mHidden{false};
    };

    //! \see null_context
    class null_camera final : public screen_camera {
    public:
        virtual void set_perspective_projection(const floating_point_type aFieldOfView,
            const floating_point_type aNear, const floating_point_type aFar,
            const floating_point_type aAspectRatio) override;

        virtual void set_orthographic_projection(const vector2_type &aOrthographicSize,
            const floating_point_type aNear, const floating_point_type aFar,
            const floating_point_type aAspectRatio) override;

        virtual void set_transform(const matrix4x4_type &aTransform) override;

        virtual void set_transform(const vector3_type &aWorldPos,
            const quaternion_type &aRotation) override;

        virtual void set_clear_color(const color &aColor) override;
        virtual void set_clear_mode(const clear_mode aClearMode) override;

        virtual void set_viewport(const float aX, const float aY,
            const float aWidth, const float aHeight) override;

        virtual void set_scissor(const float aX, const float aY,
            const float aWidth, const float aHeight) override;

        virtual void clear_scissor() override;

        //! \brief the rectangle this camera would clip to
        [[nodiscard]] std::array<float, 4> scissor() const;

        [[nodiscard]] const vector3_type &position() const;
        [[nodiscard]] const quaternion_type &rotation() const;
        [[nodiscard]] const matrix4x4_type &transform() const;
        [[nodiscard]] const color &clear_color() const;
        [[nodiscard]] clear_mode mode() const;

        //! vertical field of view in radians
        [[nodiscard]] floating_point_type field_of_view() const;
        [[nodiscard]] floating_point_type aspect_ratio() const;
        [[nodiscard]] floating_point_type near_clip() const;
        [[nodiscard]] floating_point_type far_clip() const;

    private:
        vector3_type mPosition;
        quaternion_type mRotation;
        matrix4x4_type mTransform;
        color mClearColor{color::black};
        clear_mode mClearMode{clear_mode::color_and_depth};

        floating_point_type mFieldOfView{0};
        floating_point_type mAspectRatio{0};
        floating_point_type mNear{0};
        floating_point_type mFar{0};

        //! x, y, width, height, normalised as the interface takes them
        std::array<float, 4> mViewport{0, 0, 1, 1};

        std::optional<std::array<float, 4>> mScissor;
    };

    //! \see null_context
    class null_texture_camera final : public texture_camera {
    public:
        null_texture_camera();

        virtual void set_perspective_projection(const floating_point_type aFieldOfView,
            const floating_point_type aNear, const floating_point_type aFar,
            const floating_point_type aAspectRatio) override;

        virtual void set_orthographic_projection(const vector2_type &aOrthographicSize,
            const floating_point_type aNear, const floating_point_type aFar,
            const floating_point_type aAspectRatio) override;

        virtual void set_transform(const matrix4x4_type &aTransform) override;

        virtual void set_transform(const vector3_type &aWorldPos,
            const quaternion_type &aRotation) override;

        virtual void set_clear_color(const color &aColor) override;
        virtual void set_clear_mode(const clear_mode aClearMode) override;

        [[nodiscard]] virtual const std::shared_ptr<texture> get_color_texture(
            const size_t i = 0) const override;

        [[nodiscard]] virtual const std::shared_ptr<texture> get_depth_texture() const override;

    private:
        std::shared_ptr<texture> mpColor;
        std::shared_ptr<texture> mpDepth;
    };

    //! \see null_context
    class null_scene final : public scene {
    public:
        virtual void add(const std::shared_ptr<const screen_camera> &pCamera) override;
        virtual void remove(const std::shared_ptr<const screen_camera> &pCamera) override;

        virtual void add(const std::shared_ptr<const texture_camera> &pCamera) override;
        virtual void remove(const std::shared_ptr<const texture_camera> &pCamera) override;

        virtual void add(const std::shared_ptr<const entity> &pEntity) override;
        virtual void remove(const std::shared_ptr<const entity> &pEntity) override;

        virtual void draw(const gdk::graphics::intvector2_type &aFrameBufferSize) const override;

        virtual void set_float(const std::string_view aName, float aValue) override;
        virtual void set_vector2(const std::string_view aName, vector2_type aValue) override;
        virtual void set_vector3(const std::string_view aName, vector3_type aValue) override;
        virtual void set_vector4(const std::string_view aName, vector4_type aValue) override;
        virtual void set_vector4(const std::string_view aName, const color &aValue) override;
        virtual void set_integer(const std::string_view aName, int aValue) override;

        //! what the scene was last told for a name, so a test can see what reached it
        [[nodiscard]] std::optional<float> float_at(const std::string &aName) const;
        [[nodiscard]] std::optional<vector3_type> vector3_at(const std::string &aName) const;
        [[nodiscard]] std::optional<vector4_type> vector4_at(const std::string &aName) const;

        [[nodiscard]] std::size_t entity_count() const;

        /// \brief every entity in the scene, in no particular order
        [[nodiscard]] std::vector<std::shared_ptr<const entity>> entities() const;

        [[nodiscard]] std::size_t camera_count() const;
        [[nodiscard]] bool contains(const std::shared_ptr<const entity> &pEntity) const;

        /// \brief how many times \ref draw has been called
        [[nodiscard]] std::size_t draw_count() const;

        //! the frame buffer size the most recent draw was given
        [[nodiscard]] intvector2_type last_frame_buffer_size() const;

    private:
        std::set<std::shared_ptr<const entity>> mEntities;
        std::set<std::shared_ptr<const screen_camera>> mScreenCameras;
        std::set<std::shared_ptr<const texture_camera>> mTextureCameras;

        std::map<std::string, float> mFloats;
        std::map<std::string, vector2_type> mVector2s;
        std::map<std::string, vector3_type> mVector3s;
        std::map<std::string, vector4_type> mVector4s;
        std::map<std::string, int> mIntegers;

        mutable std::size_t mDrawCount{0};
        mutable intvector2_type mLastFrameBufferSize{0, 0};
    };
}

#endif
