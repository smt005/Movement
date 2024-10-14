#version 330 core

uniform mat4 u_matProjectionView;
uniform mat4 u_matViewModel;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texCoord;

out vec2 v_texCoord;

void main() {
	v_texCoord = a_texCoord;
	gl_Position = u_matProjectionView * u_matViewModel * vec4(a_position, 1.0);
}
