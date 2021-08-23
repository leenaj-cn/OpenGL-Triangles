#version 450

in vec4 i_vPosition;
in vec4 i_vColor;
out vec4 o_color;

void main()
{
    o_color = i_vColor;
    gl_Position = i_vPosition; 
}
