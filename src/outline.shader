#shader vertex
#version 330 core
layout (location = 0) in vec4 position;

out vec4 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
   gl_Position = proj * view * model * position;
}


#shader fragment
#version 330 core
out vec4 color;
void main()
{
   color = vec4(0.0f, 0.0f, 0.0f, 1.0f);;
}

#END