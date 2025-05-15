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
    int32_t numParticles;
    float meshSize;
    float particleMass;

    int32_t numSubsteps;
    int32_t solverIterations;

} simOpts = {
    .drawMesh = false,
    .simulate = false,
    .numParticles = 100,
    .meshSize = 50.0f,
    .particleMass = 1.0f,
    .numSubsteps = 3,
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
    float spacing = (float) simOpts.meshSize / (n - 1.0f);
    gridSize = n;
    numParticles = (gridSize + 1) * (gridSize + 1);
    particleWidth = gridSize + 1;
    numConstraints = numParticles * 4;

    int32_t constraintIdx = 0;

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


        }
    }
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
    constraints = malloc(sizeof(*constraints) * maxParticles * 4);
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

void solveDistanceConstraint(int32_t cIdx) {
    DistanceConstraint *constraint = constraints + cIdx;

    Particle *p1 = particles + constraint->p1;
    Particle *p2 = particles + constraint->p2;

    if (p1->isFixed && p2->isFixed)
        return;

    vec3 delta;
    glm_vec3_sub(p1->position, p2->position, delta);
    float length = glm_vec3_norm(delta);
    if (length < 1e-5) return;

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

    static vec3 gravity = {0.0f, -9.81f, 0.0f};

    for (int32_t substep = 0; substep < simOpts.numSubsteps; substep++) {
        for (int32_t i = 0; i < numParticles; i++) {
            glm_vec3_copy(particles[i].position, particles[i].prevPosition);
        }

        for (int32_t i = 0; i < numParticles; i++) {
            if (particles[i].isFixed) continue;
            glm_vec3_muladds(gravity, subDt, particles[i].velocity);
            glm_vec3_muladds(particles[i].velocity, subDt, particles[i].position);
        }

        for (int32_t iter = 0; iter < simOpts.solverIterations; iter++) {
            for (int32_t i = 0; i < numConstraints; i++) {
                solveDistanceConstraint(i);
            }
        }

        for (int32_t i = 0; i < numParticles; i++) {
            if (particles[i].isFixed) continue;
            glm_vec3_sub(particles[i].position, particles[i].prevPosition, particles[i].velocity);
            glm_vec3_divs(particles[i].velocity, subDt, particles[i].velocity);
        }

    }

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
        vertex.color[0] = 1;
        vertex.color[1] = 1;
        vertex.color[2] = 1;
        vertex.color[3] = 1;
        vertices[i] = vertex;
    }
    wgpuQueueWriteBuffer(queue, vertexBuffer, 0, vertices, numParticles * sizeof(*vertices));

    wgpuRenderPassEncoderDrawIndexed(renderPass, gridSize * gridSize * 6, 1, 0, 0, 0);

    igBegin("Cloth Simulation", NULL, 0);
    if (!simOpts.simulate) {
        igText("Enable Simulate to start simulation");
    } else {
        igText("Simulation enabled");
    }
    igCheckbox("Simulate", &simOpts.simulate);
    igCheckbox("Show mesh", &simOpts.drawMesh);
    igSpacing();
    igSliderInt("Num substeps", &simOpts.numSubsteps, 1, 10, "%d", 0);
    igSliderInt("Solver iterations", &simOpts.solverIterations, 1, 10, "%d", 0);
    igSpacing();
    updateParticles |= igSliderInt("Cloth N", &simOpts.numParticles, 10, MAX_N, "%d", 0);
    updateParticles |= igSliderFloat("Cloth size", &simOpts.meshSize, 10.0f, 200.0f, "%.2f", 0);
    updateParticles |= igSliderFloat("Particle mass", &simOpts.particleMass, 0.1f, 10.0f, "%.2f", 0);
    igSpacing();

    igText("====== Stats ======");
    igText("dt: %.2f", dt);

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
