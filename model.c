
#include "model.h"



float PIController(float Vref, float V_measured, float Ki, float Kp, float Ts, float min, float max){
	// PI Controller with negative feedback
	static float u1_old = 0;
	float u1;
	float u2;
	float error = Vref - V_measured;

	u1 = u1_old + Ki * Ts * error;
	u2 = Kp * error;
	u1_old = u1;

	// Check controller saturation
	if((u1 + u2) < min){
		return min;
	}
	else if ((u1 + u2) > max){
		return max;
	}
	else{
		return (u1 + u2);
	}
}

float converterModel(float u_in){
	// Discrete power converter state-space model, discretized with 50 kHz sampling frequency
	float i1, i2, i3, u1, u2, u3;
	static float i1_old = 0;
	static float u1_old = 0;
	static float i2_old = 0;
	static float u2_old = 0;
	static float i3_old = 0;
	static float u3_old = 0;

	const float A[6][6] ={
			{0.9652, 	-0.0172,	 0.0057,	-0.0058,	 0.0052,	-0.0251},
			{0.7732, 	 0.1252,	 0.2315,	 0.07,		 0.1282,	 0.7754},
			{0.8278,	-0.7522,	-0.0956,	 0.3299,	-0.4855,	 0.3915},
			{0.9948,	 0.2655,	-0.3848,	 0.4212,	 0.3927,	 0.2899},
			{0.7648,	-0.4165,	-0.4855,	-0.3366,	-0.0986,	 0.7281},
			{1.1056,	 0.7587,	 0.1179,	 0.0748,	-0.2192,	 0.1491}
	};

	const float B[6] = {0.0471, 0.0377, 0.0404, 0.0485, 0.0373, 0.0539};


	i1 = A[0][0] * i1_old + A[0][1] * u1_old + A[0][2] * i2_old + A[0][3] * u2_old + A[0][4] * i3_old + A[0][5] * u3_old + B[0] * u_in;
	u1 = A[1][0] * i1_old + A[1][1] * u1_old + A[1][2] * i2_old + A[1][3] * u2_old + A[1][4] * i3_old + A[1][5] * u3_old + B[1] * u_in;
	i2 = A[2][0] * i1_old + A[2][1] * u1_old + A[2][2] * i2_old + A[2][3] * u2_old + A[2][4] * i3_old + A[2][5] * u3_old + B[2] * u_in;
	u2 = A[3][0] * i1_old + A[3][1] * u1_old + A[3][2] * i2_old + A[3][3] * u2_old + A[3][4] * i3_old + A[3][5] * u3_old + B[3] * u_in;
	i3 = A[4][0] * i1_old + A[4][1] * u1_old + A[4][2] * i2_old + A[4][3] * u2_old + A[4][4] * i3_old + A[4][5] * u3_old + B[4] * u_in;
	u3 = A[5][0] * i1_old + A[5][1] * u1_old + A[5][2] * i2_old + A[5][3] * u2_old + A[5][4] * i3_old + A[5][5] * u3_old + B[5] * u_in;

	i1_old = i1;
	u1_old = u1;
	i2_old = i2;
	u2_old = u2;
	i3_old = i3;
	u3_old = u3;

	return u3;
}
