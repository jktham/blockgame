#version 460 core

in vec3 color;
in vec2 tex_coord;

out vec4 frag_color;

uniform sampler2D atlas_texture;

void main()
{
    if (color.r == -1.0f)
    {
        frag_color = texture(atlas_texture, tex_coord);
    }
    else
    {
        frag_color = vec4(color, 1.0f);
    }
}