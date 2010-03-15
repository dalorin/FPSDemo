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
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform vec4 material_emissive;
uniform float material_shininess;
uniform float fogDensity;

attribute vec3 a_Vertex; // Input vertex
attribute vec4 a_Color; // Input color
attribute vec2 a_TexCoord0; // Input texture coordinate
attribute vec3 a_Normal; // Input vertex normal

varying vec4 color; // Output color to be interpolated and passed to fragment shader
varying vec2 texCoord0; // Output texture coordinate to be interpolated and passed to fragment shader
varying float blendFactor;

void main(void)
{
	//Calculate normal matrix and transform normal into eye space.
	mat3x3 normal_matrix = mat3x3(modelview_matrix);
	vec3 N = normalize(normal_matrix * a_Normal);
	
	//Transform light position into eye space.
	vec3 L = normalize(modelview_matrix * light0.position).xyz;
	
	//Calculate angle between normal and light direction.
	float NdotL = max(dot(N,L), 0.0);
	
	//Begin colour calculation by starting with ambient colour.
	vec4 finalColor = material_ambient * light0.ambient;
	
	//Transform vertex into eye space.
	vec4 pos = modelview_matrix * vec4(a_Vertex, 1.0);
	
	//Calculate eye vector.
	vec3 E = -(pos.xyz);
	
	//Only perform lighting calculations if the surface is facing toward the light source.
	if (NdotL > 0.0)
	{
		finalColor += material_diffuse * light0.diffuse * NdotL;
		
		//Calculate and normalize half vector.
		vec3 HV = normalize(L + E);
		
		//Calculate angle between normal and HV.
		float NdotHV = max(dot(N, HV), 0.0);
		
		//Calculate specular component.
		finalColor += material_specular * light0.specular * pow(NdotHV, material_shininess);
	}
	
	//blendFactor = exp2(-fogDensity * length(pos));
	gl_Position = projection_matrix * pos;
	texCoord0 = a_TexCoord0;
	color = finalColor;
}
