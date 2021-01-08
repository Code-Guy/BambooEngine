#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UBO
{
	mat4 mvp;
} ubo;

layout(push_constant) uniform VPCO
{
	mat4 m;
	mat4 mvp;
} vpco;

// dvec会使用2个slot
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in ivec4 inBones;
layout(location = 4) in vec4 inWeights;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outPosition;

void main()
{
	gl_Position = vpco.mvp * vec4(inPosition, 1.0);
	
	outTexCoord = inTexCoord;
	outNormal = (vpco.m * vec4(inNormal, 0.0)).xyz;
	outPosition = (vpco.m * vec4(inPosition, 1.0)).xyz;
}