// © Joseph Cameron - All Rights Reserved

#include <vector>

#include <jfc/catch.hpp>

#include <gdk/graphics/ext/sprite_animation.h>

using namespace gdk::graphics;

namespace {
    [[nodiscard]] std::vector<sprite_animation::frame> demo_frames() {
        return {
            { 0, 0, 16, 17, 0.25f},
            {16, 0, 16, 17, 0.25f},
            {32, 0, 16, 17, 0.25f},
            {48, 0, 16, 17, 0.25f},
        };
    }

    constexpr int SHEET_W = 64;
    constexpr int SHEET_H = 64;

    [[nodiscard]] int index_of(const sprite_animation::normalized_frame &aFrame) {
        return static_cast<int>(aFrame.u * SHEET_W / 16 + 0.5f);
    }
}

TEST_CASE("sprite_animation shows each frame for its own length", "[sprite_animation]") {
    auto frames = demo_frames();

    sprite_animation subject(frames, SHEET_W, SHEET_H);

    REQUIRE(index_of(subject.at(0.00f)) == 0);
    REQUIRE(index_of(subject.at(0.10f)) == 0);
    REQUIRE(index_of(subject.at(0.30f)) == 1);
    REQUIRE(index_of(subject.at(0.55f)) == 2);
    REQUIRE(index_of(subject.at(0.80f)) == 3);
}

TEST_CASE("sprite_animation loops after the last frame, not before or after that",
    "[sprite_animation]") {
    auto frames = demo_frames();

    sprite_animation subject(frames, SHEET_W, SHEET_H);

    REQUIRE(index_of(subject.at(1.00f)) == 0);
    REQUIRE(index_of(subject.at(1.10f)) == 0);
    REQUIRE(index_of(subject.at(1.80f)) == 3);

    REQUIRE(index_of(subject.at(0.99f)) == 3);
}

TEST_CASE("sprite_animation converts texels to normalized coordinates", "[sprite_animation]") {
    auto frames = demo_frames();

    sprite_animation subject(frames, SHEET_W, SHEET_H);

    const auto second = subject.at(0.30f);

    REQUIRE(second.u == Approx(16.f / SHEET_W));
    REQUIRE(second.v == Approx(0.f));
    REQUIRE(second.w == Approx(16.f / SHEET_W));
    REQUIRE(second.h == Approx(17.f / SHEET_H));
}

TEST_CASE("sprite_animation handles frames of differing lengths", "[sprite_animation]") {
    std::vector<sprite_animation::frame> frames {
        { 0, 0, 16, 17, 0.5f},   
        {16, 0, 16, 17, 0.25f},
    };

    sprite_animation subject(frames, SHEET_W, SHEET_H);

    REQUIRE(index_of(subject.at(0.40f)) == 0);
    REQUIRE(index_of(subject.at(0.60f)) == 1);
    REQUIRE(index_of(subject.at(0.80f)) == 0); 
}

TEST_CASE("sprite_animation rejects animations it could not play", "[sprite_animation]") {
    SECTION("no frames at all") {
        const std::vector<sprite_animation::frame> none;

        REQUIRE_THROWS(sprite_animation(none, SHEET_W, SHEET_H));
    }

    SECTION("a frame of no length") {
        const std::vector<sprite_animation::frame> frames {
            { 0, 0, 16, 17, 0.0f},
            {16, 0, 16, 17, 0.25f},
        };

        REQUIRE_THROWS(sprite_animation(frames, SHEET_W, SHEET_H));
    }

    SECTION("a frame of negative length") {
        const std::vector<sprite_animation::frame> frames {{0, 0, 16, 17, -0.25f}};

        REQUIRE_THROWS(sprite_animation(frames, SHEET_W, SHEET_H));
    }
}

TEST_CASE("sprite_animation reports its own shape", "[sprite_animation]") {
    const auto frames = demo_frames();

    const sprite_animation subject(frames, SHEET_W, SHEET_H);

    REQUIRE(subject.length() == Approx(1.0f));
    REQUIRE(subject.frame_count() == 4);
}

TEST_CASE("sprite_animation wraps a negative time", "[sprite_animation]") {
    const auto frames = demo_frames();

    const sprite_animation subject(frames, SHEET_W, SHEET_H);

    REQUIRE(index_of(subject.at(-0.9f)) == index_of(subject.at(0.1f)));
    REQUIRE(index_of(subject.at(-1.0f)) == index_of(subject.at(0.0f)));
}

TEST_CASE("sprite_animation rejects a sheet or frame that does not add up", "[sprite_animation]") {
    const auto frames = demo_frames();

    SECTION("a sheet with no size") {
        REQUIRE_THROWS(sprite_animation(frames, 0, SHEET_H));
        REQUIRE_THROWS(sprite_animation(frames, SHEET_W, 0));
        REQUIRE_THROWS(sprite_animation(frames, -64, SHEET_H));
    }

    SECTION("a frame that runs off the sheet") {
        const std::vector<sprite_animation::frame> overhang {{56, 0, 16, 17, 0.25f}};

        REQUIRE_THROWS(sprite_animation(overhang, SHEET_W, SHEET_H));
    }

    SECTION("a frame at a negative position") {
        const std::vector<sprite_animation::frame> negative {{-8, 0, 16, 17, 0.25f}};

        REQUIRE_THROWS(sprite_animation(negative, SHEET_W, SHEET_H));
    }

    SECTION("a frame with no area") {
        const std::vector<sprite_animation::frame> empty {{0, 0, 0, 17, 0.25f}};

        REQUIRE_THROWS(sprite_animation(empty, SHEET_W, SHEET_H));
    }
}

