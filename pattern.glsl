void mainImage(OUT(vec4, fragColor), vec2 fragCoord) {
	fragColor = vec4(vec3(iFrame & 1), 1.0);
}