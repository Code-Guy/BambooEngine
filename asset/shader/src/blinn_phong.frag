#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstantsObject
{
	mat4 mvp;
	vec4 cameraPosition;
	vec4 lightDirection;
} pco;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

void main()
{
	//outColor = texture(texSampler, inTexCoord);
	outColor = vec4(inNormal, 1.0);
}