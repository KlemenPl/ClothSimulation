#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "app.h"
#include "camera.h"

#define MAX_N 500

typedef struct Particle {
    vec3 prevPosition;
    vec3 position;
    vec3 velocity;
    float mass;
    float invMass;
    bool isFixed;
} Particle;

typedef struct DistanceConstraint {
    int32_t p1;
    int32_t p2;
    float restLength;
} DistanceConstraint;

typedef struct ParticleVertex {
    vec3 position;
    vec4 color;
} ParticleVertex;

typedef struct Uniform {
    mat4 viewProj;
} Uniform;

struct {
    bool drawMesh;
    bool simulate;
    bool visualizeStrain;
    int32_t numParticles;
    float meshSize;
    float particleMass;

    float dampingFactor;
    int32_t numSubsteps;
    int32_t solverIterations;

} simOpts = {
    .drawMesh = false,
    .simulate = false,
    .visualizeStrain = true,
    .numParticles = 100,
    .meshSize = 50.0f,
    .particleMass = 1.0f,
    .dampingFactor = 0.995f,
    .numSubsteps = 1,
    .solverIterations = 3,
};

WGPUQueue queue;
WGPUBindGroup pipelineBindGroup;
WGPURenderPipeline clothPipeline;
WGPURenderPipeline clothMeshPipeline;

WGPUBuffer uniformBuffer;
WGPUBuffer vertexBuffer;
WGPUBuffer indexBuffer;

int32_t gridSize;
int32_t numParticles;
int32_t particleWidth;
int32_t numConstraints;
Particle *particles;
DistanceConstraint *constraints;
ParticleVertex *vertices;
uint32_t *indices;

void initParticles(const AppState *app) {
    int32_t n = simOpts.numParticles;
    gridSize = n;
    numParticles = (gridSize + 1) * (gridSize + 1);
    particleWidth = gridSize + 1;
    float spacing = (float) simOpts.meshSize / (gridSize);
    numConstraints = numParticles * 6;

    int32_t constraintIdx = 0;
    float hSpacing = sqrtf(spacing * spacing + spacing * spacing);

    for (int32_t y = 0; y < particleWidth; y++) {
        for (int32_t x = 0; x < particleWidth; x++) {
            int32_t idx = y * particleWidth + x;
            particles[idx] = (Particle){
                .position = {(float) x * spacing, 0, (float) y * spacing},
                .velocity = {0, 0, 0},
                .mass = simOpts.particleMass,
                .invMass = 1.0f / simOpts.particleMass,
                .isFixed = false,
            };

            if (x > 0) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx - 1,
                    .restLength = spacing,
                };
            }
            if (x < particleWidth - 1) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx + 1,
                    .restLength = spacing,
                };
            }
            if (y > 0) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx - particleWidth,
                    .restLength = spacing,
                };
            }
            if (y < particleWidth - 1) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx + particleWidth,
                    .restLength = spacing,
                };
            }

            // Horizontal
            if (x > 0 && y > 0) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx - particleWidth - 1,
                    .restLength = hSpacing,
                };
            }
            if (x < particleWidth - 1 && y < particleWidth - 1) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx + particleWidth + 1,
                    .restLength = hSpacing,
                };
            }


        }
    }
    numConstraints = constraintIdx;
    int32_t lastRow = (gridSize - 1) * particleWidth;
    particles[lastRow].isFixed = true;
    particles[lastRow + particleWidth - 1].isFixed = true;

    int32_t numIndices = gridSize * gridSize * 6;

    int32_t offset = 0;
    for (int32_t y = 1; y < particleWidth; y++) {
        for (int32_t x = 1; x < particleWidth; x++) {
            const uint32_t topRight = (y - 1) * particleWidth + x;
            const uint32_t topLeft = (y - 1) * particleWidth + x - 1;
            const uint32_t bottomLeft = y * particleWidth + x - 1;
            const uint32_t bottomRight = y * particleWidth + x;

            // Top left triangle (CCW)
            indices[offset + 0] = topRight;
            indices[offset + 1] = topLeft;
            indices[offset + 2] = bottomLeft;
            // Bottom right triangle (CCW)
            indices[offset + 3] = bottomLeft;
            indices[offset + 4] = bottomRight;
            indices[offset + 5] = topRight;
            offset += 6;
        }
    }

    wgpuQueueWriteBuffer(queue, indexBuffer, 0, indices, numIndices * sizeof(*indices));

}


