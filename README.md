# Position-based Dynamics

Position-based Dynamics using WebGPU and C11

## Demo

Try out an interactive version here [demo](https://klemenpl.github.io/ClothSimulation/): 

![](cloth_1.png)
![](cloth_2.png)

Note: WebGPU is still experimental and may not work on all browsers and platforms.

Reportedly it should work out of the box on Windows.

On Linux I had to enable following Chromium flags:
- Vulkan
- Use Skia Renderer
- Unsafe WebGPU Support
- WebGPU Developer Features

## Building

Prerequisites:
- cmake
- make
- C compiler (that supports C11)
- docker (if you are building for web)

```bash
make desktop # For desktop

# Assets need to be relative of the executable (./assets/...)

make web # For html5
```

You may need to install some additional libraries (cmake will notify you).