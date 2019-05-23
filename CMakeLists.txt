cmake_minimum_required(VERSION 3.13)
project(opengl_test_framework)

set(CMAKE_CXX_STANDARD 17)

include_directories(3rd/include)
link_directories(3rd/lib)

add_executable(
        opengl_test_framework
        main.cpp
        src/GraphicsContext.cpp
        src/Window.cpp
        src/ShaderLoader.cpp
        src/TextureManager.cpp
        src/Camera.cpp
        src/CameraManipulator.cpp
        src/InputContext.cpp
)
target_link_libraries(opengl_test_framework GL GLEW glfw X11 gainput -pthread)