TEST_CASE("a once animation stops on its last frame", "[sprite_animation]") {
    const auto frames = demo_frames();

    const sprite_animation subject(frames, SHEET_W, SHEET_H,
        sprite_animation::play_mode::once);

    SECTION("it plays through exactly like a looping one, up to the end") {
        REQUIRE(index_of(subject.at(0.10f)) == 0);
        REQUIRE(index_of(subject.at(0.30f)) == 1);
        REQUIRE(index_of(subject.at(0.55f)) == 2);
        REQUIRE(index_of(subject.at(0.80f)) == 3);
    }

    SECTION("past the end it holds the last frame rather than starting again") {
        REQUIRE(index_of(subject.at(1.00f)) == 3);
        REQUIRE(index_of(subject.at(1.10f)) == 3);
        REQUIRE(index_of(subject.at(100.f)) == 3);
    }

    SECTION("before it starts it shows the first frame rather than wrapping backwards") {
        REQUIRE(index_of(subject.at(-0.10f)) == 0);
        REQUIRE(index_of(subject.at(-100.f)) == 0);
    }

    SECTION("it reports when it is over") {
        REQUIRE(!subject.has_finished(0.00f));
        REQUIRE(!subject.has_finished(0.99f));

        REQUIRE(subject.has_finished(1.00f));
        REQUIRE(subject.has_finished(1.50f));
    }
}

TEST_CASE("a looping animation never finishes", "[sprite_animation]") {
    const auto frames = demo_frames();

    const sprite_animation subject(frames, SHEET_W, SHEET_H);

    REQUIRE(!subject.has_finished(0.0f));
    REQUIRE(!subject.has_finished(1.0f));
    REQUIRE(!subject.has_finished(1000.f));

    REQUIRE(index_of(subject.at(1.10f)) == 0);
}

TEST_CASE("a frame starts showing at exactly its start time", "[sprite_animation]") {
    const auto frames = demo_frames();

    const sprite_animation subject(frames, SHEET_W, SHEET_H);

    REQUIRE(index_of(subject.at(0.00f)) == 0);
    REQUIRE(index_of(subject.at(0.25f)) == 1);
    REQUIRE(index_of(subject.at(0.50f)) == 2);
    REQUIRE(index_of(subject.at(0.75f)) == 3);
}

TEST_CASE("a single frame animation is still an animation", "[sprite_animation]") {
    const std::vector<sprite_animation::frame> one {{16, 0, 16, 17, 0.25f}};

    SECTION("looping") {
        const sprite_animation subject(one, SHEET_W, SHEET_H);

        REQUIRE(subject.frame_count() == 1);
        REQUIRE(subject.length() == Approx(0.25f));

        REQUIRE(index_of(subject.at(0.00f)) == 1);
        REQUIRE(index_of(subject.at(0.24f)) == 1);
        REQUIRE(index_of(subject.at(9.99f)) == 1);
        REQUIRE(index_of(subject.at(-9.99f)) == 1);

        REQUIRE(!subject.has_finished(1000.f));
    }

    SECTION("once") {
        const sprite_animation subject(one, SHEET_W, SHEET_H,
            sprite_animation::play_mode::once);

        REQUIRE(index_of(subject.at(0.00f)) == 1);
        REQUIRE(index_of(subject.at(9.99f)) == 1);

        REQUIRE(!subject.has_finished(0.24f));
        REQUIRE(subject.has_finished(0.25f));
    }
}

TEST_CASE("frames may differ in size as well as in length", "[sprite_animation]") {
    const std::vector<sprite_animation::frame> frames {
        { 0,  0,  16, 17, 0.25f},
        {16, 17,  32, 34, 0.25f}, 
    };

    const sprite_animation subject(frames, SHEET_W, SHEET_H);

    const auto first = subject.at(0.10f);
    const auto second = subject.at(0.30f);

    REQUIRE(first.w == Approx(16.f / SHEET_W));
    REQUIRE(first.h == Approx(17.f / SHEET_H));

    REQUIRE(second.u == Approx(16.f / SHEET_W));
    REQUIRE(second.v == Approx(17.f / SHEET_H));
    REQUIRE(second.w == Approx(32.f / SHEET_W));
    REQUIRE(second.h == Approx(34.f / SHEET_H));
}

TEST_CASE("a frame may sit flush against the edge of the sheet", "[sprite_animation]") {
    const std::vector<sprite_animation::frame> flush {{48, 47, 16, 17, 0.25f}};

    REQUIRE_NOTHROW(sprite_animation(flush, SHEET_W, SHEET_H));

    const sprite_animation subject(flush, SHEET_W, SHEET_H);

    const auto frame = subject.at(0.1f);

    REQUIRE(frame.u + frame.w == Approx(1.f));
    REQUIRE(frame.v + frame.h == Approx(1.f));
}

TEST_CASE("a whole frame may fill the sheet", "[sprite_animation]") {
    const std::vector<sprite_animation::frame> whole {{0, 0, SHEET_W, SHEET_H, 0.25f}};

    const sprite_animation subject(whole, SHEET_W, SHEET_H);

    const auto frame = subject.at(0.1f);

    REQUIRE(frame.u == Approx(0.f));
    REQUIRE(frame.v == Approx(0.f));
    REQUIRE(frame.w == Approx(1.f));
    REQUIRE(frame.h == Approx(1.f));
}

TEST_CASE("a long running clock still wraps correctly", "[sprite_animation]") {
    const auto frames = demo_frames();

    const sprite_animation subject(frames, SHEET_W, SHEET_H);

    REQUIRE(index_of(subject.at(1000.00f)) == 0);
    REQUIRE(index_of(subject.at(1000.30f)) == 1);
    REQUIRE(index_of(subject.at(1000.80f)) == 3);
}
