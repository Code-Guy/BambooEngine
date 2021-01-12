#version 450
#extension GL_ARB_separate_shader_objects : enable

const int INVALID_BONE = -1;
const int MAX_BONE_NUM = 100;
const int BONE_NUM_PER_VERTEX = 4;
layout(binding = 0) uniform UBO
{
	mat4 gBones[MAX_BONE_NUM];
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
	mat4 boneTransform = mat4(0.0);
	for (int i = 0; i < BONE_NUM_PER_VERTEX; ++i)
	{
		boneTransform += ubo.gBones[inBones[i]] * inWeights[i];
	}

	vec4 localPosition = boneTransform * vec4(inPosition, 1.0);
	vec4 localNormal = boneTransform * vec4(inNormal, 0.0);

	gl_Position = vpco.mvp * localPosition;
	
	outTexCoord = inTexCoord;
	outNormal = (vpco.m * localNormal).xyz;
	outPosition = (vpco.m * localPosition).xyz;
}