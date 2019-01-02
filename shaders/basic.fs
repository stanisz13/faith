#version 330 core

in vec2 pos;

out vec4 FragColor;

void main()
{
	FragColor = vec4(pos, 1.0f, 1.0f);
}

