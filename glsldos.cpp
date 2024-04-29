inline void begin() {
	asm volatile (
		// reserve a stack
		"pushaw\n\t"
		"sub sp, 640\n\t" // ought to be enough for anybody
		// mode 13h
		"mov al, 0x13\n\t"
		"int 0x10\n\t"
		// inverted grayscale palette
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
		// set dest seg to framebuffer
		"pushw 0xA000\n\t"
		"popw es\n\t"
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
		// "add %1, es:[%0 - 1]\n\t"
		// "shr %1, 1\n\t"
		"stosb\n\t"
		: "+r"(pixelreg)
		: "r"(colreg));
}

extern "C" void __attribute__((naked)) startup() {
	begin();

	const unsigned short t0 = timer();
	for (unsigned short pixel = 0; !finished(); pixel += 40503) {
		iTime = (timer() - t0) / 18.2f;
		vec4 fragColor;
		mainImage(fragColor, vec2(pixel % 320, 1.2f * (179 - (unsigned char)(pixel / 320))));
		plot(pixel, min(64.0f * (1.0f - fragColor.r), 63.0f));
	}

	end();
}

extern "C" float floorf(const float x) {return nearbyintf(x - 0.5f);}