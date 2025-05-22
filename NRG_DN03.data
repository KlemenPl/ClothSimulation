struct Vertex {
    @location(0) position: vec3f,
    @location(1) color: vec4f,
}

struct VertexOut {
    @builtin(position) position: vec4f,
    @location(0) color: vec4f,
}

struct Uniforms {
    viewProj: mat4x4<f32>,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

@vertex
fn vs_main(in: Vertex) -> VertexOut {
    var out: VertexOut;
    out.position = uniforms.viewProj * vec4f(in.position, 1.0f);
    out.color = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOut) -> @location(0) vec4f {
    return in.color;
}