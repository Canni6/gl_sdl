#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
uniform vec4 offset;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    gl_Position.x += offset.x;
    gl_Position.y += offset.y;
    ourColor = aColor;
}
    
    
    