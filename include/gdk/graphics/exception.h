// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_EXCEPTION_H
#define GDK_GFX_EXCEPTION_H

#include <gdk/graphics/types.h>

#include <exception>
#include <string>

namespace gdk::graphics {
    /// \brief root exception type for this project
    class exception : public std::exception {
        std::string mWhat = "gdk::graphics::exception";

    public:
        virtual const char *what() const noexcept override {
            return mWhat.c_str();
        }

        exception() = default;

        exception(std::string aWhat)
        : mWhat(std::move(aWhat))
        {}

        virtual ~exception() override = default;
    };
}

#endif

