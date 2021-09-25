#include "stdio.h"
#include "x86intrin.h"
#include "stdint.h"
#include <iostream>

#define u8  1
#define i8  2
#define u16 3
#define i16 4
#define u32 5
#define i32 6
#define u64 7
#define i64 8

typedef union {
	__m128i 			int128;

	unsigned char		m128_u8[16];
	signed char			m128_i8[16];

	unsigned short		m128_u16[8];
	signed short		m128_i16[8];

	uint32_t		m128_u32[4];
	int32_t		m128_i32[4];

	uint64_t		m128_u64[2];
	int64_t		m128_i64[2];
} intVec;

typedef union {
	__m128 			float128;

	float		m128_spfp[4];
} floatVec;

void print_int_vector (__m128i a, int type)
{
	intVec tmp;
	tmp.int128 = a;

	switch (type)
	{
	case u8:
		printf("Byte in Hex: [");
		for (int i=15; i>0; i--) {
			printf ("%X, ", tmp.m128_u8[i]);
		}
		printf ("%X", tmp.m128_u8[0]);
		printf("]\n\n");

		printf("Unsigned Byte in Decimal: [");
		for (int i=15; i>0; i--) {
			printf ("%u, ", tmp.m128_u8[i]);
		}
		printf ("%u", tmp.m128_u8[0]);
		break;

	case i8:
		printf("Signed Byte in Decimal: [");
		for (int i=15; i>0; i--) {
			printf ("%d, ", tmp.m128_i8[i]);
		}
		printf ("%d", tmp.m128_i8[0]);
		break;

	case u16:
		printf("Word in Hex: [");
		for (int i=7; i>0; i--) {
			printf ("%X, ", tmp.m128_u16[i]);
		}
		printf ("%X", tmp.m128_u16[0]);
		printf("]\n\n");

		printf("Unsigned Word in Decimal: [");
		for (int i=7; i>0; i--) {
			printf ("%u, ", tmp.m128_u16[i]);
		}
		printf ("%u", tmp.m128_u16[0]);
		break;

	case i16:
		printf("Signed Word in Decimal: [");
		for (int i=7; i>0; i--) {
			printf ("%d, ", tmp.m128_i16[i]);
		}
		printf ("%d", tmp.m128_i16[0]);
		break;

	case u32:
		printf("Double-Word in Hex: [");
		for (int i=3; i>0; i--) {
			printf ("%X, ", tmp.m128_u32[i]);
		}
		printf ("%X", tmp.m128_u32[0]);
		printf("]\n\n");

		printf("Unsigned Double-Word in Decimal: [");
		for (int i=3; i>0; i--) {
			printf ("%u, ", tmp.m128_u32[i]);
		}
		printf ("%u", tmp.m128_u32[0]);
		break;

	case i32:
		printf("Signed Double-Word in Decimal: [");
		for (int i=3; i>0; i--) {
			printf ("%d, ", tmp.m128_i32[i]);
		}
		printf ("%d", tmp.m128_i32[0]);
		break;

	case u64:
		printf("Quad-Word in Hex: [");
		for (int i=1; i>0; i--) {
			printf ("%llX, ", tmp.m128_u64[i]);
		}
		printf ("%llX", tmp.m128_u64[0]);
		printf("]\n\n");

		printf("Unsigned Quad-Word in Decimal: [");
		for (int i=1; i>0; i--) {
			printf ("%llu, ", tmp.m128_u64[i]);
		}
		printf ("%llu", tmp.m128_u64[0]);
		break;

	case i64:
		printf("Signed Quad-Word in Decimal: [");
		for (int i=1; i>0; i--) {
			printf ("%lld, ", tmp.m128_i64[i]);
		}
		printf ("%lld", tmp.m128_i64[0]);
		break;

	default:
		printf("Unsupported format in SSE3\n");
		break;
	}
	printf("]\n\n");
}

void print_spfp_vector (__m128 a)
{
	floatVec tmp;
	tmp.float128 = a;
	printf("Single Precision in Dec: [");
	for (int i=3; i>0; i--) {
		printf ("%f, ", tmp.m128_spfp[i]);
	}
	printf ("%f", tmp.m128_spfp[0]);
	printf ("]\n\n");
}


unsigned char intArray [16] = {	0X00, 0X11, 0X22, 0X33, 0X44, 0X55, 0X66, 0X77,
								0X88, 0X99, 0XAA, 0XBB, 0XCC, 0XDD, 0XEE, 0XFF};

float floatArray[4] = {1.55, 21.666, 321.7777, 4321.88888};

int main(void)
{
	__m128i a;
	a = _mm_load_si128((const __m128i*)intArray);

	__m128 b;
	b = _mm_load_ps(floatArray);

	for(int i = 1; i<9; i++){
		print_int_vector(a, i);
	}

	printf("\n\n");

	print_spfp_vector(b);

	return 0;
}
