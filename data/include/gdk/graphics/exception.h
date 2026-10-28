// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXCEPTION_H
#define GDK_GRAPHICS_EXCEPTION_H

#include <exception>
#include <string>
#include <utility>

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
