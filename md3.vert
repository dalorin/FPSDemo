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
uniform float lerp_value;

attribute vec3 a_Vertex; // Input vertex from current animation frame
attribute vec3 b_Vertex; // Input vertex from next animation frame
attribute vec2 a_TexCoord0; // Input texture coordinate
attribute vec3 a_Normal; // Input vertex normal

varying vec2 texCoord0; // Output texture coordinate to be interpolated and passed to fragment shader
varying vec3 normal; // Output normal
varying vec3 light_pos; // Output light position
varying vec3 half_vector; // Output half vector
varying vec4 vertex_ambient; // Vertex ambient light value
varying vec4 vertex_diffuse; // Vertex diffuse light value
varying float blend_factor;

void main(void)
{
	//Interpolate vertex coords.
	vec3 lerped_vertex = mix(a_Vertex, b_Vertex, lerp_value);

	//Calculate normal matrix and transform normal into eye space.
	mat3x3 normal_matrix = mat3x3(modelview_matrix);
	normal = normalize(normal_matrix * a_Normal);
	
	//Transform light position into eye space.
	light_pos = normalize(modelview_matrix * light0.position).xyz;
	
	//Calculate ambient colour.
	vertex_ambient = material_ambient * light0.ambient;
	
	//Calculate diffuse colour.
	vertex_diffuse = material_diffuse * light0.diffuse;
	
	//Transform vertex into eye space.
	vec4 pos = modelview_matrix * vec4(lerped_vertex, 1.0);
	
	//Calculate eye vector.
	vec3 E = -pos.xyz;
	
	//Calculate half vector.
	half_vector = normalize(light_pos + E);
		
	//blendFactor = exp2(-fogDensity * length(pos));
	gl_Position = projection_matrix * pos;
	texCoord0 = a_TexCoord0;	
}
