#pragma once

#include <cstdint>
#include <glm/mat4x4.hpp>

#include "forward_declarations.hpp"

struct GLFWwindow;

namespace grx {
    class Window {
    public:
        Window(const char *name, uint32_t width, uint32_t height);

        ~Window();

        void setCamera(Camera* camera);
        void setCamera(const CameraPtr& camera);
        Camera* getCamera();

        void makeCurrent();
        bool isShouldClose();
        void swapBuffers();

        int  getKey      (int key);
        bool isKeyPress  (int key);
        bool isKeyRelease(int key);

        bool isMouseRightButtonPress();
        bool isMouseLeftButtonPress ();

        glm::vec2 getMousePos  ();
        void      setMousePos  (const glm::vec2& position);
        void      resetMousePos();
        glm::vec2 getSize      ();
        bool      onFocus      () { return _onFocus; }

        glm::mat4 update_view_projection();

    protected:
        static void glfwFocusCallback(GLFWwindow* window, int focused);

        bool        _onFocus;
        CameraPtr   currentCam;
        GLFWwindow* glfwWindow;
    };
}