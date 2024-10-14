uniform mat4 u_matProjectionView;
uniform mat4 u_matViewModel;

attribute vec4 a_position;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;

void main(void)
{
	v_texCoord = a_texCoord;
	gl_Position = u_matProjectionView * u_matViewModel * a_position;
}
