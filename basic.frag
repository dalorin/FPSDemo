#version 120

varying vec4 color; // Interpolated color passed in from vertex shader

void main(void)
{
	gl_FragColor = color; // Copy color to output
}
