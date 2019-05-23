#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "forward_declarations.hpp"
#include "SingletonMacro.hpp"
#include "InputContext.hpp"

namespace grx {
    /**
     * Define the interface for classes that can control cameras
     */
    class CameraManipulator {
    public:
        using vec3 = glm::vec3;
        using vec3cr = const glm::vec3&;

        CameraManipulator();
        virtual ~CameraManipulator() = default;

    protected:
        friend Camera;

        void updateStart();
        virtual void updateFov        (Window* window, float& fov) = 0;
        virtual void updateOrientation(Window* window, float& yaw, float& pitch, float& roll) = 0;
        virtual void updatePosition   (Window* window, vec3& position, vec3cr direction, vec3cr right, vec3cr up) = 0;


        double _lastUpdateTime;
        float  _timestep = 0.0;
    };


    /**
     * Standard flight control
     */
    class CameraManipulatorFly : public CameraManipulator {
    public:
        explicit CameraManipulatorFly(float speed = 4.f, float mouseSpeed = 2.f);

        DEFINE_GETTER(_speed, walk_speed);
        DEFINE_GETTER(_mouseSpeed, mouse_speed);

    protected:
        void updateFov         (Window* window, float& fov) override;
        void updateOrientation (Window* window, float& yaw, float& pitch, float& roll) override;
        void updatePosition    (Window* window, vec3& position, vec3cr direction, vec3cr right, vec3cr up) override;

        float  _shift_factor = 4.f;
        float  _speed;
        float  _mouseSpeed;

        gainput::InputMap input_map = input::ctx().createInputMap();
    };
}