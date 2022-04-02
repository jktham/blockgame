#version 460 core

struct Light {
    vec3 direction;
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Fog {
    float start;
    float end;
    float exponent;
    bool enabled;
};

out vec4 frag_color;

in vec3 frag_pos;
in vec2 tex_coord;
in vec3 normal;
in vec3 block;

uniform Light light;
uniform Fog fog;
uniform vec3 view_pos;
uniform vec3 selected_block;
uniform sampler2D atlas_texture;

void main()
{
    frag_color = texture(atlas_texture, tex_coord);

    if (round(block) == round(selected_block))
    {
        frag_color += vec4(vec3(0.1f), 1.0f);
    }

    //vec3 light_dir = normalize(light.position - frag_pos);
    vec3 light_dir = normalize(light.direction);
    vec3 view_dir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, normalize(normal));

    vec3 ambient = light.color * light.ambient;
    vec3 diffuse = light.color * light.diffuse * max(dot(normalize(normal), light_dir), 0.0);
    vec3 specular = light.color * light.specular * pow(max(dot(view_dir, reflect_dir), 0.0), 8);

    vec3 lighting = ambient + diffuse + specular;

    frag_color *= vec4(lighting, 1.0f);

    if (fog.enabled)
    {
        float lum = 1.0f;
        if (distance(view_pos.xy, frag_pos.xy) >= fog.start)
        {
            lum = 1.0f - pow((distance(view_pos.xy, frag_pos.xy) - fog.start) / (fog.end - fog.start), fog.exponent);
        }

        frag_color *= vec4(vec3(clamp(lum, 0.0f, 1.0f)), 1.0f);
    }
}