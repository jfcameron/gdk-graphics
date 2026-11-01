// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_EXCEPTION_H
#define GDK_GRAPHICS_EXCEPTION_H

#include <exception>
#include <memory>
#include <string>

namespace gdk::graphics {
    /// \brief root exception type for this project
    class exception : public std::exception {
        std::shared_ptr<const std::string> mpWhat;

    public:
        exception()
        : exception("gdk::graphics::exception")
        {}

        exception(const std::string &aWhat)
        : mpWhat(std::make_shared<const std::string>(aWhat))
        {}

        exception(const exception &aOther) noexcept = default;

        exception &operator=(const exception &aOther) noexcept = default;

        const char *what() const noexcept override { return mpWhat->c_str(); }
    };
}

#endif
