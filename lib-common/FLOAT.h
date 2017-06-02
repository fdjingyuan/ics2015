#ifndef __FLOAT_H__
#define __FLOAT_H__

#include "trap.h"

typedef int FLOAT;

static inline int F2int(FLOAT a) {
	int c = a>>16;
	return c;
}

static inline FLOAT int2F(int a) {
	FLOAT c = a<<16;
	return c;
}

static inline FLOAT F_mul_int(FLOAT a, int b) {
	//special situation: can compute directly
	FLOAT c = a*b;
	return c;
}

static inline FLOAT F_div_int(FLOAT a, int b) {
	//special situation: can compute directly
	FLOAT c =a/b;
	return c;
}

FLOAT f2F(float);
FLOAT F_mul_F(FLOAT, FLOAT);
FLOAT F_div_F(FLOAT, FLOAT);
FLOAT Fabs(FLOAT);
FLOAT sqrt(FLOAT);
FLOAT pow(FLOAT, FLOAT);

#endif
