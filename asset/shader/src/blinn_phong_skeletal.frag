#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform FPCO
{
	layout(offset = 128)
	vec3 cameraPosition; float p0;
	vec3 lightDirection; float p1;
} fpco;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 baseColor = texture(texSampler, inTexCoord).xyz;

	// ambient
	float ambient = 0.05;

	// diffuse
	float diffuse = max(dot(-fpco.lightDirection, inNormal), 0.0);

	// specular
	float shininess = 64.0;
	vec3 lightColor = vec3(0.5);

	vec3 viewDirection = normalize(fpco.cameraPosition - inPosition);
	vec3 reflectDirection = reflect(fpco.lightDirection, inNormal);
	vec3 halfwayDirection = normalize(-fpco.lightDirection + inNormal);
	float specular = pow(max(dot(halfwayDirection, inNormal), 0.0), shininess);

	outColor = vec4(baseColor * ambient + baseColor * lightColor * diffuse + lightColor * specular, 1.0);
}