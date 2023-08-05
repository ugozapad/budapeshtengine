#version 330 core

in vec3 v_color;

out vec4 fragColor;
uniform sampler2D u_diffuse_texture;
uniform sampler2D u_lightmap_texture;
void main()
{
	fragColor = vec4(v_color, 1.0);   
}