#version 120

uniform sampler2D texture0;

struct light {
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

uniform light light0;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec4 material_emissive;
uniform float material_shininess;

varying vec4 color; // Input color
varying vec2 texCoord0; // Input tex coord
varying vec3 normal; // Input normal
varying vec3 light_pos; // Input light position
varying vec3 half_vector; // Input half vector
varying vec4 vertex_ambient; // Vertex ambient light value
varying vec4 vertex_diffuse; // Vertex diffuse light value
varying float blend_factor;

void main(void)
{		
	vec4 texColor = texture2D(texture0, texCoord0.st);
	if (texColor.a < 0.1)
		discard;
		
	// Renormalise normal.
	vec3 N = normalize(normal);
	
	// Renormalise light position.
	vec3 L = normalize(light_pos);
		
	//Calculate angle between normal and light direction.
	float NdotL = max(dot(N,L), 0.0);
	
	vec4 final_color = color;
	
	//Only perform lighting calculations if the surface is facing toward the light source.
	if (NdotL > 0.0)
	{
		// Add diffuse contribution.
		final_color += vertex_diffuse * NdotL;
		
		//Calculate and normalize half vector.
		vec3 HV = normalize(half_vector);
		
		//Calculate angle between normal and HV.
		float NdotHV = max(dot(N, HV), 0.0);
		
		//Calculate specular component.
		final_color += material_specular * light0.specular * pow(NdotHV, material_shininess);		
	}
		
	//Add ambient component.
	final_color += vertex_ambient;
	gl_FragColor = final_color * texColor; // Copy color to output
	//vec4 fogColor = vec4(0.5, 0.5, 0.5, 0.5);
	//gl_FragColor = mix(fogColor, fragColor, blend_factor);
}
