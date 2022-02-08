#version 460 core

struct Light {
    vec3 position;
    vec3 color;
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
    if (normal.y > 0.0f)
    {
        frag_color = texture(grass_texture, tex_coord);
    }
    else
    {
        frag_color = texture(dirt_texture, tex_coord);
    }

    vec3 light_dir = normalize(light.position - frag_pos);
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, normalize(normal));

    vec3 ambient = light.color * vec3(0.5f);
    vec3 diffuse = light.color * vec3(0.5f) * max(dot(normalize(normal), light_dir), 0.0f);
    vec3 specular = light.color * vec3(0.5f) * pow(max(dot(view_dir, reflect_dir), 0.0), 16);

    vec3 lighting = ambient + diffuse + specular;

    frag_color *= vec4(lighting, 1.0f);
}