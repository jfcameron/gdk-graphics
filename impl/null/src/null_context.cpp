// © Joseph Cameron - MIT License

#include <gdk/graphics/null_context.h>
#include <gdk/graphics/texture_patch.h>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>

#include <algorithm>
#include <string_view>

using namespace gdk;
using namespace gdk::graphics;

null_shader::null_shader(std::string aVertexSource, std::string aFragmentSource)
: mVertexSource(std::move(aVertexSource))
, mFragmentSource(std::move(aFragmentSource))
{}

const std::string &null_shader::vertex_source() const { return mVertexSource; }

const std::string &null_shader::fragment_source() const { return mFragmentSource; }

namespace {
    [[nodiscard]] std::size_t channels_of(const texture::format aFormat) {
        switch (aFormat) {
            case texture::format::grey: return 1;
            case texture::format::rg: return 2;
            case texture::format::rgb: return 3;
            case texture::format::rgba: return 4;
        }

        return 0;
    }

    [[nodiscard]] texture_data::channel_data copy_of(const texture_data::view &aView) {
        if (!aView.data) return {};

        return texture_data::channel_data(aView.data,
            aView.data + aView.width * aView.height * channels_of(aView.format));
    }
}

null_texture::null_texture(const texture_data::view &aView, const wrap_mode aU, const wrap_mode aV)
: mWidth(aView.width)
, mHeight(aView.height)
, mUploadCount(1)
, mChannels(channels_of(aView.format))
, mFormat(aView.format)
, mData(copy_of(aView))
, mWrapU(aU)
, mWrapV(aV)
{}

null_texture::null_texture(const std::size_t aWidth, const std::size_t aHeight)
: mWidth(aWidth)
, mHeight(aHeight)
, mChannels(channels_of(format::rgba))
, mFormat(format::rgba)
, mData(aWidth * aHeight * mChannels)
, mWrapU(wrap_mode::clamped)
, mWrapV(wrap_mode::clamped)
{}

void null_texture::hold_to(std::shared_ptr<const profile> pProfile) { mProfile = std::move(pProfile); }

void null_texture::update_data(const texture_data::view &aView) {
    if (mProfile) check_texture(*mProfile, aView.width, aView.height,
        mWrapU == wrap_mode::clamped && mWrapV == wrap_mode::clamped);

    mWidth = aView.width;
    mHeight = aView.height;
    mChannels = channels_of(aView.format);
    mFormat = aView.format;
    mData = copy_of(aView);

    ++mUploadCount;
}

void null_texture::update_data(const texture_data::view &aView, const std::size_t aOffsetX,
    const std::size_t aOffsetY) {
    check_patch(aView, aOffsetX, aOffsetY, mWidth, mHeight, mFormat);

    if (aView.data && mData.size() == mWidth * mHeight * mChannels) {
        for (std::size_t row = 0; row < aView.height; ++row)
            for (std::size_t column = 0; column < aView.width; ++column)
                for (std::size_t channel = 0; channel < mChannels; ++channel)
                    mData[((aOffsetY + row) * mWidth + aOffsetX + column) * mChannels + channel]
                        = aView.data[(row * aView.width + column) * mChannels + channel];
    }

    ++mUploadCount;
}

std::size_t null_texture::width() const { return mWidth; }

std::size_t null_texture::height() const { return mHeight; }

std::size_t null_texture::upload_count() const { return mUploadCount; }

const texture_data::channel_data &null_texture::data() const { return mData; }

null_model::null_model(const usage_hint aUsage, const model_data &aModelData) {
    upload(aUsage, aModelData);
}

void null_model::hold_to(std::shared_ptr<const profile> pProfile) { mProfile = std::move(pProfile); }

void null_model::upload(const usage_hint &, const model_data &aModelData) {
    if (mProfile) check_model(*mProfile, aModelData);

    mVertexCount = aModelData.vertex_count();
    mIndexCount = aModelData.indexes().size();
    mLastUpload = aModelData;

    ++mUploadCount;
}

std::size_t null_model::vertex_count() const { return mVertexCount; }

std::size_t null_model::index_count() const { return mIndexCount; }

std::size_t null_model::upload_count() const { return mUploadCount; }

const model_data &null_model::last_upload() const { return mLastUpload; }

null_material::null_material(const_shader_ptr_type pShader, const render_mode aRenderMode,
    const face_culling_mode aFaceCullingMode)
