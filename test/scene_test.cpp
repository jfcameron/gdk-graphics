// © Joseph Cameron - All Rights Reserved

#include <gdk/math_constants.h>
#include <gdk/graphics/webgl1es2_gl_state.h>
#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

#include <gdk/graphics/exception.h>
#include <gdk/graphics/types.h>
#include <gdk/graphics/webgl1es2_entity.h>
#include <gdk/graphics/webgl1es2_material.h>
#include <gdk/graphics/webgl1es2_model.h>
#include <gdk/graphics/webgl1es2_scene.h>
#include <gdk/graphics/webgl1es2_screen_camera.h>
#include <gdk/graphics/webgl1es2_shader_program.h>
#include <gdk/graphics/webgl1es2_texture_camera.h>

#include <memory>
#include <string>
#include <vector>

using namespace gdk;

namespace {
    [[nodiscard]] std::shared_ptr<gdk::graphics::gl_state> test_gl_state_ptr() {
        static auto pState = std::make_shared<gdk::graphics::gl_state>();

        return pState;
    }

    [[nodiscard]] gdk::graphics::gl_state &test_gl_state() { return *test_gl_state_ptr(); }
}
using namespace gdk::graphics;

namespace {
    constexpr float SENTINEL = 999.0f;

    const intvector2_type FRAME_BUFFER_SIZE(400, 300);

    [[nodiscard]] std::shared_ptr<webgl1es2_material> a_material(
        const material::render_mode aRenderMode = material::render_mode::opaque) {
        return std::make_shared<webgl1es2_material>(
            webgl1es2_shader_program::make_alpha_cutoff(),
            material::face_culling_mode::none,
            aRenderMode);
    }

    [[nodiscard]] std::shared_ptr<webgl1es2_entity> an_entity(
        const std::shared_ptr<webgl1es2_material> &aMaterial,
        const vector3_type &aPosition = vector3_type::zero) {
        auto pEntity = std::make_shared<webgl1es2_entity>(
            webgl1es2_model::make_cube(), aMaterial);

        pEntity->set_transform(aPosition, quaternion_type::identity);

        return pEntity;
    }

    [[nodiscard]] std::shared_ptr<webgl1es2_screen_camera> a_camera() {
        auto pCamera = std::make_shared<webgl1es2_screen_camera>();
        pCamera->set_perspective_projection(1.0f, 0.1f, 100.0f, 1.0f);
        pCamera->set_transform(vector3_type::zero, quaternion_type::identity);

        return pCamera;
    }

    [[nodiscard]] GLuint current_program() {
        GLint handle = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &handle);

        return static_cast<GLuint>(handle);
    }

    [[nodiscard]] std::vector<GLfloat> read_mvp() {
        std::vector<GLfloat> out(16, -1.0f);
        glGetUniformfv(current_program(), glGetUniformLocation(current_program(), "_MVP"),
            &out.front());

        return out;
    }

    void mark(webgl1es2_material &aMaterial) {
        aMaterial.activate(test_gl_state());

        auto marker = matrix4x4_type::identity;
        marker.set(3, 0, SENTINEL);

        aMaterial.getShaderProgram()->try_set_uniform("_MVP", marker);
    }

    [[nodiscard]] bool nothing_was_drawn() {
        return read_mvp().at(12) == SENTINEL;
    }

    [[nodiscard]] std::vector<GLfloat> expected_mvp(const webgl1es2_screen_camera &aCamera,
        const webgl1es2_entity &aEntity) {
        const auto product = aCamera.get_projection_matrix() * aCamera.get_view_matrix()
            * aEntity.getModelMatrix();

        std::vector<GLfloat> out;
        for (std::size_t column = 0; column < matrix4x4_type::order; ++column)
            for (std::size_t row = 0; row < matrix4x4_type::order; ++row)
                out.push_back(product.get(column, row));

        return out;
    }

    void require_matches(const std::vector<GLfloat> &aUploaded, const std::vector<GLfloat> &aExpected) {
        REQUIRE(aUploaded.size() == aExpected.size());

        for (std::size_t k = 0; k < aExpected.size(); ++k)
            REQUIRE(aUploaded.at(k) == Approx(aExpected.at(k)).margin(1e-4f));
    }
}

