#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord0;
layout(location = 2) in vec2 texcoord1;

out vec2 v_texcoord0;
out vec2 v_texcoord1;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewProjection;

void main() {
	gl_Position = u_modelViewProjection * vec4(position.xyz, 1.0);
	v_texcoord0 = texcoord0;
	v_texcoord1 = texcoord1;
}