// © Joseph Cameron - MIT License

#include <gdk/graphics/exception.h>
#include <gdk/graphics/ext/png.h>
#include <gdk/graphics/model_data.h>
#include <gdk/graphics/null_context.h>

int main() {
    using namespace gdk::graphics;

    const auto pContext = null_context::make();
    const auto pModel = pContext->make_model(model::usage_hint::upload_once, model_data::make_quad());

    try {
        static_cast<void>(ext::make_from_png({}));

        return 1;
    }
    catch (const exception &) {}

    return std::static_pointer_cast<const null_model>(pModel)->vertex_count() == 6 ? 0 : 1;
}
