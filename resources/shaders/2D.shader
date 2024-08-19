#shader vertex
#version 330 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 aTex;
out vec2 texCoord;

uniform vec4 p;

const mat4 proj = mat4(vec4(2.0, 0.0, 0.0, 0.0), vec4(0.0, -2.0, 0.0, 0.0), vec4(0.0, 0.0, -1.0, 0.0), vec4(-1.0, 1.0, 0.0, 1.0));

void main()
{
	gl_Position = proj * (position+p);
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