#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texcoord;

out vec4 v_normal;
out vec2 v_texcoord;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_proj_matrix;
uniform mat4 u_model_view_projection;

void main() {
	gl_Position = u_model_view_projection * vec4(position.xyz, 1.0);
	v_normal = normal;
	v_texcoord = texcoord;
}