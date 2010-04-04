#version 120

uniform sampler2D texture0;

varying vec2 texCoord0; // Input tex coord

void main(void)
{		
	vec4 texColor = texture2D(texture0, texCoord0.st);
	if (texColor.a < 0.1)
		discard;
		
	gl_FragColor = texColor; // Copy color to output	
}
