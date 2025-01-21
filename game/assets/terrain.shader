#ifdef GL_ES
#extension GL_OES_shader_io_blocks : enable
precision mediump float;
#endif

struct VertexOutput
{
    vec3 position;
    vec3 normal;
    vec3 tangent;
    vec4 color;
    vec4 light;
};

#ifdef VERTEX
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_tangent;

layout (std140) uniform ConstantBuffer
{
    mat4 ViewProjMtx;
};

layout (std140) uniform DrawBuffer
{
    vec4 color;
    vec4 light;
};

out VertexOutput io;

void main()
{
    vec4 WorldPosition = vec4(v_position, 1.0);
    gl_Position = ViewProjMtx * WorldPosition;

    io.position = WorldPosition.xyz;
    io.normal = v_normal;
    io.tangent = v_tangent;
    io.color = color;
    io.light = light;
}
#endif

#ifdef PIXEL
layout (location = 0) out vec4 p_color;

in VertexOutput io;

uniform sampler2D Albedo;

void main()
{
    vec3 N = normalize(io.normal);
    vec3 Ld = normalize(-io.light.xyz);
    float Li = io.light.w;

    float diffuse = Li * max(0.0, dot(N, Ld));

    vec2 uv = io.position.xz * 0.25;
    vec4 albedo = texture(Albedo, uv);

    vec3 color = io.color.rgb * albedo.rgb * diffuse;
    float alpha = io.color.a * albedo.a;
    p_color = vec4(color, alpha);

    //p_color = vec4(1);
    //p_color = vec4(uv, 0, 1);
    //p_color = vec4(N, 1);
    //p_color = vec4(N * 0.5 + 0.5, 1);
    //p_color = vec4(vec3(diffuse), 1);
    //p_color = albedo;
    //p_color = io.color;
}
#endif