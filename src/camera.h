//
// Created by Klemen Plestenjak on 2/19/25.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

typedef struct Camera {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float distance;
    float minDistance;
    float maxDistance;
    float yaw;
    float pitch;

    mat4 view;
    mat4 proj;
    mat4 viewProj;
} Camera;

static const Camera CAMERA_DEFAULT = {
    .position = {0.0f, 0.0f, 0.0f},
    .front = {0.0f, 0.0f, -1.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .right = {1.0f, 0.0f, 0.0f},
    .worldUp = {0.0f, 1.0f, 0.0f},

    .distance = 10.0f,
    .minDistance = 1.0f,
    .maxDistance = 1000.0f,
    .yaw = 0.0f,
    .pitch = 0.0f,
};

static void cameraUpdate(Camera* camera) {
    vec3 front;
    front[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
    front[1] = sinf(glm_rad(camera->pitch));
    front[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));

    glm_vec3_normalize_to(front, camera->front);

    // Re-calculate the right and up vectors
    glm_vec3_cross(camera->front, camera->worldUp, camera->right);
    glm_vec3_normalize(camera->right);

    glm_vec3_cross(camera->right, camera->front, camera->up);
    glm_vec3_normalize(camera->up);

    // Calculate look-at target position
    vec3 target;
    glm_vec3_add(camera->position, camera->front, target);

    // Update view matrix
    glm_mat4_identity(camera->view);
    glm_lookat(camera->position, target, camera->up, camera->view);

    // Update projection matrix
    glm_perspective(glm_rad(45.0f), 16.0f / 9.0f, 0.1f, 10000.0f, camera->proj);

    // Calculate combined view-projection matrix
    glm_mat4_mul(camera->proj, camera->view, camera->viewProj);
}

static void cameraMove(Camera* camera, vec3 direction, float delta) {
    vec3 displacement = GLM_VEC3_ZERO_INIT;
    vec3 worldDirection = GLM_VEC3_ZERO_INIT;

    if (direction[0] != 0) {  // Right/Left movement
        vec3 rightScaled;
        glm_vec3_scale(camera->right, direction[0], rightScaled);
        glm_vec3_add(worldDirection, rightScaled, worldDirection);
    }

    if (direction[1] != 0) {  // Up/Down movement
        vec3 upScaled;
        glm_vec3_scale(camera->up, direction[1], upScaled);
        glm_vec3_add(worldDirection, upScaled, worldDirection);
    }

    if (direction[2] != 0) {  // Forward/Backward movement
        vec3 frontScaled;
        glm_vec3_scale(camera->front, direction[2], frontScaled);
        glm_vec3_add(worldDirection, frontScaled, worldDirection);
    }

    glm_vec3_scale(worldDirection, delta, displacement);
    glm_vec3_add(camera->position, displacement, camera->position);
}

static void cameraRotate(Camera* camera, float dx, float dy, bool constrainPitch) {
    camera->yaw += dx;
    camera->pitch += dy;

    // Constrain the pitch to avoid gimbal lock
    if (constrainPitch) {
        if (camera->pitch > 89.0f)
            camera->pitch = 89.0f;
        if (camera->pitch < -89.0f)
            camera->pitch = -89.0f;
    }
}
#endif