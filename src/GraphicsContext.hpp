#pragma once

#include "SingletonMacro.hpp"

namespace grx_ctx {
    class GraphicsContext {

    protected:
        static void glfwError(int id, const char *description);

    MARK_AS_SINGLETON(GraphicsContext);
    };
}

namespace grx {
    /**
     * @return Main graphics context
     */
    inline auto &current_context() {
        return grx_ctx::GraphicsContext::instance();
    }
}