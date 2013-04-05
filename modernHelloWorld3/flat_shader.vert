#version 400

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;


out vec3 LightIntensity

struct LightInfo
{
	vec4 Position;
	vec3 La;
	vec3 Ls;

};

uniform LightInfo Light;

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;

};

uniform MaterialInfo Material;

uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;
uniform mat4 MVP;


void main()
{
	vec3 tnorm = normalize(normal_matrix * vertex_matrix);
	vec4 eyeCoords = modelview_matrix * vec4(vertex_position, 1.0);
	vec3 s = normalize(vec3(Light.Postion - eyeCoords));
	vec3 v = normalize(-eyeCoords.xyz);
	vec3 r = reflect(-s, tnorm);
	vec3 ambient = Light.La * Material.Ka;
	float sDotN = max(dot(s, tnorm), 0.0);
	vec3 spec = vec(0.0);
	if(sDotN > 0.0)
		spec = Light.Ls * Material.Ks * pow( max(dot(r,v), 0.0), Material.Shininess);
	LightIntensity = ambient + diffuse + spec;
	gl_Position = MVP * vec4(vertex_position, 1.0);

}

