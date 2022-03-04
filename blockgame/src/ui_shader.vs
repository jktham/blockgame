#version 460 core

layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in float a_tex_type;

out vec3 color;
out vec2 tex_coord;
out float tex_type;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 0.0f, 1.0f);
    color = a_color;
    tex_coord = a_tex_coord;
    tex_type = a_tex_type;
}