// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GRAPHICS_CONFORMANCE_H
#define GDK_GRAPHICS_CONFORMANCE_H

#include <gdk/graphics/context.h>
#include <gdk/graphics/profile.h>

#include <optional>
#include <string_view>

/// \file
/// \brief the interface's conformance suite: should be used in the test suites of all implementation libraries
namespace gdk::graphics::conformance {
    /// \brief **a context of the implementation under test**, held to aProfile where one is given
    [[nodiscard]] context_ptr_type make_context(std::optional<profile> aProfile = {});

    //! **its name**, in the report: `null`, `webgles`
    [[nodiscard]] std::string_view implementation();
}

#endif
