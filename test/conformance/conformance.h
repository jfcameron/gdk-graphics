// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_CONFORMANCE_H
#define GDK_GRAPHICS_CONFORMANCE_H

#include <gdk/graphics/context.h>
#include <gdk/graphics/profile.h>

#include <optional>
#include <string_view>

/// \file
/// \brief the interface's conformance suite: should be used in the test suites of all implementation libraries
namespace gdk::graphics::conformance {
    /// \brief a context of the implementation under test, held to aProfile where one is given
    [[nodiscard]] context_ptr_type make_context(std::optional<profile> aProfile = {});

    //! its name, in the report. eg: `null`, `webgles`
    [[nodiscard]] std::string_view implementation();

    /// \brief the least any target it runs on has: its built-in shaders must fit this. An implementation with no
    /// target of its own, as the null context has none, says nothing is limited.
    [[nodiscard]] profile minimum_profile();

    //! whether the implementation's shaders are checked against its profile: false where it has no shader language
    [[nodiscard]] bool checks_shaders();
}

#endif
