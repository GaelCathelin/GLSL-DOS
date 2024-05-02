// https://www.shadertoy.com/view/4ccSzX
#define MAX_STEPS 42.0
#define EPSILON 1.0 / 42.0

vec2 rotate(vec2 v, float r) {
/*
	float c = cos(r), s = sin(r);
	mat2 rot = mat2(c, -s, s, c);
	return rot * v;
/*/
	// Approximation used in the demo, where for small angles, cos(r) ~= 1.0 and sin(r) ~= r
	// It simplfies into this
	return v + v.yx * vec2(r, -r);
//*/
}

void mainImage(OUT(vec4, fragColor), vec2 fragCoord) {
	float time = mod(iTime / 16.0, 6.0);
	int shot = int(time); // current camera shot

	vec3 pos = vec3(-69.0, -6.25, 24.0); // starting position
	pos.y += float(shot);
	pos.z -= 4.0 * time;

	fragCoord.y -= 0.1 * iResolution.y; // look down a bit
	vec3 dir = vec3((2.0 * fragCoord - iResolution.xy) / iResolution.y, -1.0);
	dir.x *= 0.75;

	// camera behave slightly differently on even and odd shots
	if ((shot & 1) != 0) {
		pos.y += float(shot) + 4.0 * (time - float(shot));
		dir = vec3(-dir.z, dir.yx);
	}

	// fractal gets slighly modified each shot
	float fract_rot = 0.417 + 0.0095 * float(shot);

	float l;
	for (l = 0.0; l < MAX_STEPS; l++) {
		// The fractal
		vec3 v = pos;
		float rayStep = shot < 4 ? pos.y : 0.0; // ceiling is popped-off in the first 4 shots
		for (float i = 100.0; i > 0.5; i *= fract_rot) {
			vec2 r = rotate(v.xz, 0.198);
			v.x = r.x; v.z = r.y;
			v = 0.79 * i - abs(mod(v, 2.0 * i) - i);
			rayStep = max(rayStep, min(v.x, min(v.y, v.z)));
		}

		if (rayStep < EPSILON)
			break;

		pos += 0.6 * rayStep * dir;
	}

	fragColor = vec4(vec3(l / MAX_STEPS), 1.0);
}