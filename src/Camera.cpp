#include "Camera.hpp"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.hpp"
#include "CameraManipulator.hpp"

void grx::Camera::initDefaultCameraManipulator() {
    _cameraManipulator = std::make_shared<CameraManipulatorFly>();
}

grx::Camera& grx::Camera::look_at(const glm::vec3& pos) {
    if (pos != _pos) {
        _dir   =  glm::normalize(pos - _pos);
        _pitch =  (asinf(-_dir.y));
        _yaw   = -(atan2f(-_dir.x, -_dir.z));
        _roll  = 0.0f;

        modYaw();
        normalizePitch();

        _orient = glm::rotate(glm::mat4(1.f), _pitch, glm::vec3(1.f, 0.f, 0.f));
        _orient = glm::rotate(_orient, _yaw,   glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 inv_orient = glm::inverse(_orient);

        _right  = inv_orient * glm::vec4(1.f, 0.f,  0.f, 1.f);
        _up     = glm::cross (_right, _dir);
        _orient = glm::rotate(_orient, _roll, _dir);
    }

    return *this;
}

glm::mat4 grx::Camera::update_view_projection(grx::Window *window) {
    if (_cameraManipulator && window && window->onFocus()) {
        _cameraManipulator->updateStart();
        _cameraManipulator->updateFov(window, _fov);
        _cameraManipulator->updateOrientation(window, _yaw, _pitch, _roll);

        modYaw();
        normalizeFov();
        normalizePitch();
        normalizeRoll();
    }

    _orient = glm::rotate(glm::mat4(1.f), _pitch, glm::vec3(1.f, 0.f, 0.f));
    _orient = glm::rotate(_orient,          _yaw, glm::vec3(0.f, 1.f, 0.f));

    glm::mat4 inv_orient = glm::inverse(_orient);

    _dir    = inv_orient * glm::vec4(0.f, 0.f, -1.f, 1.f);
    _right  = inv_orient * glm::vec4(1.f, 0.f,  0.f, 1.f);
    _up     = glm::cross (_right,  _dir);
    _orient = glm::rotate(_orient, _roll, _dir);

    if (_cameraManipulator && window && window->onFocus())
        _cameraManipulator->updatePosition(window, _pos, _dir, _right, _up);

    auto view        = _orient * glm::translate(glm::mat4(1.f), -_pos);
    auto projection  = glm::perspective(glm::radians(_fov), _aspect, _zNear, _zFar);

    return projection * view;
}

void grx::Camera::setCameraManipulator(CameraManipulator* cameraManipulator) {
    _cameraManipulator = CameraManipulatorPtr(cameraManipulator);
}

void grx::Camera::setCameraManipulator(const CameraManipulatorPtr& cameraManipulator) {
    _cameraManipulator = cameraManipulator;
}

grx::CameraManipulator* grx::Camera::getCameraManipulator() {
    return _cameraManipulator.get();
}