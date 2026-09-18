// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/graphics/ext/aseprite.h>

#include <gdk/graphics/exception.h>

#include <string>
#include <string_view>

using namespace gdk::graphics;
using namespace gdk::graphics::ext;

namespace {
    [[nodiscard]] aseprite_sheet read(const std::string &aText) {
        return read_aseprite({reinterpret_cast<const std::byte *>(aText.data()), aText.size()});
    }

    void refused(const std::string &aText, const std::string_view aSaying = {}) {
        try {
            read(aText);
        }
        catch (const gdk::graphics::exception &aException) {
            if (!aSaying.empty()) {
                INFO("refused with: " << aException.what());

                REQUIRE(std::string(aException.what()).find(aSaying) != std::string::npos);
            }

            return;
        }

        FAIL("read what should have been refused, from " + aText.substr(0, 60));
    }

    const std::string SHEET = R"({ "frames": {
   "walk 0.aseprite": {
    "frame": { "x": 0, "y": 0, "w": 16, "h": 24 },
    "rotated": false, "trimmed": false,
    "spriteSourceSize": { "x": 0, "y": 0, "w": 16, "h": 24 },
    "sourceSize": { "w": 16, "h": 24 },
    "duration": 100
   },
   "walk 1.aseprite": {
    "frame": { "x": 16, "y": 0, "w": 16, "h": 24 },
    "spriteSourceSize": { "x": 2, "y": 3, "w": 12, "h": 20 },
    "sourceSize": { "w": 16, "h": 24 },
    "duration": 150
   },
   "walk 2.aseprite": {
    "frame": { "x": 32, "y": 0, "w": 16, "h": 24 },
    "sourceSize": { "w": 16, "h": 24 },
    "duration": 200
   }
 },
 "meta": {
  "app": "https://www.aseprite.org/", "version": "1.3",
  "image": "hero.png", "format": "RGBA8888",
  "size": { "w": 48, "h": 24 }, "scale": "1",
  "frameTags": [
   { "name": "idle", "from": 0, "to": 0, "direction": "forward" },
   { "name": "walk", "from": 0, "to": 2, "direction": "forward" },
   { "name": "wave", "from": 1, "to": 2, "direction": "pingpong", "repeat": "3" }
  ],
  "slices": [
   { "name": "feet", "color": "#0000ffff", "keys": [
     { "frame": 0, "bounds": { "x": 7, "y": 22, "w": 2, "h": 2 }, "pivot": { "x": 1, "y": 1 } } ] }
  ]
 }
})";
}

TEST_CASE("an aseprite sheet says its frames, its tags and its slices", "[aseprite]") {
    const auto sheet = read(SHEET);

    REQUIRE(sheet.image == "hero.png");
    REQUIRE(sheet.width == 48);
    REQUIRE(sheet.height == 24);

    REQUIRE(sheet.frames.size() == 3);

    REQUIRE(sheet.frames[1].x == 16);
    REQUIRE(sheet.frames[1].w == 16);
    REQUIRE(sheet.frames[1].milliseconds == 150);

    SECTION("where a frame was trimmed, where it sat in the whole sprite is kept") {
        REQUIRE(sheet.frames[1].offsetX == 2);
        REQUIRE(sheet.frames[1].offsetY == 3);
        REQUIRE(sheet.frames[1].sourceWidth == 16);

        REQUIRE(sheet.frames[0].offsetX == 0);
        REQUIRE(sheet.frames[2].offsetX == 0);
    }

    SECTION("its tags, by name, and which way each plays") {
        REQUIRE(sheet.tags.size() == 3);

        REQUIRE(sheet.tag("walk"));
        REQUIRE(sheet.tag("walk")->from == 0);
        REQUIRE(sheet.tag("walk")->to == 2);
        REQUIRE(sheet.tag("walk")->direction == aseprite_direction::forward);
        REQUIRE(sheet.tag("walk")->repeat == 0);

        REQUIRE(sheet.tag("wave")->direction == aseprite_direction::ping_pong);
        REQUIRE(sheet.tag("wave")->repeat == 3);

        REQUIRE_FALSE(sheet.tag("swim"));
    }

    SECTION("its slices, and the pivot a sheet says where its sprite stands by") {
        REQUIRE(sheet.slices.size() == 1);

        const auto *const pFeet = sheet.slice("feet");

        REQUIRE(pFeet);
        REQUIRE(pFeet->frame == 0);
        REQUIRE(pFeet->x == 7);
        REQUIRE(pFeet->y == 22);
        REQUIRE(pFeet->pivot);
        REQUIRE(pFeet->pivot->first == 1);
        REQUIRE(pFeet->pivot->second == 1);

        REQUIRE_FALSE(sheet.slice("hand"));
    }
}

