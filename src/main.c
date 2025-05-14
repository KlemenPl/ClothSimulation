#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "app.h"
#include "camera.h"

#include <stb_ds.h>
#include <stb_image.h>

WGPUQueue queue;


int init(const AppState *app, int argc, const char **argv) {
    queue = wgpuDeviceGetQueue(app->device);

    return 0;
}
void deinit(const AppState *app) {
    wgpuQueueRelease(queue);
}

void render(const AppState *app, float dt) {
#ifndef __EMSCRIPTEN__
    // Cant exit on html
    if (inputIsKeyPressed(GLFW_KEY_ESCAPE)) {
        exit(0);
    }
#endif

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
