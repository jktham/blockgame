#version 460 core

struct Light {
    vec3 direction;
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 frag_color;

in vec3 frag_pos;
in vec2 tex_coord;
in vec3 normal;

uniform Light light;
uniform vec3 view_pos;
uniform sampler2D dirt_texture;
uniform sampler2D grass_texture;

void main()
{
    if (normal.z > 0.0f)
    {
        frag_color = texture(grass_texture, tex_coord);
    }
    else
    {
        frag_color = texture(dirt_texture, tex_coord);
    }

    //vec3 light_dir = normalize(light.position - frag_pos);
    vec3 light_dir = normalize(light.direction);
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, normalize(normal));

    vec3 ambient = light.color * light.ambient;
    vec3 diffuse = light.color * light.diffuse * max(dot(normalize(normal), light_dir), 0.0);
    vec3 specular = light.color * light.specular * pow(max(dot(view_dir, reflect_dir), 0.0), 16);

    vec3 lighting = ambient + diffuse + specular;

    frag_color *= vec4(lighting, 1.0f);
}