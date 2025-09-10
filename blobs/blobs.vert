#version 330

in vec2 vertexPosition;
in float pointSize;

uniform float time;

out float u_pointSize;

void main() {
	vec2 pos = vertexPosition.xy;
	u_pointSize = pointSize;

	gl_Position = vec4(pos, 0.0, 1.0);
	gl_PointSize = 3 - abs(sin(pointSize * time));
}
