// © Joseph Cameron - All Rights Reserved

#include "conformance.h"

#include <gdk/graphics/null_context.h>

#include <utility>

namespace gdk::graphics::conformance {
    context_ptr_type make_context(std::optional<profile> aProfile) {
        return null_context::make(std::move(aProfile));
    }

    std::string_view implementation() { return "null"; }
}
