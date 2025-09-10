#version 330

in float u_pointSize;

uniform float time;

out vec4 finalColor;

void main() {
	finalColor =
	    vec4(vec3(1.0), 1.0 * (1.0 - length(gl_PointCoord.xy - vec2(0.25)) *
	                                     sin(time)));
}
