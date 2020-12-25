#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject
{
	mat4 mvp;
} ubo;

layout(push_constant) uniform PushConstantsObject
{
	mat4 mvp;
	vec4 cameraPosition;
	vec4 lightDirection;
} pco;

// dvec会使用2个slot
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outPosition;

void main()
{
	gl_Position = pco.mvp * vec4(inPosition, 1.0);
	
	outTexCoord = inTexCoord;
	outNormal = inNormal;
}