int init(const AppState *app, int argc, const char **argv) {
    queue = wgpuDeviceGetQueue(app->device);

    uniformBuffer = wgpuDeviceCreateBuffer(app->device, &(WGPUBufferDescriptor) {
        .label = "Uniform Buffer",
        .size = sizeof(Uniform),
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
    });


    WGPUBindGroupLayout bgLayout = wgpuDeviceCreateBindGroupLayout(app->device, &(WGPUBindGroupLayoutDescriptor){
        .entryCount = 1,
        .entries = (WGPUBindGroupLayoutEntry[]){
            [0] = {
                .binding = 0,
                .visibility = WGPUShaderStage_Vertex,
                .buffer.type = WGPUBufferBindingType_Uniform,
            }
        }
    });
    pipelineBindGroup = wgpuDeviceCreateBindGroup(app->device, &(WGPUBindGroupDescriptor){
        .layout = bgLayout,
        .entryCount = 1,
        .entries = (WGPUBindGroupEntry[]){
            [0] = {
                .binding = 0,
                .buffer = uniformBuffer,
                .offset = 0,
                .size = sizeof(Uniform),
            }
        }
    });
    WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(app->device, &(WGPUPipelineLayoutDescriptor){
        .bindGroupLayoutCount = 1,
        .bindGroupLayouts = (WGPUBindGroupLayout[]) {
            bgLayout,
        },
    });
    wgpuBindGroupLayoutRelease(bgLayout);

    WGPUShaderModule shaderModule = createWGSLShaderModule(app->device, "assets/cloth.wgsl");


    WGPURenderPipelineDescriptor pipelineDesc = {
        .layout = pipelineLayout,
        .primitive.topology = WGPUPrimitiveTopology_TriangleList,
        .primitive.cullMode = WGPUCullMode_None,
        .vertex = (WGPUVertexState) {
            .module = shaderModule,
            .entryPoint = "vs_main",
            .bufferCount = 1,
            .buffers = (WGPUVertexBufferLayout[]) {
                [0] = {
                    .arrayStride = sizeof(ParticleVertex),
                    .stepMode = WGPUVertexStepMode_Vertex,
                    .attributeCount = 2,
                    .attributes = (WGPUVertexAttribute[]) {
                        [0] = {
                            .format = WGPUVertexFormat_Float32x3,
                            .offset = offsetof(ParticleVertex, position),
                            .shaderLocation = 0
                        },
                        [1] = {
                            .format = WGPUVertexFormat_Float32x4,
                            .offset = offsetof(ParticleVertex, color),
                            .shaderLocation = 1,
                        }
                    }
                }
            }
        },
        .fragment = &(WGPUFragmentState) {
            .module = shaderModule,
            .entryPoint = "fs_main",
            .targetCount = 1,
            .targets = (WGPUColorTargetState[]) {
                [0].format = app->format,
                [0].writeMask = WGPUColorWriteMask_All,
            },
        },
        .depthStencil = NULL,
        .multisample = (WGPUMultisampleState) {
            .count = 1,
            .mask = ~0u,
            .alphaToCoverageEnabled = false
        },
    };
    clothPipeline = wgpuDeviceCreateRenderPipeline(app->device, &pipelineDesc);
    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_LineList;
    clothMeshPipeline = wgpuDeviceCreateRenderPipeline(app->device, &pipelineDesc);

    wgpuShaderModuleRelease(shaderModule);
    wgpuPipelineLayoutRelease(pipelineLayout);

    int32_t maxParticles = MAX_N * MAX_N;
    particles = malloc(sizeof(*particles) * maxParticles);
    constraints = malloc(sizeof(*constraints) * maxParticles * 6);
    vertexBuffer = wgpuDeviceCreateBuffer(app->device, &(WGPUBufferDescriptor){
        .label = "Vertex Buffer",
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        .size = maxParticles * sizeof(ParticleVertex),
    });
    vertices = malloc(sizeof(*vertices) * maxParticles);
    int32_t maxIndices = MAX_N * MAX_N * 6;
    indexBuffer = wgpuDeviceCreateBuffer(app->device, &(WGPUBufferDescriptor){
        .label = "Index Buffer",
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index,
        .size = maxIndices * sizeof(uint32_t),
    });

    indices = malloc(sizeof(*indices) * maxIndices);
    initParticles(app);

    return 0;
}
void deinit(const AppState *app) {
    wgpuRenderPipelineRelease(clothMeshPipeline);
    wgpuRenderPipelineRelease(clothPipeline);
    wgpuBindGroupRelease(pipelineBindGroup);

    wgpuBufferRelease(uniformBuffer);
    wgpuBufferRelease(vertexBuffer);
    wgpuBufferRelease(indexBuffer);

    wgpuQueueRelease(queue);

    free(indices);
    free(vertices);
    free(particles);
    free(constraints);
}

