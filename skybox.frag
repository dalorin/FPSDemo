#version 120

uniform sampler2D texture0;

struct light {
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

uniform light light0;

//varying vec4 color; // Input color
varying vec2 texCoord0; // Input tex coord

void main(void)
{		
	vec4 texColor = texture2D(texture0, texCoord0.st);
	if (texColor.a < 0.1)
		discard;
		
	gl_FragColor = light0.ambient * texColor; // Copy color to output
	//vec4 fogColor = vec4(0.5, 0.5, 0.5, 0.5);
	//gl_FragColor = mix(fogColor, fragColor, blend_factor);
}