: mpShader(std::move(pShader))
, mRenderMode(aRenderMode)
, mFaceCullingMode(aFaceCullingMode)
{}

void null_material::set_texture(const std::string_view aName, texture_ptr_type aValue) {
    const std::string name(aName);

    if (!aValue) {
        mTextures.erase(name);
        mNames.erase(name);

        return;
    }

    mTextures[name] = std::move(aValue);
    mNames.insert(name);
}

void null_material::set_float(const std::string_view aName, float aValue) {
    const std::string name(aName);

    mFloats[name] = aValue;
    mNames.insert(name);
}

void null_material::set_vector2(const std::string_view aName, vector2_type aValue) {
    set_vector4(aName, vector4_type(aValue.x, aValue.y, 0, 0));
}

void null_material::set_vector3(const std::string_view aName, vector3_type aValue) {
    set_vector4(aName, vector4_type(aValue.x, aValue.y, aValue.z, 0));
}

void null_material::set_vector4(const std::string_view aName, vector4_type aValue) {
    const std::string name(aName);

    mVector4s[name] = aValue;
    mNames.insert(name);
}

void null_material::set_vector4(const std::string_view aName, const color &aValue) {
    set_vector4(aName, vector4_type(aValue.r, aValue.g, aValue.b, aValue.a));
}

void null_material::set_integer(const std::string_view aName, int aValue) {
    set_float(aName, static_cast<float>(aValue));
}

void null_material::set_integer2(const std::string_view aName, int a1, int a2) {
    set_vector4(aName, vector4_type(static_cast<float>(a1), static_cast<float>(a2), 0, 0));
}

void null_material::set_integer3(const std::string_view aName, int a1, int a2, int a3) {
    set_vector4(aName, vector4_type(static_cast<float>(a1), static_cast<float>(a2),
        static_cast<float>(a3), 0));
}

void null_material::set_integer4(const std::string_view aName, int a1, int a2, int a3, int a4) {
    set_vector4(aName, vector4_type(static_cast<float>(a1), static_cast<float>(a2),
        static_cast<float>(a3), static_cast<float>(a4)));
}

void null_material::set_int_vector2_array(const std::string_view aName,
    const std::vector<intvector2_type> &) {
    mNames.insert(std::string(aName));
}

std::optional<float> null_material::float_at(std::string_view aName) const {
    const auto found = mFloats.find(aName);

    return found == mFloats.end() ? std::optional<float>{} : found->second;
}

std::optional<vector4_type> null_material::vector4_at(std::string_view aName) const {
    const auto found = mVector4s.find(aName);

    return found == mVector4s.end() ? std::optional<vector4_type>{} : found->second;
}

texture_ptr_type null_material::texture_at(std::string_view aName) const {
    const auto found = mTextures.find(aName);

    return found == mTextures.end() ? nullptr : found->second;
}

std::vector<std::string> null_material::uniforms() const {
    return {mNames.begin(), mNames.end()};
}

material::render_mode null_material::mode() const { return mRenderMode; }

null_entity::null_entity(const_model_ptr_type pModel, const_material_ptr_type pMaterial)
: mpModel(std::move(pModel))
, mpMaterial(std::move(pMaterial))
{}

void null_entity::hide() { mHidden = true; }

void null_entity::show() { mHidden = false; }

bool null_entity::is_hidden() const { return mHidden; }

void null_entity::set_transform(const vector3_type &aWorldPos, const quaternion_type &aRotation,
    const vector3_type &aScale) {
    matrix4x4_type translation;
    translation.set_translation(aWorldPos);

    matrix4x4_type rotation;
    rotation.set_rotation(aRotation);

    matrix4x4_type scale;
    scale.set_scale(aScale);

    mTransform = translation * rotation * scale;
}

void null_entity::set_transform(const matrix4x4_type &aTransform) { mTransform = aTransform; }

void null_entity::set_bounds(const vector3_type &aCentre, const floating_point_type aRadius) {
    mBounds = std::pair{aCentre, aRadius};
}

const matrix4x4_type &null_entity::transform() const { return mTransform; }

const std::optional<std::pair<vector3_type, floating_point_type>> &null_entity::bounds() const { return mBounds; }

const_model_ptr_type null_entity::model() const { return mpModel; }

const_material_ptr_type null_entity::material() const { return mpMaterial; }

