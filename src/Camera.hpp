#pragma once

#include "forward_declarations.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>

#include "SingletonMacro.hpp"

namespace grx {
    /**
     * Simple camera implementation
     *
     * Supports basic camera functions and input from camera manipulators
     */
    class Camera {
    public:
        explicit Camera(
                const glm::vec3 &pos = {0.f, 0.f, -2.f},
                float aspectRatio = 16.f / 9.f,
                float fov = 73.f,
                float zNear = 0.1f,
                float zFar = 2048.f
        ) : _pos(pos), _aspect(aspectRatio), _fov(fov), _zNear(zNear), _zFar(zFar)
        {
            initDefaultCameraManipulator();
        }

        DEFINE_GETTER(_pos, position);
        DEFINE_GETTER(_aspect, aspect_ratio);
        DEFINE_GETTER(_zNear, z_near);
        DEFINE_GETTER(_zFar, z_far);

        DEFINE_CONST_GETTER(_fov, fov);
        DEFINE_CONST_GETTER(_orient, orientation);
        DEFINE_CONST_GETTER(_dir, dir);
        DEFINE_CONST_GETTER(_right, right);
        DEFINE_CONST_GETTER(_up, up);

        void fov(float field_of_view) {
            _fov = field_of_view;
            normalizeFov();
        }

        Camera& look_at(const glm::vec3& pos);

        void setCameraManipulator(CameraManipulator* cameraManipulator);
        void setCameraManipulator(const CameraManipulatorPtr& cameraManipulator);
        CameraManipulator* getCameraManipulator();

        /**
         * Update camera manipulators, get view_projection matrix
         * @param window pointer to window, may be nullptr
         * @return View * Projection matrix
         */
        glm::mat4 update_view_projection(Window* window);

    protected:
        void initDefaultCameraManipulator();

        void modYaw() {
            if (_yaw > pi)
                _yaw -= pi2;
            else if (_yaw < -pi)
                _yaw += pi2;
        }

        void normalizeFov() {
            if (_fov > fov_max)
                _fov = fov_max;
            else if (_fov < fov_min)
                _fov = fov_min;
        }

        void normalizePitch() {
            if (_pitch > pitch_lock)
                _pitch = pitch_lock;
            else if (_pitch < -pitch_lock)
                _pitch = -pitch_lock;
        }

        void normalizeRoll() {
            if (_roll > roll_lock)
                _roll = roll_lock;
            else if (_roll < -roll_lock)
                _roll = -roll_lock;
        }

        glm::vec3 _pos;
        glm::vec3 _dir    = {0.f, 0.f, 1.f};
        glm::vec3 _right  = {1.f, 0.f, 0.f};
        glm::vec3 _up     = {0.f, 1.f, 0.f};
        glm::mat4 _orient = glm::mat4(1.f);

        const float pi      = glm::pi     <float>();
        const float pi2     = glm::two_pi <float>();
        float pitch_lock    = glm::half_pi<float>() - 0.05f;
        float roll_lock     = glm::half_pi<float>() - 0.1f;
        const float fov_min = 10.f;
        const float fov_max = 180.f;

        float _aspect;
        float _fov;
        float _zNear;
        float _zFar;

        float _yaw   = 0.f;
        float _pitch = 0.f;
        float _roll  = 0.f;

        CameraManipulatorPtr _cameraManipulator = nullptr;
    };
}