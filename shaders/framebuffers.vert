#version 460 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	TexCoords = texCoords;
}