TEST_CASE("frames come back in the order they were written, both layouts", "[aseprite]") {
    const auto array = read(R"({ "frames": [
        { "frame": { "x": 32, "y": 0, "w": 16, "h": 8 }, "duration": 10 },
        { "frame": { "x": 0, "y": 0, "w": 16, "h": 8 }, "duration": 20 }
      ], "meta": { "image": "a.png", "size": { "w": 48, "h": 8 } } })");

    REQUIRE(array.frames.size() == 2);
    REQUIRE(array.frames[0].x == 32);
    REQUIRE(array.frames[1].x == 0);

    SECTION("and the keyed layout keeps the order it was written in, not the order of its names") {
        const auto keyed = read(R"({ "frames": {
            "z last.aseprite": { "frame": { "x": 32, "y": 0, "w": 16, "h": 8 }, "duration": 10 },
            "a first.aseprite": { "frame": { "x": 0, "y": 0, "w": 16, "h": 8 }, "duration": 20 }
          }, "meta": { "image": "a.png", "size": { "w": 48, "h": 8 } } })");

        REQUIRE(keyed.frames[0].x == 32);
        REQUIRE(keyed.frames[1].x == 0);
    }
}

TEST_CASE("a tag becomes an animation, in the order it plays", "[aseprite]") {
    const auto sheet = read(SHEET);

    SECTION("forward: its frames, with their durations in seconds") {
        const auto frames = frames_of(sheet, *sheet.tag("walk"));

        REQUIRE(frames.size() == 3);
        REQUIRE(frames[0].x == 0);
        REQUIRE(frames[2].x == 32);
        REQUIRE(frames[0].length == Approx(0.1f));
        REQUIRE(frames[2].length == Approx(0.2f));
    }

    SECTION("reverse: the last frame first") {
        aseprite_tag backwards = *sheet.tag("walk");

        backwards.direction = aseprite_direction::reverse;

        const auto frames = frames_of(sheet, backwards);

        REQUIRE(frames.size() == 3);
        REQUIRE(frames[0].x == 32);
        REQUIRE(frames[2].x == 0);
    }

    SECTION("ping pong: there and back, and neither end played twice") {
        aseprite_tag whole = *sheet.tag("walk");

        whole.direction = aseprite_direction::ping_pong;

        const auto three = frames_of(sheet, whole);

        REQUIRE(three.size() == 4);
        REQUIRE(three[0].x == 0);
        REQUIRE(three[1].x == 16);
        REQUIRE(three[2].x == 32);
        REQUIRE(three[3].x == 16);

        const auto two = frames_of(sheet, *sheet.tag("wave"));

        REQUIRE(two.size() == 2);
        REQUIRE(two[0].x == 16);
        REQUIRE(two[1].x == 32);
    }

    SECTION("ping pong the other way round starts at the end") {
        aseprite_tag whole = *sheet.tag("walk");

        whole.direction = aseprite_direction::ping_pong_reverse;

        const auto frames = frames_of(sheet, whole);

        REQUIRE(frames.size() == 4);
        REQUIRE(frames[0].x == 32);
        REQUIRE(frames[1].x == 16);
        REQUIRE(frames[2].x == 0);
        REQUIRE(frames[3].x == 16);
    }

    SECTION("as an animation: what it shows when, and whether it ends") {
        const auto walk = animation_of(sheet, "walk");

        REQUIRE(walk.frame_count() == 3);
        REQUIRE(walk.length() == Approx(0.45f));

        REQUIRE(walk.at(0.0f).u == Approx(0));
        REQUIRE(walk.at(0.0f).w == Approx(16.0f / 48.0f));
        REQUIRE(walk.at(0.12f).u == Approx(16.0f / 48.0f));

        REQUIRE_FALSE(walk.has_finished(10.0f));

        REQUIRE(animation_of(sheet, "wave").has_finished(10.0f));

        REQUIRE_THROWS_AS(animation_of(sheet, "swim"), gdk::graphics::exception);
    }

    SECTION("a sheet with no tags at all is one animation of all of it") {
        const auto whole = animation_of(sheet);

        REQUIRE(whole.frame_count() == 3);
        REQUIRE(whole.length() == Approx(0.45f));
    }
}

