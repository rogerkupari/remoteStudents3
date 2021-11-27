#include "OS_includes.h"
#include "zynq_registers.h"
#include "xttcps.h"
#include "test_task.h"
#include "pi_ctrl_task.h"
#include "xil_printf.h"



void initialize_tasks(){
	if(xTaskCreate(test_task, "test task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS){
		xil_printf("test task creation failed\n");
		while(1){};
	}
	if(xTaskCreate(pi_ctrl_task, "PI controller", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS){
		xil_printf("test task creation failed\n");
		while(1){};
	}

}


void OS_setup(){
	hardware_init();
	initialize_tasks();
	vTaskStartScheduler();
}

void hardware_init(){
	// leds = output
	AXI_LED_TRI = ~0xF;

	// clock control prescaler / 1
	TTC0_CLK_CNTRL  = (0 << XTTCPS_CLK_CNTRL_PS_VAL_SHIFT) | XTTCPS_CLK_CNTRL_PS_EN_MASK;
	TTC0_CLK_CNTRL2 = (0 << XTTCPS_CLK_CNTRL_PS_VAL_SHIFT) | XTTCPS_CLK_CNTRL_PS_EN_MASK;
	TTC0_CLK_CNTRL3 = (0 << XTTCPS_CLK_CNTRL_PS_VAL_SHIFT) | XTTCPS_CLK_CNTRL_PS_EN_MASK;

	// Operational and mode reset = reset count value | disable counter | match mode | enable output to RGB
	TTC0_CNT_CNTRL  = XTTCPS_CNT_CNTRL_RST_MASK | XTTCPS_CNT_CNTRL_DIS_MASK | XTTCPS_CNT_CNTRL_MATCH_MASK | XTTCPS_CNT_CNTRL_POL_WAVE_MASK;
	TTC0_CNT_CNTRL2 = XTTCPS_CNT_CNTRL_RST_MASK | XTTCPS_CNT_CNTRL_DIS_MASK | XTTCPS_CNT_CNTRL_MATCH_MASK | XTTCPS_CNT_CNTRL_POL_WAVE_MASK;
	TTC0_CNT_CNTRL3 = XTTCPS_CNT_CNTRL_RST_MASK | XTTCPS_CNT_CNTRL_DIS_MASK | XTTCPS_CNT_CNTRL_MATCH_MASK | XTTCPS_CNT_CNTRL_POL_WAVE_MASK;

	// Clear counter match values
	TTC0_MATCH_0           = 0;
	TTC0_MATCH_1_COUNTER_2 = 0;
	TTC0_MATCH_1_COUNTER_3 = 0;

	// start timers
	TTC0_CNT_CNTRL  &= ~XTTCPS_CNT_CNTRL_DIS_MASK;
	TTC0_CNT_CNTRL2 &= ~XTTCPS_CNT_CNTRL_DIS_MASK;
	TTC0_CNT_CNTRL3 &= ~XTTCPS_CNT_CNTRL_DIS_MASK;

}
