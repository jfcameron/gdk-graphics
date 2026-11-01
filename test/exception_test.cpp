// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>

#include <jfc/catch.hpp>

#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

using gdk::graphics::exception;

static_assert(std::is_base_of_v<std::exception, exception>);
static_assert(!std::is_base_of_v<std::runtime_error, exception>);
static_assert(std::is_nothrow_copy_constructible_v<exception>);
static_assert(std::is_nothrow_copy_assignable_v<exception>);

TEST_CASE("the root exception keeps its message", "[exception]") {
    REQUIRE(std::string(exception("a reason").what()) == "a reason");
}

TEST_CASE("a root exception moved from keeps its message", "[exception]") {
    exception from("a reason");
    const exception to(std::move(from));

    REQUIRE(std::string(from.what()) == "a reason");
    REQUIRE(std::string(to.what()) == "a reason");
}

TEST_CASE("the root exception names itself when given no message", "[exception]") {
    REQUIRE(std::string(exception().what()) == "gdk::graphics::exception");
}
