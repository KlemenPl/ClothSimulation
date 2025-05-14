//
// Created by Klemen Plestenjak on 2/19/25.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

typedef struct OrbitCamera {
    vec3 position;
    vec3 target;
    vec3 up;

    float distance;
    float minDistance;
    float maxDistance;
    float yaw;
    float pitch;

    mat4 view;
    mat4 proj;
    mat4 viewProj;
} OrbitCamera;

static const OrbitCamera ORBIT_CAMERA_DEFAULT = {
    .position = {0.0f, 0.0f, 0.0f},
    .target = {0.0f, 0.0f, 0.0f},
    .up = {0.0f, 0.0f, 1.0f},

    .distance = 10.0f,
    .minDistance = 1.0f,
    .maxDistance = 1000.0f,
    .yaw = 0.0f,
    .pitch = 0.0f,
};

static void orbitCameraUpdate(OrbitCamera* camera) {
    camera->pitch = glm_clamp(camera->pitch, glm_rad(0.1f), glm_rad(89.9f));
    camera->distance = glm_clamp(camera->distance, camera->minDistance, camera->maxDistance);

    float phi = camera->pitch;
    float theta = camera->yaw;

    float x = camera->distance * sinf(phi) * sinf(theta);
    float y = camera->distance * sinf(phi) * cosf(theta);
    float z = camera->distance * cosf(phi);

    camera->position[0] = camera->target[0] + x;
    camera->position[1] = camera->target[1] - y;
    camera->position[2] = camera->target[2] + z;

    glm_mat4_identity(camera->view);
    glm_perspective(glm_rad(45.0f), 16.0f / 9.0f, 0.1f, 10000.0f, camera->proj);
    glm_lookat(camera->position, camera->target, camera->up, camera->view);
    glm_mat4_mul(camera->proj, camera->view, camera->viewProj);
}

static void orbitCameraRotate(OrbitCamera* camera, float deltaYaw, float deltaPitch) {
    camera->yaw += deltaYaw;
    camera->pitch += deltaPitch;

    while (camera->yaw >= 2.0f * GLM_PI)
        camera->yaw -= 2.0f * GLM_PI;
    while (camera->yaw <= -2.0f * GLM_PI)
        camera->yaw += 2.0f * GLM_PI;
}
static void orbitCameraZoom(OrbitCamera* camera, float delta) {
    camera->distance *= (1.0f - delta * 0.1f);
}
static void orbitCameraPan(OrbitCamera* camera, float deltaX, float deltaY) {
    vec3 right;
    vec3 forward;

    vec3 dir;
    glm_vec3_sub(camera->target, camera->position, dir);
    glm_vec3_normalize(dir);

    glm_vec3_cross(dir, camera->up, right);
    glm_vec3_normalize(right);

    glm_vec3_cross(camera->up, right, forward);
    glm_vec3_normalize(forward);

    float dstFactor = camera->distance * 0.01f;

    camera->target[0] += (right[0] * deltaX + forward[0] * deltaY) * dstFactor;
    camera->target[1] += (right[1] * deltaX + forward[1] * deltaY) * dstFactor;
    camera->target[2] += (right[2] * deltaX + forward[2] * deltaY) * dstFactor;

}

#endif