void null_camera::set_perspective_projection(const floating_point_type aFieldOfView,
    const floating_point_type aNear, const floating_point_type aFar,
    const floating_point_type aAspectRatio) {
    mFieldOfView = aFieldOfView;
    mNear = aNear;
    mFar = aFar;
    mAspectRatio = aAspectRatio;
}

void null_camera::set_perspective_projection(const floating_point_type aFieldOfView,
    const floating_point_type aNear, const floating_point_type aFar,
    const floating_point_type aAspectRatio, const vector2_type &) {
    set_perspective_projection(aFieldOfView, aNear, aFar, aAspectRatio);
}

void null_camera::set_orthographic_projection(const vector2_type &,
    const floating_point_type aNear, const floating_point_type aFar,
    const floating_point_type aAspectRatio) {
    mFieldOfView = 0;
    mNear = aNear;
    mFar = aFar;
    mAspectRatio = aAspectRatio;
}

void null_camera::set_transform(const matrix4x4_type &aTransform) { mTransform = aTransform; }

void null_camera::set_transform(const vector3_type &aWorldPos, const quaternion_type &aRotation) {
    mPosition = aWorldPos;
    mRotation = aRotation;

    matrix4x4_type translation;
    translation.set_translation(aWorldPos);

    matrix4x4_type rotation;
    rotation.set_rotation(aRotation);

    mTransform = translation * rotation;
}

void null_camera::set_clear_color(const color &aColor) { mClearColor = aColor; }

void null_camera::set_clear_mode(const clear_mode aClearMode) { mClearMode = aClearMode; }

void null_camera::set_viewport(const float aX, const float aY, const float aWidth,
    const float aHeight) {
    mViewport = {aX, aY, aWidth, aHeight};
}

void null_camera::set_scissor(const float aX, const float aY, const float aWidth,
    const float aHeight) {
    mScissor = std::array<float, 4>{aX, aY, aWidth, aHeight};
}

void null_camera::clear_scissor() { mScissor.reset(); }

std::array<float, 4> null_camera::scissor() const {
    return mScissor ? *mScissor : mViewport;
}

const vector3_type &null_camera::position() const { return mPosition; }

const quaternion_type &null_camera::rotation() const { return mRotation; }

const matrix4x4_type &null_camera::transform() const { return mTransform; }

const color &null_camera::clear_color() const { return mClearColor; }

camera::clear_mode null_camera::mode() const { return mClearMode; }

floating_point_type null_camera::field_of_view() const { return mFieldOfView; }

floating_point_type null_camera::aspect_ratio() const { return mAspectRatio; }

floating_point_type null_camera::near_clip() const { return mNear; }

floating_point_type null_camera::far_clip() const { return mFar; }

null_texture_camera::null_texture_camera(const intvector2_type &aSize)
: mpColor(std::make_shared<null_texture>(static_cast<std::size_t>(aSize.x), static_cast<std::size_t>(aSize.y)))
, mpDepth(std::make_shared<null_texture>(static_cast<std::size_t>(aSize.x), static_cast<std::size_t>(aSize.y)))
{}

void null_texture_camera::set_perspective_projection(const floating_point_type,
    const floating_point_type, const floating_point_type, const floating_point_type) {}

void null_texture_camera::set_perspective_projection(const floating_point_type,
    const floating_point_type, const floating_point_type, const floating_point_type, const vector2_type &) {}

void null_texture_camera::set_orthographic_projection(const vector2_type &,
    const floating_point_type, const floating_point_type, const floating_point_type) {}

void null_texture_camera::set_transform(const matrix4x4_type &) {}

void null_texture_camera::set_transform(const vector3_type &, const quaternion_type &) {}

void null_texture_camera::set_clear_color(const color &) {}

void null_texture_camera::set_clear_mode(const clear_mode) {}

texture_ptr_type null_texture_camera::color_texture() const { return mpColor; }

texture_ptr_type null_texture_camera::depth_texture() const { return mpDepth; }

namespace {
    template<typename held_type>
    void hold(std::vector<std::weak_ptr<const held_type>> &aHeld, const std::shared_ptr<const held_type> &pAdded) {
        if (!pAdded) return;

        std::erase_if(aHeld, [](const auto &a) { return a.expired(); });

        if (std::none_of(aHeld.begin(), aHeld.end(), [&](const auto &a) { return a.lock() == pAdded; }))
            aHeld.push_back(pAdded);
    }

