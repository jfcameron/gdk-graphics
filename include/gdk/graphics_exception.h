// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_EXCEPTION_H
#define GDK_GFX_EXCEPTION_H

#include <gdk/graphics_types.h>

#include <exception>

namespace gdk {
    /// \brief root exception type for this project
    class graphics_exception : public std::exception {
        std::string mWhat = "gdk::graphics_exception";

    public:
        virtual const char *what() const noexcept override {
            return mWhat.c_str();
        }

        graphics_exception() = default;

        graphics_exception(std::string aWhat)
        : mWhat(std::move(aWhat))
        {}

        virtual ~graphics_exception() override = default;
    };
}

#endif

