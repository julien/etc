#version 330

in vec2 vertexPosition;
in float pointSize;

uniform mat4 mvp;
uniform float time;

void main() {
	vec2 pos = vertexPosition.xy;

	gl_Position = mvp * vec4(pos.x, pos.y, 0.0, 1.0);
	gl_PointSize = pointSize;
}
