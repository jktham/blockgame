#version 460 core

layout (location = 0) in vec2 a_pos;

out vec2 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 0.0f, 1.0f);
    frag_pos = a_pos;
}