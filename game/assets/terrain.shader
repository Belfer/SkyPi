#ifdef GL_ES
#extension GL_OES_shader_io_blocks : enable
precision mediump float;
#endif

#ifdef VERTEX
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;

layout (std140) uniform ConstantBuffer
{
    mat4 ViewProjMtx;
};

layout (std140) uniform DrawBuffer
{
    vec4 Color;
    vec4 Light;
};

out VertexOutput
{
    vec3 Position;
    vec3 Normal;
    vec3 Tangent;
    vec4 Color;
    vec4 Light;
} FragData;

void main()
{
    vec4 WorldPosition = vec4(Position, 1.0);
    gl_Position = ViewProjMtx * WorldPosition;

    FragData.Position = WorldPosition.xyz;
    FragData.Normal = Normal;
    FragData.Tangent = Tangent;
    FragData.Color = Color;
    FragData.Light = Light;
}
#endif

#ifdef PIXEL
layout (location = 0) out vec4 Out_Color;

in VertexOutput
{
    vec3 Position;
    vec3 Normal;
    vec3 Tangent;
    vec4 Color;
    vec4 Light;
} FragData;

uniform sampler2D Albedo;

void main()
{
    float diffuse = FragData.Light.w * max(0.0, dot(FragData.Normal, -FragData.Light.xyz));
    vec4 albedo = texture(Albedo, FragData.Position.xz * 0.05);

    vec3 color = FragData.Color.rgb * albedo.rgb * diffuse;
    float alpha = FragData.Color.a * albedo.a;
    Out_Color = vec4(color, alpha);

    //Out_Color = vec4(1);
    //Out_Color = texture(Albedo, FragData.Position.xz);
    //Out_Color = vec4(FragData.UV, 0, 1);
    //Out_Color = vec4(FragData.Normal * 0.5 + 0.5, 1);
}
#endif