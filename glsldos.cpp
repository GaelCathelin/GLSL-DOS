inline void begin() {
	asm volatile (
		// reserve a stack
		"pushaw\n\t"
		"sub sp, 640\n\t" // ought to be enough for anybody
		// mode 13h
		"mov al, 0x13\n\t"
		"int 0x10\n\t"
		// set dest seg to framebuffer
		"pushw 0xA000\n\t"
		"popw es\n\t"

	#ifdef GRAYSCALE
		"mov dx, 0x3C9\n\t"
		"mov al, 64\n\t"
		"L_palette:\n\t"
			"dec ax\n\t"
			"out dx, al\n\t"
			"out dx, al\n\t"
			"out dx, al\n\t"
			"jne L_palette\n\t"
		// reset timer segment
		"mov fs, ax\n\t"
	#else
		#ifdef RGB
			"mov dx, 0x3C9\n\t"
			"xor bx, bx\n\t"
			"L_red:\n\t"
				"xor cx, cx\n\t"
				"L_green:\n\t"
					"xor ah, ah\n\t"
					"L_blue:\n\t"
						"mov al, bh\n\t"
						"out dx, al\n\t"
						"mov al, ch\n\t"
						"out dx, al\n\t"
						"mov al, ah\n\t"
						"out dx, al\n\t"
						"add ah, 0x15\n\t"
						"cmp ah, 0x54\n\t"
						"jne L_blue\n\t"
					"add cx, 0x924\n\t"
					"cmp cx, 0x4920\n\t"
					"jne L_green\n\t"
				"add bx, 0x924\n\t"
				"cmp bx, 0x4920\n\t"
				"jne L_red\n\t"
		#endif
		// reset timer segment
		"pushw 0\n\t"
		"popw fs\n\t"
	#endif
		:);
}

inline void end() {
	asm volatile (
		// proper DOS exit
		"add sp, 640\n\t"
		"popaw\n\t"
		"retw\n\t"
		:);
	__builtin_unreachable();
}

inline unsigned short timer() {
	unsigned short ticks;
	asm volatile ("movw %0, fs:0x046c\n\t" : "=irm"(ticks));
	return ticks;
}

inline bool finished() {
	register unsigned char key asm("al");
	asm volatile ("in %0, 0x60\n\t" : "=r"(key));
	return key == 1; // Esc
}

inline void plot(const unsigned short pixel, const unsigned char color) {
	register unsigned short pixelreg asm("di") = pixel;
	register const unsigned char colreg asm("al") = color;
	asm volatile (
		// "add %1, es:[%0 - 1]\n\t" // simple spatio-temporal
		// "shr %1, 1\n\t"           // filtering used in Remnants
		"stosb\n\t"
		: "+r"(pixelreg)
		: "r"(colreg));
}

extern "C" void __attribute__((naked)) startup() {
#ifdef RGB
	static const float bayer[2][2] = {
		{0.125, 0.875},
		{0.625, 0.375},
	};
#endif

	begin();

	const unsigned short t0 = timer();
	for (unsigned short pixel = 0; !finished(); pixel += 40503) {
		iTime = (timer() - t0) / 18.2;
		const unsigned short x = pixel % 320;
		const unsigned char y = pixel / 320;
		vec4 fragColor;
		mainImage(fragColor, vec2(x, 1.2 * (179 - y)));

	#ifdef GRAYSCALE
		const unsigned char color = clamp(64.0f * (1.0f - fragColor.g), 0.0f, 63.0f);
	#elif defined(RGB)
		fragColor = vec4(clamp(vec3(7.0, 7.0, 3.0) * fragColor.rgb + bayer[y & 1][x & 1], vec3(0.0), vec3(7.25, 7.25, 3.25)), fragColor.a);
		const unsigned char color = ((unsigned char)fragColor.r << 5) | ((unsigned char)fragColor.g << 2) | (unsigned char)fragColor.b;
	#else
		const unsigned char color = 16 + (unsigned char)clamp(16.0f * fragColor.g, 0.0f, 15.0f);
	#endif

		plot(pixel, color);
	}

	end();
}

extern "C" float floorf(const float x) {return nearbyintf(x - 0.5f);}