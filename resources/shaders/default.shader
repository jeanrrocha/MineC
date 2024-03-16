#shader vertex
#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 aTex;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
   gl_Position = proj * view * model * position;
   texCoord = aTex;
}


#shader fragment
#version 330 core
out vec4 color;
in vec2 texCoord;

uniform sampler2D tex0;

void main()
{
   //color = vec4(0, 0, 0, 1);
   color = texture(tex0, texCoord);
}

#END