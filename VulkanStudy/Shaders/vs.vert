#version 450

layout(location = 0) in vec3 inPos;

layout (set = 0, binding = 0) uniform UBO
{
	mat4 MVP1;
} ubo;

void main()
{
	gl_Position = ubo.MVP1 * (vec4(inPos, 1.0) + vec4(6.0f, 7.7f, 0.0f, 1.0f));
}