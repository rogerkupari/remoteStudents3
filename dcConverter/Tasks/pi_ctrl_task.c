#include "pi_ctrl_task.h"
#include "zynq_registers.h"
#include "xttcps.h"
#include "OS_includes.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

volatile int PWM_toggle = 0;

void pi_ctrl_task(void *params){
	// Main control loop of the PI controller, converter model and inverter model
	// Inputs: Voltage amplitude setpoint
	// Outputs: AC inverter output (outputAC), PWM-signal (PWM_toggle), RGB-led indicator
	// Execution interval: 100 ms

	float testMin = 0.0;
	float testMax = 5.0;
	float testMeas = 0.0;
	float testOut = 0.0;
	float outputAc = 0.0;

	// wait while ki/kp/setpoint pointers are uninitialized state
	while(g_ki_param_ptr == NULL || g_kp_param_ptr == NULL || g_sp_param_ptr == NULL){
		vTaskDelay(10);
	}
	pi_init(g_kp_param_ptr, g_ki_param_ptr, SAMPLE_TIME_SECONDS, testMin, testMax, g_sp_param_ptr , testMeas, testOut);

	for(;;){

		if(is_g_modulation_semaphore_free()){

			// Calculate PI controller output
			float Pi_out = PIController(_PI_ctrl.kp, _PI_ctrl.ki, _PI_ctrl.st, _PI_ctrl.min, _PI_ctrl.max, _PI_ctrl.sp, _PI_ctrl.meas);

			// Pi_out normalized to range [0,1] and scaled with 2^16-1
			uint16_t TTC_match_value = (uint16_t) ((Pi_out - _PI_ctrl.min)/(_PI_ctrl.max - _PI_ctrl.min) * 65535);
			//float pwm_value = (Pi_out - _PI_ctrl.min)/(_PI_ctrl.max - _PI_ctrl.min) * _PI_ctrl.max;

			// PWM-signal for RGB LED
			TTC0_MATCH_0 = TTC0_MATCH_1_COUNTER_2 = TTC0_MATCH_1_COUNTER_3 = TTC_match_value;

			// PWM-signal for converter model
			TTC1_MATCH_0 = TTC_match_value;


			float measurement = converterModel(Pi_out);
			// bonus 2, kesken
			//float measurement = converterModel(pwm_value * PWM_toggle);

			// Update measured voltage for PI
			_PI_ctrl.meas = measurement;

			// Calculate AC output
			outputAc = dc_to_ac(measurement);

			// Print values to serial terminal
			char charbuf[150];
			sprintf(charbuf, "SP:%.2f, PI:%.2f, Meas:%.2f, Ki:%.2f, Kp:%.2f, Ac:%.2f, PWM:%d\n", *_PI_ctrl.sp, Pi_out, measurement, *_PI_ctrl.ki, *_PI_ctrl.kp, outputAc, PWM_toggle);
			xil_printf(charbuf);


			vTaskDelay(100);
		}
		else {
			if(TTC0_MATCH_0 || TTC0_MATCH_1_COUNTER_2 || TTC0_MATCH_1_COUNTER_3){
				TTC0_MATCH_0 = TTC0_MATCH_1_COUNTER_2 = TTC0_MATCH_1_COUNTER_3 = 0;
			}
			vTaskDelay(20);
		}



	}
}

void pi_init(volatile float *kp, volatile float *ki, float st, float min, float max, volatile float *sp, float meas, float out){
	// Initialises PI controller variables
	_PI_ctrl.kp = kp;
	_PI_ctrl.ki = ki;
	_PI_ctrl.st = st;
	_PI_ctrl.min = min;
	_PI_ctrl.max = max;
	_PI_ctrl.sp = sp;
	_PI_ctrl.meas = meas;
	_PI_ctrl.out = out;

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

float PIController(volatile float *Ki, volatile float *Kp, float st, float min, float max, volatile float *sp, float meas){
	// PI Controller with negative feedback
		static float u1_old = 0;
		float u1;
		float u2;
		float error = *sp - meas;

		u1 = u1_old + *Ki * st * error;
		u2 = *Kp * error;
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

float dc_to_ac(float u){
	// Converts converter model output from DC to AC
	// FreeRTOS tick rate must be divisible by output frequency to eliminate phase error when sineCounter resets
	// library "m" must be added to project C/C++ Build settings/linker/libraries for sin function
	static int sineCounter = 0;

	if (sineCounter == (int) (configTICK_RATE_HZ/OUTPUT_FREQ)){
		sineCounter = 0;
	}

	float uAC = u * sin(2 * M_PI * OUTPUT_FREQ * 1/configTICK_RATE_HZ * sineCounter);
	sineCounter++;
	return uAC;
}


void prvTimerHandler(void *pvCallBackRef) {
	// Interrupt handler triggered by TTC1
	// Toggles PWM signal output
	uint32_t ulInterruptStatus;
	XTtcPs *pxTimer = (XTtcPs *) pvCallBackRef;

	/* Read the interrupt status, then write it back to clear the interrupt. */
	ulInterruptStatus = XTtcPs_GetInterruptStatus(pxTimer);
	XTtcPs_ClearInterruptStatus(pxTimer, ulInterruptStatus);

	int value = XTtcPs_GetCounterValue(pxTimer);
	if (value == 0) {
		PWM_toggle = 0;
	} else {
		PWM_toggle = 1;
	}

	//xil_printf("INTERRUPT %d %d\n", value, PWM_toggle);

}