void integrateParticles(float dt) {
    static vec3 gravity = {0.0f, -9.81f, 0.0f};
    for (int32_t i = 0; i < numParticles; i++) {
        if (particles[i].isFixed) continue;
        glm_vec3_copy(particles[i].position, particles[i].prevPosition);

        vec3 force;
        glm_vec3_scale(gravity, particles[i].mass, force);

        vec3 acceleration;
        glm_vec3_scale(force, particles[i].invMass, acceleration);

        vec3 velocityChange;
        glm_vec3_scale(acceleration, dt, velocityChange);
        glm_vec3_add(particles[i].velocity, velocityChange, particles[i].velocity);

        vec3 positionChange;
        glm_vec3_scale(particles[i].velocity, dt, positionChange);
        glm_vec3_add(particles[i].position, positionChange, particles[i].position);
    }
}

void solveDistanceConstraint(int32_t cIdx) {
    DistanceConstraint *constraint = constraints + cIdx;

    Particle *p1 = particles + constraint->p1;
    Particle *p2 = particles + constraint->p2;

    if (p1->isFixed && p2->isFixed)
        return;

    vec3 delta;
    glm_vec3_sub(p1->position, p2->position, delta);
    float length = glm_vec3_norm(delta);
    if (length < 1e-3) return;

    float C = length - constraint->restLength;
    vec3 direction;
    glm_vec3_divs(delta, length, direction);

    float w1 = p1->isFixed ? 0 : p1->invMass;
    float w2 = p2->isFixed ? 0 : p2->invMass;
    float totalWeight = w1 + w2;
    if (totalWeight < 1e-5) return;

    float correction = C / totalWeight;

    if (!p1->isFixed) {
        glm_vec3_muladds(direction, -(w1 * correction), p1->position);
    }
    if (!p2->isFixed) {
        glm_vec3_muladds(direction, (w2 * correction), p2->position);
    }
}

void simulateCloth(float dt) {
    float subDt = dt / simOpts.numSubsteps;

    for (int32_t substep = 0; substep < simOpts.numSubsteps; substep++) {
        integrateParticles(subDt);

        for (int32_t iter = 0; iter < simOpts.solverIterations; iter++) {
            for (int32_t i = 0; i < numConstraints; i++) {
                solveDistanceConstraint(i);
            }
        }

        for (int32_t i = 0; i < numParticles; i++) {
            if (particles[i].isFixed) continue;

            glm_vec3_sub(particles[i].position, particles[i].prevPosition, particles[i].velocity);
            glm_vec3_divs(particles[i].velocity, subDt, particles[i].velocity);

            float dampingFactor = simOpts.dampingFactor;
            glm_vec3_scale(particles[i].velocity, dampingFactor, particles[i].velocity);
        }
    }
}

int32_t selectedParticle = -1;
vec3 rayOrigin;
vec3 rayDirection;
vec3 dragPlaneNormal;
vec3 dragPlanePoint;
float dragDepth;

void castRayFromCamera(const Camera *camera, float screenX, float screenY, vec3 origin, vec3 direction) {
    // convert to NDC
    float ndcX = (2.0f * screenX) - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY);

    vec4 clipPos = {ndcX, ndcY, -1.0f, 1.0f};

    // View space
    vec4 viewPos;
    mat4 invProj;
    glm_mat4_inv(camera->proj, invProj);
    glm_mat4_mulv(invProj, clipPos, viewPos);
    viewPos[2] = -1.0f;
    viewPos[3] = 0.0f;

    // World Space
    vec4 worldPos;
    mat4 invView;
    glm_mat4_inv(camera->view, invView);
    glm_mat4_mulv(invView, viewPos, worldPos);

    glm_vec3_copy(camera->position, origin);
    glm_vec3_copy((vec3){worldPos[0], worldPos[1], worldPos[2]}, direction);
    glm_vec3_normalize(direction);
}

