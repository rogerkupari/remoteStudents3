#include <zynq_registers.h>
#include <xil_printf.h>
#include <xparameters.h>
#include <xuartps.h>
#include <stdio.h>
#include <stdlib.h>
#include <xttcps.h>
#include "model.h"


int main(void) {

	float Kp = 2.0;
	float Ki = 3.0;
	float Ts = 0.00002; // 1/50 kHz
	float PImin = 0;
	float PImax = 1.0;
	float ref = 0;
	float meas = 0;
	float PI_out;

	int counter = 0;
	int button_released = 1;
	char charbuf[100];

	AXI_BTN_TRI = 0b1111;


	// TTC0 asetukset RGB-ledin ajoa varten, kopioitu harjoitustehtävästä 5


	// First we need to set up the 'Clock Control' -register - TTC0_CLK_CNTRLx :
	//     1. Set prescale to 0 (plus 1) (hint: (value << XTTCPS_CLK_CNTRL_PS_VAL_SHIFT)
	//     2. Enable prescaler (hint: use XTTCPS_CLK_CNTRL_PS_EN_MASK mask)
	TTC0_CLK_CNTRL  = (0 << XTTCPS_CLK_CNTRL_PS_VAL_SHIFT) | XTTCPS_CLK_CNTRL_PS_EN_MASK;
	TTC0_CLK_CNTRL2 = (0 << XTTCPS_CLK_CNTRL_PS_VAL_SHIFT) | XTTCPS_CLK_CNTRL_PS_EN_MASK; // Set identical to TTC0_CLK_CNTRL
	TTC0_CLK_CNTRL3 = (0 << XTTCPS_CLK_CNTRL_PS_VAL_SHIFT) | XTTCPS_CLK_CNTRL_PS_EN_MASK; // Set identical to TTC0_CLK_CNTRL

	// Then let's set correct values to 'Operational mode and reset' -register - TTC0_CNT_CNTRLx :
	//     1. Reset count value (hint: use XTTCPS_CNT_CNTRL_RST_MASK mask)
	//     2. Disable counter (XTTCPS_CNT_CNTRL_DIS_MASK)
	//     3. Set timer to Match mode (XTTCPS_CNT_CNTRL_MATCH_MASK)
	//     4. Enable output Waveform (XTTCPS_CNT_CNTRL_POL_WAVE_MASK)
	//        (Waveform output is default to EMIO, which is connected in the FPGA to the RGB led (LD6)
	TTC0_CNT_CNTRL  = XTTCPS_CNT_CNTRL_RST_MASK | XTTCPS_CNT_CNTRL_DIS_MASK | XTTCPS_CNT_CNTRL_MATCH_MASK | XTTCPS_CNT_CNTRL_POL_WAVE_MASK;
	TTC0_CNT_CNTRL2 = XTTCPS_CNT_CNTRL_RST_MASK | XTTCPS_CNT_CNTRL_DIS_MASK | XTTCPS_CNT_CNTRL_MATCH_MASK | XTTCPS_CNT_CNTRL_POL_WAVE_MASK; // Set identical to TTC0_CNT_CNTRL
	TTC0_CNT_CNTRL3 = XTTCPS_CNT_CNTRL_RST_MASK | XTTCPS_CNT_CNTRL_DIS_MASK | XTTCPS_CNT_CNTRL_MATCH_MASK | XTTCPS_CNT_CNTRL_POL_WAVE_MASK; // Set identical to TTC0_CNT_CNTRL

	// Match value register - TTC0_MATCH_x
	//     1. Initialize match value to 0
	TTC0_MATCH_0           = 0;
	TTC0_MATCH_1_COUNTER_2 = 0;
	TTC0_MATCH_1_COUNTER_3 = 0;

	// Operational mode and reset register - TTC0_CNT_CNTRLx
	//     1. Start timer (hint: clear operation using XTTCPS_CNT_CNTRL_DIS_MASK)
	TTC0_CNT_CNTRL  &= ~XTTCPS_CNT_CNTRL_DIS_MASK;
	TTC0_CNT_CNTRL2 &= ~XTTCPS_CNT_CNTRL_DIS_MASK;
	TTC0_CNT_CNTRL3 &= ~XTTCPS_CNT_CNTRL_DIS_MASK;

	// Variable initialization
	uint16_t pwm_output = 0;


	while(1){
		PI_out = PIController(ref, meas, Ki, Kp, Ts, PImin, PImax);
		meas = converterModel(PI_out);

		// RGB-led
		// TTC 16-bittinen -> max arvo: 2^16-1 = 65535

		pwm_output = (uint16_t) (PI_out * 65535);
		TTC0_MATCH_0 = TTC0_MATCH_1_COUNTER_2 = TTC0_MATCH_1_COUNTER_3 = pwm_output;

		// Referenssiarvon säätö napeilla
		// Vain PI-säätimen ja mallin testausta varten
		if(AXI_BTN_DATA == 0b0001 && button_released){
			ref = ref + 0.05;
			button_released = 0;
		}
		else if(AXI_BTN_DATA == 0b0010 && button_released){
			ref = ref - 0.05;
			button_released = 0;
		}
		else if(AXI_BTN_DATA == 0b0000){
			button_released = 1;
		}



		if (counter == 1000000){
			sprintf(charbuf, "Vref: %f\nPI out: %f\nModel out: %f\r\nMatch value: %d\n\n", ref, PI_out, meas, pwm_output);
			xil_printf(charbuf);
			counter = 0;
		}
		counter++;
	}


return 0;
}
