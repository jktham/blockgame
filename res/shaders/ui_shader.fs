#version 460 core

in vec4 color;
in vec2 tex_coord;
in float tex_type;

out vec4 frag_color;

uniform sampler2D atlas_texture;
uniform sampler2D font_texture;

void main()
{
    if (tex_type == 2.0f)
    {
        frag_color = color * texture(font_texture, tex_coord);
    }
    else if (tex_type == 1.0f)
    {
        frag_color = color * texture(atlas_texture, tex_coord);
    }
    else
    {
        frag_color = color;
    }
}