bool rayIntersectsParticle(const vec3 origin, const vec3 direction, const vec3 particlePos, float radius, float *t) {
    vec3 oc;
    glm_vec3_sub(origin, particlePos, oc);

    float a = glm_vec3_dot(direction, direction);
    float b = 2.0f * glm_vec3_dot(oc, direction);
    float c = glm_vec3_dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return false;
    } else {
        *t = (-b - sqrtf(discriminant)) / (2.0f * a);
        return *t > 0;
    }
}

int32_t findIntersectedParticle(const vec3 origin, const vec3 direction, float *closestT) {
    int32_t closest = -1;
    *closestT = FLT_MAX;

    float particleRadius = 0.5f;

    for (int32_t i = 0; i < numParticles; i++) {
        if (particles[i].isFixed) continue;

        float t;
        if (rayIntersectsParticle(origin, direction, particles[i].position, particleRadius, &t)) {
            if (t < *closestT) {
                *closestT = t;
                closest = i;
            }
        }
    }

    return closest;
}

bool rayPlaneIntersection(vec3 rayOrigin, vec3 rayDir, vec3 planePoint, vec3 planeNormal, float *t, vec3 intersection) {
    float denom = glm_vec3_dot(planeNormal, rayDir);

    if (fabsf(denom) < 1e-6) {
        return false;
    }

    vec3 p0l0;
    glm_vec3_sub(planePoint, rayOrigin, p0l0);
    *t = glm_vec3_dot(p0l0, planeNormal) / denom;

    if (*t < 0) {
        return false;  // Intersection is behind the ray origin
    }

    vec3 scaled_dir;
    glm_vec3_scale(rayDir, *t, scaled_dir);
    glm_vec3_add(rayOrigin, scaled_dir, intersection);

    return true;
}


