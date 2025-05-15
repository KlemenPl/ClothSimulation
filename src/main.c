#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "app.h"
#include "camera.h"


typedef struct Particle {
    vec3 position;
    vec3 velocity;
    float mass;
    bool isFixed;
} Particle;

typedef struct DistanceConstraint {
    int32_t p1;
    int32_t p2;
    float restLen;
} DistanceConstraint;

typedef struct ParticleVertex {
    vec3 position;
    vec4 color;
} ParticleVertex;

typedef struct Uniform {
    mat4 viewProj;
} Uniform;

WGPUQueue queue;
WGPUBindGroup pipelineBindGroup;
WGPURenderPipeline pipeline;

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

void initParticles(const AppState *app, int32_t n, float spacing) {
    gridSize = n;
    free(particles);
    free(constraints);
    free(vertices);

    numParticles = (gridSize + 1) * (gridSize + 1);
    particleWidth = gridSize + 1;
    numConstraints = numParticles * 4;

    particles = malloc(sizeof(*particles) * numParticles);
    constraints = malloc(sizeof(*constraints) * numConstraints);

    int32_t constraintIdx = 0;

    for (int32_t y = 0; y < particleWidth; y++) {
        for (int32_t x = 0; x < particleWidth; x++) {
            int32_t idx = y * particleWidth + x;
            particles[idx] = (Particle){
                .position = {(float) x * spacing, 0, (float) y * spacing},
                .velocity = {0, 0, 0},
                .mass = 1.0f,
                .isFixed = false,
            };

            if (x > 0) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx - 1,
                    .restLen = spacing,
                };
            }
            if (x < particleWidth - 1) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx + 1,
                    .restLen = spacing,
                };
            }
            if (y > 0) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx - 1,
                    .restLen = spacing,
                };
            }
            if (y < particleWidth - 1) {
                constraints[constraintIdx++] = (DistanceConstraint) {
                    .p1 = idx,
                    .p2 = idx + 1,
                    .restLen = spacing,
                };
            }


        }
    }
    particles[0].isFixed = true;
    particles[particleWidth - 1].isFixed = true;

    if (vertexBuffer)
        wgpuBufferRelease(vertexBuffer);
    if (indexBuffer)
        wgpuBufferRelease(indexBuffer);

    vertexBuffer = wgpuDeviceCreateBuffer(app->device, &(WGPUBufferDescriptor){
        .label = "Vertex Buffer",
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        .size = numParticles * sizeof(ParticleVertex),
    });
    vertices = malloc(sizeof(*vertices) * numParticles);
    int32_t numIndices = gridSize * gridSize * 6;
    indexBuffer = wgpuDeviceCreateBuffer(app->device, &(WGPUBufferDescriptor){
        .label = "Index Buffer",
        .usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index,
        .size = numIndices * sizeof(uint32_t),
    });

    uint32_t *indices = malloc(sizeof(*indices) * numIndices);
    int32_t offset = 0;
    for (int32_t y = 1; y < gridSize; y++) {
        for (int32_t x = 1; x < gridSize; x++) {
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
    free(indices);
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


    pipeline = wgpuDeviceCreateRenderPipeline(app->device, &(WGPURenderPipelineDescriptor){
        .layout = pipelineLayout,
        // TODO: Able to switchero
        .primitive.topology = WGPUPrimitiveTopology_TriangleList,
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
    });

    wgpuShaderModuleRelease(shaderModule);
    wgpuPipelineLayoutRelease(pipelineLayout);
    initParticles(app, 100, 1.0f);

    return 0;
}
void deinit(const AppState *app) {
    wgpuRenderPipelineRelease(pipeline);
    wgpuBindGroupRelease(pipelineBindGroup);

    wgpuBufferRelease(uniformBuffer);
    wgpuBufferRelease(vertexBuffer);
    wgpuBufferRelease(indexBuffer);

    wgpuQueueRelease(queue);

    free(vertices);
    free(particles);
    free(constraints);
}

void render(const AppState *app, float dt) {
#ifndef __EMSCRIPTEN__
    // Cant exit on html
    if (inputIsKeyPressed(GLFW_KEY_ESCAPE)) {
        exit(0);
    }
#endif

    static OrbitCamera camera = ORBIT_CAMERA_DEFAULT;
    ImGuiIO *io = igGetIO();
    bool capturedMouse = io->WantCaptureMouse;
    bool capturedKeyboard = io->WantCaptureKeyboard;


    if (!capturedKeyboard) {
        float scl = 10.0f;
        if (inputIsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
            scl = 250.0f;
        }
        vec2 moveDelta = {
            scl * (-dt * inputIsKeyDown(GLFW_KEY_A) + dt * inputIsKeyDown(GLFW_KEY_D)),
            scl * (-dt * inputIsKeyDown(GLFW_KEY_S) + dt * inputIsKeyDown(GLFW_KEY_W))
        };
        orbitCameraPan(&camera, moveDelta[0], moveDelta[1]);
    }

    if (!capturedMouse) {
        vec2 mouseDelta;
        inputGetMouseDelta(mouseDelta);
        glm_vec2_scale(mouseDelta, 10.0f * dt, mouseDelta);
        if (inputIsButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            orbitCameraPan(&camera, -mouseDelta[0], mouseDelta[1]);
        }
        if (inputIsButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            orbitCameraRotate(&camera, -mouseDelta[0], -mouseDelta[1]);
        }

        vec2 wheelDelta;
        inputGetMouseWheelDelta(wheelDelta);
        orbitCameraZoom(&camera, wheelDelta[1]);
    }

    orbitCameraUpdate(&camera);

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

    wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);
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

    igBegin("NRG DN03", NULL, 0);
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
        .title = "NRG_DN03",
        .init = (AppInitFn) init,
        .deinit = (AppDeInitFn) deinit,
        .render = (AppRenderFn) render,
    };
}
