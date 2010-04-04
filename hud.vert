#version 120

uniform mat4 projection_matrix; // Projection matrix passed in to vertex shader
uniform mat4 modelview_matrix; // Modelview matrix passed in to vertex shader

attribute vec3 a_Vertex; // Input vertex
attribute vec2 a_TexCoord0; // Input texture coordinate

varying vec2 texCoord0; // Output texture coordinate to be interpolated and passed to fragment shader

void main(void)
{
	//Transform vertex into eye space.
	vec4 pos = modelview_matrix * vec4(a_Vertex, 1.0);
	
	gl_Position = projection_matrix * pos;
	texCoord0 = a_TexCoord0;	
}
