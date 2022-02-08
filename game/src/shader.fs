#version 460 core

struct Light {
    vec3 position;
    vec3 color;
};

uniform Light light;

out vec4 frag_color;

in vec2 tex_coord;
in vec3 normal;

uniform sampler2D dirt_texture;
uniform sampler2D grass_texture;

void main()
{
    if (normal == vec3(0.0f, 1.0f, 0.0f))
    {
        frag_color = texture(grass_texture, tex_coord);
    }
    else
    {
        frag_color = texture(dirt_texture, tex_coord);
    }

    frag_color *= vec4(light.color, 0.0f);
}