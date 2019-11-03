#define rotl(x,k) ( (x << (k)) | (x >> (32u - (k))))

float next(inout uvec2 s) {
	uint s0 = s.x;
	uint s1 = s.y;
	uint result = rotl(s0 * 0x9E3779BBu, 5u) * 5u;

	s1 ^= s0;
	s.x = rotl(s0, 26u) ^ s1 ^ (s1 << 9u); // a, b
	s.y = rotl(s1, 13u); // c
	
    return uintBitsToFloat((result >> 9)|0x3f000000u);
}
//From xoroshito64*
