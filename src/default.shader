#shader vertex
#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 inColor;

out vec4 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
   gl_Position = proj * view * model * position;
   fragColor = inColor;
}


#shader fragment
#version 330 core
out vec4 color;
in vec4 fragColor;
void main()
{
   color = fragColor;
}

#END