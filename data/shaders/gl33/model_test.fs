#version 330 core

in vec4 v_normal;
in vec2 v_texcoord;

out vec4 fragColor;

uniform sampler2D u_diffuse_texture;

void main() {
	vec3 color = texture(u_diffuse_texture, v_texcoord).rgb;
	fragColor = vec4( color, 1.0 );
}