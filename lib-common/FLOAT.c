#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	long long result = (long long)a * b;	
	return result >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	/* a<<16 */
	long long la = a << 16;
	
	/*result = a/b*/
	unsigned int result, temp, rel;
	result = 0;
	rel = 0xffffffff / b;
	while (la > (long long)0xffffffff)
	{
		temp = la >> 32;
		result += temp * rel;
		la -= (long long)rel * b * temp;
	}	

	result += (unsigned)la / b;	
	return  result;
}

FLOAT f2F(float a) {
// 0| 0000 0000|000 0000 0000 0000 0000 0000
	int ieee_exp = *(int *)&a;
	int sign = (ieee_exp >> 31) & 1;
	int exp = ((ieee_exp >> 23) & 0xff) - 127;
	int frac = ieee_exp & 0x7fffff;
	/*
	 * float a = 0.1frac * 2 ^ exp
	 * FLOAT A = a * 2 ^ 16 
	 *		   = 0.1frac * 2 ^(16 + exp)
	 *		   = 1frac * 2 ^(16 - 24 + exp)
	 *		   = 1frac * 2 ^(exp - 8)
i	 * */



	int result;

	//alert(exp <= 15);

	if (exp > 8)
		result = ((1 << 23) + frac) << (exp - 8);
	else
		result = ((1 << 23) + frac) >> (8 - exp);


/*
	int temp = 24 - exp;
	//overflow assertion	

	int result;
	
	if (temp <= 17)
		result = ((1 << 23) + frac) << (17 - temp);
	else
		result = ((1 << 23) + frac) >> (temp - 17);

*/
	if (sign) result = -result;
	return result;
}

FLOAT Fabs(FLOAT a) {
	//set_bp();
	int sign = a >> 31;
		
	if (sign == -1) 
		return a;
	else
		return -a;
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));


	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}


