// © Joseph Cameron - All Rights Reserved

#include <gdk/graphics/null_context.h>

#include <gdk/graphics/model_data.h>
#include <gdk/graphics/texture_data.h>

#include <algorithm>

using namespace gdk;
using namespace gdk::graphics;

null_shader::null_shader(std::string aVertexSource, std::string aFragmentSource)
: mVertexSource(std::move(aVertexSource))
, mFragmentSource(std::move(aFragmentSource))
{}

const std::string &null_shader::vertex_source() const { return mVertexSource; }

const std::string &null_shader::fragment_source() const { return mFragmentSource; }

null_texture::null_texture(const texture_data::view &aView, const wrap_mode aU, const wrap_mode aV)
: mWidth(aView.width)
, mHeight(aView.height)
, mUploadCount(1)
, mWrapU(aU)
, mWrapV(aV)
{}

void null_texture::update_data(const texture_data::view &aView) {
    mWidth = aView.width;
    mHeight = aView.height;

    ++mUploadCount;
}

void null_texture::update_data(const texture_data::view &, const size_t, const size_t) {
    ++mUploadCount;
}

std::size_t null_texture::width() const { return mWidth; }

std::size_t null_texture::height() const { return mHeight; }

std::size_t null_texture::upload_count() const { return mUploadCount; }

null_model::null_model(const usage_hint aUsage, const model_data &aModelData) {
    upload(aUsage, aModelData);
}

void null_model::upload(const usage_hint &, const model_data &aModelData) {
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

std::optional<float> null_material::float_at(const std::string &aName) const {
    const auto found = mFloats.find(aName);

    return found == mFloats.end() ? std::optional<float>{} : found->second;
}

std::optional<vector4_type> null_material::vector4_at(const std::string &aName) const {
    const auto found = mVector4s.find(aName);

    return found == mVector4s.end() ? std::optional<vector4_type>{} : found->second;
}

texture_ptr_type null_material::texture_at(const std::string &aName) const {
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

const matrix4x4_type &null_entity::transform() const { return mTransform; }

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

null_texture_camera::null_texture_camera()
: mpColor(std::make_shared<null_texture>())
, mpDepth(std::make_shared<null_texture>())
{}

void null_texture_camera::set_perspective_projection(const floating_point_type,
    const floating_point_type, const floating_point_type, const floating_point_type) {}

void null_texture_camera::set_orthographic_projection(const vector2_type &,
    const floating_point_type, const floating_point_type, const floating_point_type) {}

void null_texture_camera::set_transform(const matrix4x4_type &) {}

void null_texture_camera::set_transform(const vector3_type &, const quaternion_type &) {}

void null_texture_camera::set_clear_color(const color &) {}

void null_texture_camera::set_clear_mode(const clear_mode) {}

const std::shared_ptr<texture> null_texture_camera::get_color_texture(const size_t) const {
    return mpColor;
}

const std::shared_ptr<texture> null_texture_camera::get_depth_texture() const { return mpDepth; }

void null_scene::add(const std::shared_ptr<const screen_camera> &pCamera) {
    mScreenCameras.insert(pCamera);
}

void null_scene::remove(const std::shared_ptr<const screen_camera> &pCamera) {
    mScreenCameras.erase(pCamera);
}

void null_scene::add(const std::shared_ptr<const texture_camera> &pCamera) {
    mTextureCameras.insert(pCamera);
}

void null_scene::remove(const std::shared_ptr<const texture_camera> &pCamera) {
    mTextureCameras.erase(pCamera);
}

void null_scene::add(const std::shared_ptr<const entity> &pEntity) { mEntities.insert(pEntity); }

void null_scene::remove(const std::shared_ptr<const entity> &pEntity) { mEntities.erase(pEntity); }

void null_scene::draw(const intvector2_type &aFrameBufferSize) const {
    mLastFrameBufferSize = aFrameBufferSize;

    ++mDrawCount;
}

std::size_t null_scene::entity_count() const { return mEntities.size(); }

std::size_t null_scene::camera_count() const {
    return mScreenCameras.size() + mTextureCameras.size();
}

bool null_scene::contains(const std::shared_ptr<const entity> &pEntity) const {
    return mEntities.count(pEntity) > 0;
}

std::size_t null_scene::draw_count() const { return mDrawCount; }

intvector2_type null_scene::last_frame_buffer_size() const { return mLastFrameBufferSize; }

std::shared_ptr<null_context> null_context::make() {
    return std::shared_ptr<null_context>(new null_context());
}

scene_ptr_type null_context::make_scene() {
    ++mResourcesMade;

    return std::make_shared<null_scene>();
}

camera_ptr_type null_context::make_camera() {
    ++mResourcesMade;

    return std::make_shared<null_camera>();
}

texture_camera_ptr_type null_context::make_texture_camera() {
    ++mResourcesMade;

    return std::make_shared<null_texture_camera>();
}

entity_ptr_type null_context::make_entity(const const_model_ptr_type pModel,
    const const_material_ptr_type pMaterial) {
    ++mResourcesMade;

    return std::make_shared<null_entity>(pModel, pMaterial);
}

model_ptr_type null_context::make_model() {
    ++mResourcesMade;

    return std::make_shared<null_model>();
}

model_ptr_type null_context::make_model(const gdk::graphics::model::usage_hint aUsage,
    const model_data &aModelData) {
    ++mResourcesMade;

    return std::make_shared<null_model>(aUsage, aModelData);
}

material_ptr_type null_context::make_material(const const_shader_ptr_type pShader,
    const material::render_mode aRenderMode, const material::face_culling_mode aFaceCullingMode) {
    ++mResourcesMade;

    return std::make_shared<null_material>(pShader, aRenderMode, aFaceCullingMode);
}

texture_ptr_type null_context::make_texture(const texture_data::view &aView,
    const texture::wrap_mode aWrapModeU, const texture::wrap_mode aWrapModeV,
    const texture::filter_mode) {
    ++mResourcesMade;

    return std::make_shared<null_texture>(aView, aWrapModeU, aWrapModeV);
}

texture_ptr_type null_context::make_texture() {
    ++mResourcesMade;

    return std::make_shared<null_texture>();
}

model_ptr_type null_context::make_cube_model() const {
    return std::make_shared<null_model>();
}

model_ptr_type null_context::make_sphere_model() const {
    return std::make_shared<null_model>();
}

shader_ptr_type null_context::make_alpha_cutoff_shader() const {
    return std::make_shared<null_shader>();
}

shader_ptr_type null_context::make_shader(const std::string_view aVertexSource,
    const std::string_view aFragmentSource) {
    ++mResourcesMade;

    return std::make_shared<null_shader>(std::string(aVertexSource), std::string(aFragmentSource));
}

std::size_t null_context::resources_made() const { return mResourcesMade; }
