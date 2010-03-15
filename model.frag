#version 120

uniform sampler2D texture0;

varying vec4 color; // Interpolated color passed in from vertex shader
varying vec2 texCoord0;
varying float blendFactor;

void main(void)
{		
	vec4 texColor = texture2D(texture0, texCoord0.st);
	if (texColor.a < 0.1)
		discard;
	gl_FragColor = color * texColor; // Copy color to output
	//vec4 fogColor = vec4(0.5, 0.5, 0.5, 0.5);
	//gl_FragColor = mix(fogColor, fragColor, blendFactor);
}