TEST_CASE("what is not an aseprite sheet is refused, and says what was wrong", "[aseprite]") {
    SECTION("not json at all, or nothing") {
        refused("", "nothing to read");
        refused("hello");
        refused("{");
        refused(R"({ "frames": )");
        refused(R"({ "frames": {} } trailing)");

        refused(SHEET + " trailing", "after the end");
    }

    SECTION("json, but not a sheet") {
        refused(R"({ "hello": 1 })", "no frames");
        refused(R"({ "frames": [] })");
        refused(R"({ "frames": [{ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "duration": 10 }] })",
            "no meta");

        refused(R"({ "frames": [], "meta": { "image": "a.png", "size": { "w": 8, "h": 8 } } })",
            "no frames in it");
        refused(R"({ "frames": 7, "meta": { "size": { "w": 8, "h": 8 } } })",
            "neither a list nor a table");
    }

    SECTION("a frame that is not one") {
        const auto with = [](const std::string &aFrame) {
            return R"({ "frames": [)" + aFrame + R"(], "meta": { "image": "a.png",
                "size": { "w": 16, "h": 16 } } })";
        };

        REQUIRE_NOTHROW(read(with(R"({ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "duration": 10 })")));

        refused(with(R"({ "duration": 10 })"));
        refused(with(R"({ "frame": { "x": 0, "y": 0, "w": 0, "h": 8 }, "duration": 10 })"));
        refused(with(R"({ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "duration": 0 })"));
        refused(with(R"({ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 } })"));
        refused(with(R"({ "frame": { "x": 12, "y": 0, "w": 8, "h": 8 }, "duration": 10 })"),
            "not inside the image");
        refused(with(R"({ "frame": { "x": -4, "y": 0, "w": 8, "h": 8 }, "duration": 10 })"),
            "not inside the image");
        refused(with(R"({ "frame": { "x": 0, "y": 0, "w": 8.5, "h": 8 }, "duration": 10 })"),
            "not a whole number");

        refused(with(R"({ "frame": { "x": 0, "y": 0, "h": 8 }, "duration": 10 })"),
            "frame rectangle is not a number");
        refused(with(R"({ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "sourceSize": { "h": 8 },
            "duration": 10 })"), "source size is not a number");
    }

    SECTION("an image with no size to it") {
        refused(R"({ "frames": [{ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "duration": 10 }],
            "meta": { "image": "a.png" } })", "image has no size");
    }

    SECTION("a slice with no rectangle to it") {
        refused(R"({ "frames": [{ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "duration": 10 }],
            "meta": { "image": "a.png", "size": { "w": 8, "h": 8 },
              "slices": [ { "name": "feet", "keys": [ { "frame": 0 } ] } ] } })", "slice bounds");
    }

    SECTION("a tag whose frames the sheet does not have, or that plays a way nobody knows") {
        const auto with = [](const std::string &aTag) {
            return R"({ "frames": [{ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "duration": 10 }],
                "meta": { "image": "a.png", "size": { "w": 8, "h": 8 }, "frameTags": [)" + aTag + "] } }";
        };

        REQUIRE_NOTHROW(read(with(R"({ "name": "idle", "from": 0, "to": 0 })")));

        refused(with(R"({ "name": "idle", "from": 0, "to": 3 })"), "does not have");

        refused(with(R"({ "name": "idle", "from": 0, "to": 1 })"), "does not have");

        refused(with(R"({ "name": "idle", "from": -1, "to": 0 })"), "does not have");
        refused(with(R"({ "name": "idle", "from": 1, "to": 0 })"), "does not have");
        refused(with(R"({ "name": "idle", "from": 0, "to": 0, "direction": "sideways" })"),
            "plays a way this does not know");
        refused(with(R"({ "name": "idle", "from": 0, "to": 0, "repeat": "often" })"),
            "says it plays");
    }

    SECTION("json nested deeper than is allowed") {
        std::string deep = R"({ "frames": )";

        for (int i = 0; i < 64; ++i) deep += "[";

        refused(deep);

        std::string nest;

        for (int i = 0; i < 40; ++i) nest += "[";

        nest += "1";

        for (int i = 0; i < 40; ++i) nest += "]";

        refused(R"({ "frames": [{ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "duration": 10 }],
            "meta": { "image": "a.png", "size": { "w": 8, "h": 8 }, "deep": )" + nest + " } }",
            "deeper than is allowed");
    }
}

