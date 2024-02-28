#version 450

layout(location = 1) in vec3 vPos;

layout (set = 0, binding = 0) uniform UBO
{
	mat4 MVP1;
} ubo;

void main()
{
	gl_Position = ubo.MVP1 * vec4(vPos, 1.0f);
}