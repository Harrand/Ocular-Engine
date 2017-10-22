// Vertex Shader version 4.30
#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 5) in vec3 positions_instance;

out vec3 vs_position_modelspace;
out vec2 vs_texcoord_modelspace;
out vec3 vs_normal_modelspace;

out mat4 vs_model_matrix;
out mat4 vs_view_matrix;
out mat4 vs_projection_matrix;
out mat3 vs_tbn_matrix;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform float displacement_factor;
uniform bool is_instanced;

uniform sampler2D displacement_map_sampler;

void share()
{
	vs_position_modelspace = position;
	vs_texcoord_modelspace = texcoord;
	vs_normal_modelspace = normal;
	vs_position_modelspace += normal * texture2D(displacement_map_sampler, vs_texcoord_modelspace).r * displacement_factor;
	
	vs_model_matrix = m;
	vs_view_matrix = v;
	vs_projection_matrix = p;
	
	vec3 normal_cameraspace = normalize((v * m * vec4(vs_normal_modelspace, 0.0)).xyz);
	vec3 tangent_cameraspace = normalize((m * vec4(tangent, 0.0)).xyz);
	
	// Gramm-Schmidt Process
	tangent_cameraspace = normalize(tangent_cameraspace - dot(tangent_cameraspace, normal_cameraspace) * normal_cameraspace);
	
	vec3 bitangent_cameraspace = cross(tangent_cameraspace, normal_cameraspace);
	
	vs_tbn_matrix = transpose(mat3(tangent_cameraspace, bitangent_cameraspace, normal_cameraspace));
}

void main()
{
	share();
	if(is_instanced)
		gl_Position = p * v * ((m * vec4(vs_position_modelspace, 1.0)) + vec4(positions_instance, 0));
	else
		gl_Position = (p * v * m) * vec4(vs_position_modelspace, 1.0);
}