#version 120

uniform mat4 projection_matrix; // Projection matrix passed in to vertex shader
uniform mat4 modelview_matrix; // Modelview matrix passed in to vertex shader

attribute vec3 a_Vertex; // Input vertex
attribute vec4 a_Color; // Input color
varying vec4 color; // Output color to be interpolated and passed to fragment shader

void main(void)
{
	vec4 pos = modelview_matrix * vec4(a_Vertex, 1.0);
	gl_Position = projection_matrix * pos;
	color = a_Color;
}
