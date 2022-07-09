#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord_pos;
layout (location = 2) in vec2 a_tex_coord_offset;
layout (location = 3) in vec3 a_normal;
layout (location = 4) in vec3 a_block_pos;

out vec3 frag_pos;
out vec2 tex_coord_pos;
out vec2 tex_coord_offset;
out vec3 normal;
out vec3 block_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0f);

    frag_pos = a_pos;
    tex_coord_pos = a_tex_coord_pos;
    tex_coord_offset = a_tex_coord_offset;
    normal = a_normal;
    block_pos = a_block_pos;
}