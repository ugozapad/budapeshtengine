#version 330 core

in vec2 v_texcoord0;
in vec2 v_texcoord1;

out vec4 fragColor;

uniform sampler2D u_diffuseTexture;
uniform sampler2D u_lightmapTexture;

void main() {
	vec3 color = texture2D(u_diffuseTexture, v_texcoord0);
	color *= texture2D(u_lightmapTexture, v_texcoord1);
	return color;
}