TEST_CASE("gdk::webgl1es2_scene camera membership", "[gdk::webgl1es2_scene]")
{
    initGL();

    webgl1es2_scene scene(test_gl_state_ptr());

    const auto pMaterial = a_material();
    const auto pEntity = an_entity(pMaterial, {0, 0, -10});
    const auto pCamera = a_camera();

    scene.add(pEntity);

    SECTION("a scene with no camera draws nothing")
    {
        mark(*pMaterial);

        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("an added camera draws the scene's entities through itself")
    {
        scene.add(pCamera);

        mark(*pMaterial);

        scene.draw(FRAME_BUFFER_SIZE);

        require_matches(read_mvp(), expected_mvp(*pCamera, *pEntity));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a removed camera stops drawing")
    {
        scene.add(pCamera);
        scene.remove(pCamera);

        mark(*pMaterial);

        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("removing a camera that was never added is harmless")
    {
        scene.add(pCamera);
        scene.remove(a_camera());   

        mark(*pMaterial);

        scene.draw(FRAME_BUFFER_SIZE);

        require_matches(read_mvp(), expected_mvp(*pCamera, *pEntity));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("adding the same camera twice draws through it once")
    {
        scene.add(pCamera);
        scene.add(pCamera);

        mark(*pMaterial);

        scene.draw(FRAME_BUFFER_SIZE);

        require_matches(read_mvp(), expected_mvp(*pCamera, *pEntity));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a texture camera is accepted on its own overload and draws")
    {
        auto pTextureCamera = std::make_shared<webgl1es2_texture_camera>();
        pTextureCamera->set_perspective_projection(1.0f, 0.1f, 100.0f, 1.0f);
        pTextureCamera->set_transform(vector3_type::zero,
            quaternion_type::identity);

        scene.add(pTextureCamera);

        mark(*pMaterial);

        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE_FALSE(nothing_was_drawn());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a null camera is ignored rather than stored")
    {
        REQUIRE_NOTHROW(scene.add(std::shared_ptr<const screen_camera>()));
        REQUIRE_NOTHROW(scene.remove(std::shared_ptr<const screen_camera>()));
        REQUIRE_NOTHROW(scene.add(std::shared_ptr<const texture_camera>()));
        REQUIRE_NOTHROW(scene.remove(std::shared_ptr<const texture_camera>()));

        mark(*pMaterial);

        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());
        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("gdk::webgl1es2_scene entity membership", "[gdk::webgl1es2_scene]")
{
    initGL();

    webgl1es2_scene scene(test_gl_state_ptr());

    const auto pMaterial = a_material();
    const auto pCamera = a_camera();

    scene.add(pCamera);

    SECTION("a scene with no entities draws nothing")
    {
        mark(*pMaterial);

        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("removing an entity takes it out")
    {
        const auto pEntity = an_entity(pMaterial, {0, 0, -10});
        scene.add(pEntity);

        scene.remove(pEntity);

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());
    }

    SECTION("and letting go of an entity takes it out on its own")
    {
        {
            const auto pEntity = an_entity(pMaterial, {0, 0, -10});
            scene.add(pEntity);

            mark(*pMaterial);
            scene.draw(FRAME_BUFFER_SIZE);

            REQUIRE_FALSE(nothing_was_drawn());
        }

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());
    }

    SECTION("one entity can be in two scenes, and leaves both when it is dropped")
    {
        webgl1es2_scene other(test_gl_state_ptr());

        const auto pOtherCamera = a_camera();

        other.add(pOtherCamera);

        {
            const auto pEntity = an_entity(pMaterial, {0, 0, -10});

            scene.add(pEntity);
            other.add(pEntity);

            mark(*pMaterial);
            scene.draw(FRAME_BUFFER_SIZE);
            REQUIRE_FALSE(nothing_was_drawn());

            mark(*pMaterial);
            other.draw(FRAME_BUFFER_SIZE);
            REQUIRE_FALSE(nothing_was_drawn());
        }

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);
        REQUIRE(nothing_was_drawn());

        mark(*pMaterial);
        other.draw(FRAME_BUFFER_SIZE);
        REQUIRE(nothing_was_drawn());
    }

    SECTION("a hidden entity in the scene draws nothing")
    {
        auto pEntity = an_entity(pMaterial, {0, 0, -10});
        scene.add(pEntity);

        pEntity->hide();

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());

        pEntity->show();

        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE_FALSE(nothing_was_drawn());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("a null entity is ignored rather than stored")
    {
        REQUIRE_NOTHROW(scene.add(std::shared_ptr<const entity>()));

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());
        REQUIRE(!jfc::glGetError());
    }

    SECTION("opaque and transparent entities are both drawn")
    {
        const auto pOpaqueMaterial = a_material(material::render_mode::opaque);
        const auto pTransparentMaterial = a_material(material::render_mode::transparent);

        const auto pOpaque = an_entity(pOpaqueMaterial, {-2, 0, -10});
        const auto pTransparent = an_entity(pTransparentMaterial, {2, 0, -10});

        SECTION("the opaque one alone")
        {
            scene.add(pOpaque);

            mark(*pOpaqueMaterial);
            scene.draw(FRAME_BUFFER_SIZE);

            require_matches(read_mvp(), expected_mvp(*pCamera, *pOpaque));
        }

        SECTION("the transparent one alone")
        {
            scene.add(pTransparent);

            mark(*pTransparentMaterial);
            scene.draw(FRAME_BUFFER_SIZE);

            require_matches(read_mvp(), expected_mvp(*pCamera, *pTransparent));
        }

        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("gdk::webgl1es2_scene draw order", "[gdk::webgl1es2_scene]")
{
    initGL();

    webgl1es2_scene scene(test_gl_state_ptr());

    const auto pCamera = a_camera();   
    scene.add(pCamera);

    const auto pMaterial = a_material(material::render_mode::transparent);

    SECTION("transparent entities are drawn back to front")
    {
        const auto pNear = an_entity(pMaterial, {0, 0, -5});
        const auto pFar = an_entity(pMaterial, {0, 0, -30});

        scene.add(pFar);
        scene.add(pNear);

        scene.draw(FRAME_BUFFER_SIZE);

        require_matches(read_mvp(), expected_mvp(*pCamera, *pNear));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("opaque entities are drawn before transparent ones")
    {
        const auto pOpaqueMaterial = a_material(material::render_mode::opaque);

        const auto pOpaque = an_entity(pOpaqueMaterial, {0, 0, -5});
        const auto pTransparent = an_entity(pMaterial, {0, 0, -30});

        scene.add(pOpaque);
        scene.add(pTransparent);

        scene.draw(FRAME_BUFFER_SIZE);

        require_matches(read_mvp(), expected_mvp(*pCamera, *pTransparent));
        REQUIRE(!jfc::glGetError());
    }

    SECTION("the order follows the camera, not the order they were added")
    {
        const auto pA = an_entity(pMaterial, {0, 0, -5});
        const auto pB = an_entity(pMaterial, {0, 0, -30});

        scene.add(pA);
        scene.add(pB);

        scene.draw(FRAME_BUFFER_SIZE);
        require_matches(read_mvp(), expected_mvp(*pCamera, *pA));

        pCamera->set_transform({0, 0, -50},
            quaternion_type::from_euler(vector3_type{0, gdk::numbers::pi_v<float>, 0}));

        scene.draw(FRAME_BUFFER_SIZE);
        require_matches(read_mvp(), expected_mvp(*pCamera, *pB));

        REQUIRE(!jfc::glGetError());
    }
}

TEST_CASE("entities outside the view volume are not drawn", "[gdk::webgl1es2_scene][culling]")
{
    initGL();

    webgl1es2_scene scene(test_gl_state_ptr());

    const auto pMaterial = a_material();
    const auto pCamera = a_camera();

    scene.add(pCamera);

    SECTION("something in front of the camera still draws")
    {
        const auto pEntity = an_entity(pMaterial, {0, 0, -10});
        scene.add(pEntity);

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE_FALSE(nothing_was_drawn());
    }

    SECTION("something behind the camera does not")
    {
        const auto pEntity = an_entity(pMaterial, {0, 0, 50});
        scene.add(pEntity);

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());
    }

    SECTION("nor does something far off to the side")
    {
        const auto pEntity = an_entity(pMaterial, {5000, 0, -10});
        scene.add(pEntity);

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE(nothing_was_drawn());
    }

    SECTION("and moving it back into view draws it again")
    {
        const auto pEntity = an_entity(pMaterial, {0, 0, 50});
        scene.add(pEntity);

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);
        REQUIRE(nothing_was_drawn());

        pEntity->set_transform({0, 0, -10}, quaternion_type::identity);

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);
        REQUIRE_FALSE(nothing_was_drawn());
    }

    SECTION("an entity straddling the edge is kept, not dropped")
    {
        const auto pEntity = an_entity(pMaterial, {0, 0, 0});
        scene.add(pEntity);

        mark(*pMaterial);
        scene.draw(FRAME_BUFFER_SIZE);

        REQUIRE_FALSE(nothing_was_drawn());
    }
}
