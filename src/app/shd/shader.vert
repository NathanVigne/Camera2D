#version 330 core

in vec4 vertex;
in vec2 texCoord;
out vec2 texc;
flat out uvec2 line;
uniform mat4 mvp_matrix;
uniform vec2 in_line;

void main(void)
{
   gl_Position = mvp_matrix * vertex;
   texc = texCoord;
   line = uvec2(in_line.x,in_line.y);
}
