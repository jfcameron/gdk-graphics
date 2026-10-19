// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/graphics/string_keyed.h>

#include <string>
#include <string_view>
#include <type_traits>

using namespace gdk::graphics;

namespace {
    template<typename map_type, typename key_type, typename = void>
    struct searchable_with : std::false_type {};

    template<typename map_type, typename key_type>
    struct searchable_with<map_type, key_type,
        std::void_t<decltype(std::declval<const map_type &>().find(std::declval<key_type>()))>>
        : std::true_type {};
}

TEST_CASE("a string keyed map can be searched without building a string", "[string_keyed]")
{
    static_assert(!searchable_with<std::unordered_map<std::string, int>, std::string_view>::value,
        "a plain unordered_map should not accept a string_view key");

    static_assert(searchable_with<string_keyed<int>, std::string_view>::value,
        "string_keyed exists to accept one");

    string_keyed<int> subject;

    subject.emplace("_Colour", 1);
    subject.emplace("_Offset", 2);

    SECTION("a view finds what a string inserted")
    {
        const std::string_view name("_Colour");

        const auto found = subject.find(name);

        REQUIRE(found != subject.end());
        REQUIRE(found->second == 1);
        REQUIRE(found->first == "_Colour");
    }

    SECTION("and so does a literal, with no temporary in between")
    {
        REQUIRE(subject.find("_Offset") != subject.end());
        REQUIRE(subject.find("_NotHere") == subject.end());
    }

    SECTION("a view over a longer buffer matches only its own extent")
    {
        const std::string backing("_ColourAndThenSome");

        REQUIRE(subject.find(std::string_view(backing).substr(0, 7)) != subject.end());
        REQUIRE(subject.find(std::string_view(backing)) == subject.end());
    }

    SECTION("the hash agrees with the one the standard uses for the same characters")
    {
        REQUIRE(transparent_string_hash{}(std::string_view("_Colour"))
            == std::hash<std::string_view>{}(std::string_view("_Colour")));
    }
}
