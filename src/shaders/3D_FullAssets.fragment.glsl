// Fragment Shader version 4.30
#version 430

in vec3 position_modelspace;
in vec2 texcoord_modelspace;
in vec3 eye_direction_cameraspace;
in vec3 light_direction_cameraspace;

in mat4 model_matrix;
in mat4 view_matrix;
in mat3 tbn_matrix;

uniform sampler2D texture_sampler;
uniform sampler2D normal_map_sampler;
uniform sampler2D parallax_map_sampler;

uniform uint shininess;
uniform float parallax_multiplier;
uniform float parallax_bias;

layout(location = 0) out vec4 fragment_colour;

// This shader works primarily in camera-space.

struct DirectionalLight
{
    // Direction is in cameraspace.
    vec3 direction;
    vec3 colour;
    float power;
};

struct PointLight
{
    // Position is in cameraspace.
    vec3 position;
    vec3 colour;
    float power;
};

vec3 diffuse_directional(DirectionalLight light, vec3 diffuse_colour, vec3 normal_cameraspace)
{
    float cos_theta = clamp(dot(normal_cameraspace, light.direction), 0.0, 1.0);
    return diffuse_colour * light.colour * light.power * cos_theta;
}

vec3 diffuse(PointLight light, vec3 diffuse_colour, vec3 normal_cameraspace, vec3 position_cameraspace)
{
    float distance = length(light.position - position_cameraspace);
    DirectionalLight directional;
    directional.direction = light.position + eye_direction_cameraspace;
    directional.colour = light.colour;
    directional.power = light.power;
    return diffuse_directional(directional, diffuse_colour, normal_cameraspace) / pow(distance, 2);
}

vec3 specular_directional(DirectionalLight light, vec3 specular_colour, vec3 normal_cameraspace)
{
    /* //phong specular
    vec3 reflection = reflect(-light.direction, normal_cameraspace);
    float cos_alpha = clamp(dot(eye_direction_cameraspace, reflection), 0.0, 1.0);
    return specular_colour * light.colour * light.power * pow(cos_alpha, 5);
    */

    //blinn-phong specular
    const float shininess = 5;
    vec3 halfway_direction = normalize(light.direction + eye_direction_cameraspace);
    float specular_component = pow(max(dot(normal_cameraspace, halfway_direction), 0.0), shininess);
    return specular_colour * light.colour * light.power * specular_component;
}

vec3 specular(PointLight light, vec3 specular_colour, vec3 normal_cameraspace, vec3 position_cameraspace)
{
    float distance = length(light.position - position_cameraspace);
    DirectionalLight directional;
    directional.direction = light.position + eye_direction_cameraspace;
    directional.colour = light.colour;
    directional.power = light.power;
    return specular_directional(directional, specular_colour, normal_cameraspace) / pow(distance, 2);
}

void main()
{
    // TBN matrix goes from cameraspace to tangentspace
    vec3 position_cameraspace = (view_matrix * model_matrix * vec4(position_modelspace, 1.0)).xyz;
	vec3 normal_cameraspace = transpose(tbn_matrix) * (texture(normal_map_sampler, texcoord_modelspace).xyz * 255.0/128.0 - 1);
	vec3 texture_colour = texture(texture_sampler, texcoord_modelspace).xyz;
	 // Directional Component.
    DirectionalLight cam_light;
    cam_light.colour = vec3(1, 1, 1);
    cam_light.direction = vec3(0, 0, 0) + eye_direction_cameraspace;
    cam_light.power = 0.5f;
    fragment_colour = vec4(diffuse_directional(cam_light, texture_colour, normal_cameraspace) + specular_directional(cam_light, texture_colour, normal_cameraspace), 1.0);
    /*
    // Non-Directional Component.
    PointLight cam_light;
    cam_light.colour = vec3(1, 1, 1);
    cam_light.position = vec3(0, 0, 0);
    cam_light.power = 20.0f;
    fragment_colour = vec4(diffuse(cam_light, texture_colour, normal_cameraspace, position_cameraspace) + specular(cam_light, texture_colour, normal_cameraspace, position_cameraspace), 1.0);
    */
}