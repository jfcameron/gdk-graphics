// © Joseph Cameron - MIT License

#ifndef GDK_GRAPHICS_MODEL_H
#define GDK_GRAPHICS_MODEL_H

#include <gdk/graphics/model_data.h>

namespace gdk::graphics {
    /// \brief Vertex data representing a 3D graphical object
    class model {
    public:
        enum class usage_hint {
            dynamic,   //!< data will be rewritten 
            streaming, //!< data will be rewritten extremely frequently
            upload_once //!< data will only be written once
        };

        /// \brief replace vertex data held by this model
        virtual void upload(const usage_hint &, const model_data &vertexDataView) = 0;

        virtual ~model() = default;

    protected:
        model() = default;
    };
}

#endif

