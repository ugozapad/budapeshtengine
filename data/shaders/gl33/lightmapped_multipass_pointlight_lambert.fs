#version 330 core

in vec2 v_texcoord0;
in vec2 v_texcoord1;

out vec4 fragColor;

uniform sampler2D u_diffuseTexture;
uniform sampler2D u_lightmapTexture;

// point light stuff
uniform vec4 u_color;
uniform vec3 u_position;
uniform float u_radius;
uniform float u_constAttenuation;
uniform float u_linearAttenuation;
uniform float u_quadraticAttenuation;
uniform float u_reserved;

vec3 calculatePointLambert() {
	return vec3(1.0);
}

void main() {
	vec3 color = texture2D(u_diffuseTexture, v_texcoord0);
	color *= calculatePointLambert();
	return color;
}