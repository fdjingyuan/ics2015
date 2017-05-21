#include "FLOAT.h"

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	long long c = (long long) a * (long long) b;
	return (FLOAT)(c>>16);
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	//1 means positive number
	int sign = 1;
	if(a < 0)
	{
		sign=-sign;
		a=-a;
	}
	if(b < 0)
	{
		sign=-sign;
		b=-b;
	}
	//the integer part of the result
	int result = a / b;
	//the decimal part of the result
	a = a % b;
	int i;
	for(i = 0;i < 16; i++)
	{
		//extract the decimal part of the result, left shifting 
		a=a<<1;
		result=result<<1;
		
		if(a >= b)
		{
			a-=b;
			result++;
		}
	}
	return result * sign;
}

FLOAT f2F(float a) {
	
}

FLOAT Fabs(FLOAT a) {
	
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

