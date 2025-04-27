#version 330

out vec4 FragColor;

uniform float time;
uniform vec2 resolution;

float shakeGate(float t) {
	// shake between 5s-6s and again between 12s-14s
	float g1 = step(5.0, t) - step(6.0, t);
	float g2 = step(12.0, t) - step(14.0, t);
	return g1 + g2;
}

void main() {
	vec2 uv = (gl_FragCoord.xy - (resolution * 0.5)) / resolution.y;

	float scale = 1.0 + 0.2 * sin(time * 1.7);
	uv *= scale;

	float c = cos(time * 0.7 + sin(time * 0.23) * 2.0);
	float s = sin(time * 0.9 + cos(time * 0.17) * 2.5);
	mat2 rot = mat2(c, -s, s, c);
	uv = rot * uv;

	float gate = shakeGate(time);

	vec2 shake = gate * vec2(
		sin(time * 12.0) * 0.007 + sin(time * 32.0) * 0.003,
		cos(time * 15.0) * 0.002 + cos(time * 48.0) * 0.008
	);
	uv += shake;

	float pattern = fract(
		max(abs(uv.x * 2.0) + uv.y * sin(time * (2.0)),
			abs(uv.y * 1.3) - uv.x * cos(time * (3.0))
		) * (5.0 + c));

	pattern = step(0.6, pattern);
	vec3 color = vec3(0.1 + 0.2 * s, pattern, 0.3 + 0.2 * c);

	FragColor = vec4(color, 1.0);
}