    template<typename held_type>
    void let_go(std::vector<std::weak_ptr<const held_type>> &aHeld, const std::shared_ptr<const held_type> &pRemoved) {
        std::erase_if(aHeld, [&](const auto &a) {
            const auto pLocked = a.lock();

            return !pLocked || pLocked == pRemoved;
        });
    }

    template<typename held_type>
    [[nodiscard]] std::vector<std::shared_ptr<const held_type>> alive(std::vector<std::weak_ptr<const held_type>> &aHeld) {
        std::erase_if(aHeld, [](const auto &a) { return a.expired(); });

        std::vector<std::shared_ptr<const held_type>> out;

        for (const auto &each : aHeld) if (auto pLocked = each.lock()) out.push_back(std::move(pLocked));

        return out;
    }
}

void null_scene::add(const std::shared_ptr<const screen_camera> &pCamera) { hold(mScreenCameras, pCamera); }

void null_scene::remove(const std::shared_ptr<const screen_camera> &pCamera) { let_go(mScreenCameras, pCamera); }

void null_scene::add(const std::shared_ptr<const texture_camera> &pCamera) { hold(mTextureCameras, pCamera); }

void null_scene::remove(const std::shared_ptr<const texture_camera> &pCamera) { let_go(mTextureCameras, pCamera); }

void null_scene::add(const std::shared_ptr<const entity> &pEntity) { hold(mEntities, pEntity); }

void null_scene::remove(const std::shared_ptr<const entity> &pEntity) { let_go(mEntities, pEntity); }

void null_scene::draw(const intvector2_type &aFrameBufferSize) const {
    mLastFrameBufferSize = aFrameBufferSize;

    ++mDrawCount;
}

void null_scene::set_float(const std::string_view aName, float aValue) {
    mFloats[std::string(aName)] = aValue;
}

void null_scene::set_vector2(const std::string_view aName, vector2_type aValue) {
    mVector2s[std::string(aName)] = aValue;
}

void null_scene::set_vector3(const std::string_view aName, vector3_type aValue) {
    mVector3s[std::string(aName)] = aValue;
}

void null_scene::set_vector4(const std::string_view aName, vector4_type aValue) {
    mVector4s[std::string(aName)] = aValue;
}

void null_scene::set_vector4(const std::string_view aName, const color &aValue) {
    set_vector4(aName, vector4_type(aValue.r, aValue.g, aValue.b, aValue.a));
}

void null_scene::set_integer(const std::string_view aName, int aValue) {
    mIntegers[std::string(aName)] = aValue;
}

std::optional<float> null_scene::float_at(std::string_view aName) const {
    const auto found = mFloats.find(aName);

    return found == mFloats.end() ? std::optional<float>() : found->second;
}

std::optional<vector3_type> null_scene::vector3_at(std::string_view aName) const {
    const auto found = mVector3s.find(aName);

    return found == mVector3s.end() ? std::optional<vector3_type>() : found->second;
}

std::optional<vector4_type> null_scene::vector4_at(std::string_view aName) const {
    const auto found = mVector4s.find(aName);

    return found == mVector4s.end() ? std::optional<vector4_type>() : found->second;
}

std::size_t null_scene::entity_count() const { return alive(mEntities).size(); }

std::vector<std::shared_ptr<const entity>> null_scene::entities() const { return alive(mEntities); }

std::size_t null_scene::camera_count() const {
    return alive(mScreenCameras).size() + alive(mTextureCameras).size();
}

bool null_scene::contains(const std::shared_ptr<const entity> &pEntity) const {
    const auto held = alive(mEntities);

    return std::find(held.begin(), held.end(), pEntity) != held.end();
}

std::size_t null_scene::draw_count() const { return mDrawCount; }

intvector2_type null_scene::last_frame_buffer_size() const { return mLastFrameBufferSize; }

std::shared_ptr<null_context> null_context::make(std::optional<graphics::profile> aProfile,
    shader_check_type aShaderCheck) {
    const auto pContext = std::shared_ptr<null_context>(new null_context());

    if (aProfile) pContext->mProfile = std::make_shared<const graphics::profile>(std::move(*aProfile));

    pContext->mShaderCheck = std::move(aShaderCheck);

    return pContext;
}

scene_ptr_type null_context::make_scene() {
    ++mResourcesMade;

    return std::make_shared<null_scene>();
}

