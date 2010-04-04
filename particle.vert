#version 120

uniform mat4 projection_matrix; // Projection matrix passed in to vertex shader
uniform mat4 modelview_matrix; // Modelview matrix passed in to vertex shader

struct light {
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

uniform light light0; // Input light source 0

attribute vec3 a_Vertex; // Input vertex from current animation frame
attribute vec2 a_TexCoord0; // Input texture coordinate
attribute vec4 a_Color; // Input start color
attribute vec4 b_Color; // Input end color
attribute float lerpValue; // Linear interpolation value

varying vec4 color;
varying vec2 texCoord0; // Output texture coordinate to be interpolated and passed to fragment shader

void main(void)
{
	//Transform vertex into eye space.
	vec4 pos = modelview_matrix * vec4(a_Vertex, 1.0);

	gl_Position = projection_matrix * pos;
	texCoord0 = a_TexCoord0;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	color = mix(a_Color, b_Color, lerpValue);
}