namespace {
    constexpr std::size_t MAX_ASEPRITE_ENTRIES = 65536;
}

TEST_CASE("a sheet of more frames, tags or slices than are allowed is refused", "[aseprite]") {
    const std::string FRAME = R"({ "frame": { "x": 0, "y": 0, "w": 1, "h": 1 }, "duration": 1 })";

    const std::string SIZE = R"("image": "a.png", "size": { "w": 1, "h": 1 })";

    SECTION("more frames than are allowed, written as a list") {
        std::string many = R"({ "frames": [)";

        for (std::size_t i = 0; i <= MOST_ASEPRITE_FRAMES; ++i) many += FRAME + ",";

        many.pop_back();

        refused(many + R"(], "meta": { )" + SIZE + " } }", "more frames than are allowed");
    }

    SECTION("more frames than are allowed, written as a table") {
        std::string many = R"({ "frames": {)";

        for (std::size_t i = 0; i <= MOST_ASEPRITE_FRAMES; ++i)
            many += "\"" + std::to_string(i) + ".aseprite\": " + FRAME + ",";

        many.pop_back();

        refused(many + R"(}, "meta": { )" + SIZE + " } }", "more frames than are allowed");
    }

    SECTION("more tags than are allowed") {
        std::string many = R"({ "frames": [)" + FRAME + R"(], "meta": { )" + SIZE + R"(, "frameTags": [)";

        for (std::size_t i = 0; i <= MOST_ASEPRITE_FRAMES; ++i)
            many += R"({ "name": "a", "from": 0, "to": 0 },)";

        many.pop_back();

        refused(many + "] } }", "more tags than are allowed");
    }
}

TEST_CASE("what a sheet's json escaped is read back as it was written", "[aseprite]") {
    const std::string E_ACUTE = "\\u00e9";      // é, two bytes of utf-8
    const std::string SUN = "\\u65e5";          // 日, three

    const auto sheet = read(R"({ "frames": [
        { "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "duration": 10 }
      ], "meta": { "image": "art\/h)" + E_ACUTE + R"(ros \"one\".png", "size": { "w": 8, "h": 8 },
        "frameTags": [ { "name": "one\ttwo", "from": 0, "to": 0 },
          { "name": ")" + SUN + R"(", "from": 0, "to": 0 } ] } })");

    REQUIRE(sheet.image == "art/h\xc3\xa9ros \"one\".png");

    REQUIRE(sheet.tags[1].name == "\xe6\x97\xa5");

    REQUIRE(sheet.tags.size() == 2);
    REQUIRE(sheet.tags[0].name == "one\ttwo");
    REQUIRE(sheet.tag("one\ttwo"));

    SECTION("and an escape that is not one is refused") {
        const auto with = [](const std::string &aName) {
            return R"({ "frames": [{ "frame": { "x": 0, "y": 0, "w": 8, "h": 8 }, "duration": 10 }],
                "meta": { "image": ")" + aName + R"(", "size": { "w": 8, "h": 8 } } })";
        };

        refused(with(R"(a\qb)"), "an escape in the json this does not know");
        refused(with(R"(a\u00zz)"), "not four digits");

        refused(R"({ "frames": [], "meta": { "image": "a\u00)", "short escape");
        refused(R"({ "frames": [], "meta": { "image": "a\)", "ends in an escape");
    }
}
