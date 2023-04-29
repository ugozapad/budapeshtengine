#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord0;
layout(location = 2) in vec2 texcoord1;

out vec2 v_texcoord0;
out vec2 v_texcoord1;

uniform mat4 u_model_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_proj_matrix;
uniform mat4 u_model_view_projection;

void main() {
	gl_Position = u_model_view_projection * vec4(position.xyz, 1.0);
	v_texcoord0 = texcoord0;
	v_texcoord1 = texcoord1;
}