void render(const AppState *app, float dt) {
    static bool updateParticles = false;
    if (updateParticles) {
        initParticles(app);
        simOpts.simulate = false;
        updateParticles = false;
    }
    if (simOpts.simulate) {
        simulateCloth(dt);
    }

#ifndef __EMSCRIPTEN__
    // Cant exit on html
    if (inputIsKeyPressed(GLFW_KEY_ESCAPE)) {
        exit(0);
    }
#endif

    static Camera camera = CAMERA_DEFAULT;
    ImGuiIO *io = igGetIO();
    bool capturedMouse = io->WantCaptureMouse;
    bool capturedKeyboard = io->WantCaptureKeyboard;


    if (!capturedKeyboard) {
        float scl = 10.0f;
        if (inputIsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
            scl = 25.0f;
        }
        vec3 moveDelta = {
            scl * (-dt * inputIsKeyDown(GLFW_KEY_A) + dt * inputIsKeyDown(GLFW_KEY_D)),
            scl * (-dt * inputIsKeyDown(GLFW_KEY_LEFT_CONTROL) + dt * inputIsKeyDown(GLFW_KEY_SPACE)),
            scl * (-dt * inputIsKeyDown(GLFW_KEY_S) + dt * inputIsKeyDown(GLFW_KEY_W)),
        };
        cameraMove(&camera, moveDelta, 1.0f);
    }

    if (!capturedMouse) {
        vec2 mouseDelta;
        inputGetMouseDelta(mouseDelta);
        glm_vec2_scale(mouseDelta, 0.1f, mouseDelta);

        if (inputIsButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            cameraRotate(&camera, -mouseDelta[0], mouseDelta[1], true);
        }
    }

    if (selectedParticle != -1) {
        particles[selectedParticle].isFixed = false;
    }
    if (!capturedMouse && inputIsButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        vec2 mouse;
        inputGetMousePos(mouse);
        int width, height;
        glfwGetWindowSize(app->window, &width, &height);

        float normX = mouse[0] / width;
        float normY = mouse[1] / height;

        castRayFromCamera(&camera, normX, normY, rayOrigin, rayDirection);
        float t;
        selectedParticle = findIntersectedParticle(rayOrigin, rayDirection, &t);
        if (selectedParticle != -1) {
            vec3 hitPoint;
            glm_vec3_scale(rayDirection, t, hitPoint);
            glm_vec3_add(rayOrigin, hitPoint, dragPlanePoint);

            glm_vec3_copy(camera.front, dragPlaneNormal);
            glm_vec3_negate(dragPlaneNormal);

            dragDepth = t;
            glm_vec3_copy(particles[selectedParticle].position, particles[selectedParticle].prevPosition);
        }
    }
    if (!capturedMouse && inputIsButtonReleased(GLFW_MOUSE_BUTTON_RIGHT)) {
        selectedParticle = -1;
    }

    if (selectedParticle != -1) {
        vec2 mouse;
        inputGetMousePos(mouse);
        int width, height;
        glfwGetWindowSize(app->window, &width, &height);

        float normX = mouse[0] / width;
        float normY = mouse[1] / height;

        vec3 currentRayOrigin, currentRayDir;
        castRayFromCamera(&camera, normX, normY, currentRayOrigin, currentRayDir);

        vec3 intersection;
        float t;
        if (rayPlaneIntersection(currentRayOrigin, currentRayDir, dragPlanePoint, dragPlaneNormal, &t, intersection)) {
            glm_vec3_copy(intersection, particles[selectedParticle].position);

            vec3 delta;
            glm_vec3_sub(intersection, particles[selectedParticle].prevPosition, delta);
            glm_vec3_divs(delta, dt, particles[selectedParticle].velocity);
        }
    }

    cameraUpdate(&camera);

    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(app->device, &(WGPUCommandEncoderDescriptor) {
        .nextInChain = NULL,
        .label = "Command Encoder"
    });
    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &(WGPURenderPassDescriptor) {
         .nextInChain = NULL,
         .colorAttachmentCount = 1,
         .colorAttachments = &(WGPURenderPassColorAttachment) {
             .view = app->view,
             .loadOp = WGPULoadOp_Clear,
             .storeOp = WGPUStoreOp_Store,
             .clearValue = {
                 .r = 0.1f,
                 .g = 0.1f,
                 .b = 0.1f,
                 .a = 1.0f
             },
#ifdef __EMSCRIPTEN__
                 .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
#endif
         },
         .depthStencilAttachment = NULL,
         .timestampWrites = NULL,
     });

    wgpuRenderPassEncoderSetPipeline(renderPass, simOpts.drawMesh ? clothMeshPipeline : clothPipeline);
    wgpuRenderPassEncoderSetBindGroup(renderPass, 0, pipelineBindGroup, 0, NULL);
    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer, 0, wgpuBufferGetSize(vertexBuffer));
    wgpuRenderPassEncoderSetIndexBuffer(renderPass, indexBuffer, WGPUIndexFormat_Uint32, 0, wgpuBufferGetSize(indexBuffer));

    Uniform uniform;
    glm_mat4_copy(camera.viewProj, uniform.viewProj);
    wgpuQueueWriteBuffer(queue, uniformBuffer, 0, &uniform, sizeof(uniform));

    for (int32_t i = 0; i < numParticles; i++) {
        ParticleVertex vertex;
        glm_vec3_copy(particles[i].position, vertex.position);
        float restDistance = simOpts.meshSize / particleWidth;
        if (simOpts.visualizeStrain) {
            int x = i % particleWidth;
            int y = i / particleWidth;

            int n = 0;
            float strainSum = 0.0f;
            vec3 tmp;

            if (x + 1 < particleWidth) {
                int rightIdx = i + 1;
                glm_vec3_sub(particles[i].position, particles[rightIdx].position, tmp);
                float actualDist = glm_vec3_norm(tmp);
                float restDist = restDistance;
                float strain = fabsf(actualDist - restDist) / restDist;
                strainSum += strain;
                n++;
            }

            if (x > 0) {
                int leftIdx = i - 1;
                glm_vec3_sub(particles[i].position, particles[leftIdx].position, tmp);
                float actualDist = glm_vec3_norm(tmp);
                float restDist = restDistance;
                float strain = fabsf(actualDist - restDist) / restDist;
                strainSum += strain;
                n++;
            }

            if (y + 1 < particleWidth) {
                int bottomIdx = i + particleWidth;
                glm_vec3_sub(particles[i].position, particles[bottomIdx].position, tmp);
                float actualDist = glm_vec3_norm(tmp);
                float restDist = restDistance;
                float strain = fabsf(actualDist - restDist) / restDist;
                strainSum += strain;
                n++;
            }

            if (y > 0) {
                int topIdx = i - particleWidth;
                glm_vec3_sub(particles[i].position, particles[topIdx].position, tmp);
                float actualDist = glm_vec3_norm(tmp);
                float restDist = restDistance;
                float strain = fabsf(actualDist - restDist) / restDist;
                strainSum += strain;
                n++;
            }
            float avgStrain = (n > 0) ? strainSum / n : 0.0f;

            // Map strain to a color
            // Using a heat map: blue (low strain) -> green -> yellow -> red (high strain)
            if (avgStrain < 0.25f) {
                // Blue to green (0-0.25)
                float t = avgStrain / 0.25f;
                vertex.color[0] = 0.0f;                // R: 0 -> 0
                vertex.color[1] = t;                   // G: 0 -> 1
                vertex.color[2] = 1.0f - 0.5f * t;     // B: 1.0 -> 0.5
            } else if (avgStrain < 0.5f) {
                // Green to yellow (0.25-0.5)
                float t = (avgStrain - 0.25f) / 0.25f;
                vertex.color[0] = t;                   // R: 0 -> 1
                vertex.color[1] = 1.0f;                // G: 1 -> 1
                vertex.color[2] = 0.5f - 0.5f * t;     // B: 0.5 -> 0
            } else {
                // Yellow to red (0.5-1.0+)
                float t = fminf((avgStrain - 0.5f) / 0.5f, 1.0f);
                vertex.color[0] = 1.0f;                // R: 1 -> 1
                vertex.color[1] = 1.0f - t;            // G: 1 -> 0
                vertex.color[2] = 0.0f;                // B: 0 -> 0
            }

            vertex.color[3] = 1.0f;

        } else {
            const vec3 darkColor = {0.3f, 0.3f, 0.4f};  // Dark blue-gray
            const vec3 lightColor = {0.7f, 0.5f, 1.0f};  // Light blue-gray

            float height = particles[i].position[2] / simOpts.meshSize;
            if (height > 1.0) height = 1.0f;
            else if (height < 0.0) height = 0.0f;


            vertex.color[0] = darkColor[0] + height * (lightColor[0] - darkColor[0]);
            vertex.color[1] = darkColor[1] + height * (lightColor[1] - darkColor[1]);
            vertex.color[2] = darkColor[2] + height * (lightColor[2] - darkColor[2]);
            vertex.color[3] = 1.0f;  // Full opacity
        }
        vertices[i] = vertex;
    }
    wgpuQueueWriteBuffer(queue, vertexBuffer, 0, vertices, numParticles * sizeof(*vertices));

    wgpuRenderPassEncoderDrawIndexed(renderPass, gridSize * gridSize * 6, 1, 0, 0, 0);

    igBegin("Cloth Simulation", NULL, 0);
    igText("Use right click to interact.");
    igSpacing();
    if (!simOpts.simulate) {
        igText("Enable Simulate to start simulation");
    } else {
        igText("Simulation enabled");
    }
    igCheckbox("Simulate", &simOpts.simulate);
    igCheckbox("Show mesh", &simOpts.drawMesh);
    igCheckbox("Visualize strain", &simOpts.visualizeStrain);
    igSpacing();
    igSliderInt("Num substeps", &simOpts.numSubsteps, 1, 10, "%d", 0);
    igSliderInt("Solver iterations", &simOpts.solverIterations, 1, 10, "%d", 0);
    igSliderFloat("Damping factor", &simOpts.dampingFactor, 0.9f, 1.0f, "%.3f", 0);
    igSpacing();
    //updateParticles |= igSliderInt("Cloth N", &simOpts.numParticles, 10, MAX_N, "%d", 0);
    updateParticles |= igSliderFloat("Cloth size", &simOpts.meshSize, 10.0f, 200.0f, "%.2f", 0);
    updateParticles |= igSliderFloat("Particle mass", &simOpts.particleMass, 0.1f, 10.0f, "%.2f", 0);
    igSpacing();

    igEnd();


    igRender();
    ImGui_ImplWGPU_RenderDrawData(igGetDrawData(), renderPass);

    wgpuRenderPassEncoderEnd(renderPass);
    wgpuRenderPassEncoderRelease(renderPass);

    // Encode and submit
    WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &(WGPUCommandBufferDescriptor) {
        .nextInChain = NULL,
        .label = "Command Buffer",
    });

    wgpuQueueSubmit(queue, 1, &command);
    wgpuCommandBufferRelease(command);

    wgpuCommandEncoderRelease(encoder);

}

AppConfig appMain() {
     return (AppConfig) {
        .width = 1280,
        .height = 720,
        .title = "Cloth Simulation",
        .init = (AppInitFn) init,
        .deinit = (AppDeInitFn) deinit,
        .render = (AppRenderFn) render,
    };
}
