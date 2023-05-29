#version 330 core

in vec2 v_texcoord0;
in vec2 v_texcoord1;

out vec4 fragColor;

uniform sampler2D u_diffuse_texture;
uniform sampler2D u_lightmap_texture;

void main() {
	vec3 color = texture2D(u_diffuse_texture, v_texcoord0).rgb;
	color *= texture2D(u_lightmap_texture, v_texcoord1).rgb;
	fragColor = vec4( color, 1.0 );
}