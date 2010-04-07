#version 120

uniform sampler2D texture0;

struct light {
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

uniform light light0;

varying vec4 color; // Input color
varying vec2 texCoord0; // Input tex coord

void main(void)
{		
	//vec4 texColor = texture2D(texture0, texCoord0.st);
	vec4 texColor = texture2D(texture0, gl_TexCoord[0].st);
	if (texColor.a < 0.1 || color.a < 0.1)
		discard;
		
	vec4 final_color = color;
	
	//Add ambient component.
	final_color += light0.ambient;
	gl_FragColor = final_color * texColor; // Copy color to output
}