screen_camera_ptr_type null_context::make_camera() {
    ++mResourcesMade;

    return std::make_shared<null_camera>();
}

texture_camera_ptr_type null_context::make_texture_camera() {
    return make_texture_camera({1024, 1024});
}

texture_camera_ptr_type null_context::make_texture_camera(const intvector2_type &aSize) {
    if (aSize.x <= 0 || aSize.y <= 0) throw exception("null_context::make_texture_camera: a size must be positive");

    if (mProfile) check_texture(*mProfile, static_cast<std::size_t>(aSize.x), static_cast<std::size_t>(aSize.y), true);

    ++mResourcesMade;

    return std::make_shared<null_texture_camera>(aSize);
}

entity_ptr_type null_context::make_entity(const const_model_ptr_type pModel,
    const const_material_ptr_type pMaterial) {
    ++mResourcesMade;

    return std::make_shared<null_entity>(pModel, pMaterial);
}

model_ptr_type null_context::make_model() {
    ++mResourcesMade;

    const auto pModel = std::make_shared<null_model>();
    pModel->hold_to(mProfile);

    return pModel;
}

model_ptr_type null_context::make_model(const gdk::graphics::model::usage_hint aUsage,
    const model_data &aModelData) {
    if (mProfile) check_model(*mProfile, aModelData);

    ++mResourcesMade;

    const auto pModel = std::make_shared<null_model>(aUsage, aModelData);
    pModel->hold_to(mProfile);

    return pModel;
}

material_ptr_type null_context::make_material(const const_shader_ptr_type pShader,
    const material::render_mode aRenderMode, const material::face_culling_mode aFaceCullingMode) {
    ++mResourcesMade;

    return std::make_shared<null_material>(pShader, aRenderMode, aFaceCullingMode);
}

material_ptr_type null_context::make_material(const const_material_ptr_type &aPrototype) {
    const auto pPrototype = std::dynamic_pointer_cast<const null_material>(aPrototype);

    if (!pPrototype) throw exception("null_context::make_material: the prototype is not a null_material");

    ++mResourcesMade;

    return std::make_shared<null_material>(*pPrototype);
}

texture_ptr_type null_context::make_texture(const texture_data::view &aView,
    const texture::wrap_mode aWrapModeU, const texture::wrap_mode aWrapModeV,
    const texture::filter_mode) {
    if (mProfile) check_texture(*mProfile, aView.width, aView.height,
        aWrapModeU == texture::wrap_mode::clamped && aWrapModeV == texture::wrap_mode::clamped);

    ++mResourcesMade;
    ++mTextureCount;

    const auto pTexture = std::make_shared<null_texture>(aView, aWrapModeU, aWrapModeV);
    pTexture->hold_to(mProfile);

    return pTexture;
}

std::size_t null_context::texture_count() const { return mTextureCount; }

texture_ptr_type null_context::make_texture() {
    ++mResourcesMade;
    ++mTextureCount;

    const auto pTexture = std::make_shared<null_texture>();
    pTexture->hold_to(mProfile);

    return pTexture;
}

model_ptr_type null_context::make_cube_model() const {
    ++mResourcesMade;

    const auto pModel = std::make_shared<null_model>();
    pModel->hold_to(mProfile);

    return pModel;
}

model_ptr_type null_context::make_sphere_model() const { return make_cube_model(); }

graphics::limits null_context::limits() const {
    graphics::limits own;
    own.texture_size = 8192;

    return mProfile ? narrowest(own, mProfile->most) : own;
}

shader_ptr_type null_context::make_alpha_cutoff_shader() const {
    if (!mpAlphaCutoff) mpAlphaCutoff = std::make_shared<null_shader>();

    return mpAlphaCutoff;
}

shader_ptr_type null_context::make_alpha_blend_shader() const {
    if (!mpAlphaBlend) mpAlphaBlend = std::make_shared<null_shader>();

    return mpAlphaBlend;
}

shader_ptr_type null_context::make_shader(const std::string_view aVertexSource,
    const std::string_view aFragmentSource) {
    if (mProfile && mShaderCheck) mShaderCheck(*mProfile, aVertexSource, aFragmentSource);

    ++mResourcesMade;

    return std::make_shared<null_shader>(std::string(aVertexSource), std::string(aFragmentSource));
}

std::size_t null_context::resources_made() const { return mResourcesMade; }
