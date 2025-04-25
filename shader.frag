#version 330

out vec4 FragColor;

uniform float time;
uniform vec2 resolution;
uniform vec4 rotation;

void main() {
	vec2 uv = (gl_FragCoord.xy - (resolution * 0.5)) / resolution.y;

	mat2 rot = mat2(rotation.xy, rotation.zw);
	uv = rot * uv;

	float pattern = fract(
		max(abs(uv.x * 2.0) + uv.y * sin(time * (2.0)),
			abs(uv.y * 1.3) - uv.x * cos(time * (3.0))
		) * (5.0 + cos(time)));

	pattern = step(0.6, pattern);
	vec3 color = vec3(0.2 + 0.2 * sin(time), pattern, 0.3 + 0.2 * cos(time));

	FragColor = vec4(color, 1.0);
}
