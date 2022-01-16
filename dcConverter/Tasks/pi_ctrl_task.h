#ifndef PI_CTRL_TASK_H
#define PI_CTRL_TASK_H

#define SAMPLE_TIME_SECONDS 0.00002F
#define OUTPUT_FREQ 50

typedef struct {
	volatile float *kp;
	volatile float *ki;
	float st; // sampletime
	float min;
	float max;
	volatile float *sp; // setpoint
	float meas;
	float out;
} PI_ctrl_def_t;

volatile PI_ctrl_def_t _PI_ctrl;


// Converter model state space variables
static const float A[6][6] ={
			{0.9652, 	-0.0172,	 0.0057,	-0.0058,	 0.0052,	-0.0251},
			{0.7732, 	 0.1252,	 0.2315,	 0.07,		 0.1282,	 0.7754},
			{0.8278,	-0.7522,	-0.0956,	 0.3299,	-0.4855,	 0.3915},
			{0.9948,	 0.2655,	-0.3848,	 0.4212,	 0.3927,	 0.2899},
			{0.7648,	-0.4165,	-0.4855,	-0.3366,	-0.0986,	 0.7281},
			{1.1056,	 0.7587,	 0.1179,	 0.0748,	-0.2192,	 0.1491}
	};

static const float B[6] = {0.0471, 0.0377, 0.0404, 0.0485, 0.0373, 0.0539};





void pi_ctrl_task(void *params);
void pi_init(volatile float *kp, volatile float *ki, float st, float min, float max, volatile float *sp, float meas, float out);
float converterModel(float u_in);
float PIController(volatile float *Kp, volatile float *Ki, float st, float min, float max, volatile float *sp, float meas);
float dc_to_ac(float u);

void prvTimerHandler( void *CallBackRef );


#endif
