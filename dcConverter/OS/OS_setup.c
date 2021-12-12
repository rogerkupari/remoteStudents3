#include "operation_mode_task.h"

#include "OS_includes.h"
#include "zynq_registers.h"
#include "xttcps.h"
#include "pi_ctrl_task.h"

#include "xil_printf.h"



void initialize_tasks(){
	if(xTaskCreate(operation_mode_task, "operation mode task", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL) != pdPASS){
		xil_printf("button task creation failed\n");
		while(1){};
	}
	if(xTaskCreate(pi_ctrl_task, "PI controller", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS){
		xil_printf("test task creation failed\n");
		while(1){};
	}


}

void middleware_init(){
	g_disable_modulation_semaphore = xSemaphoreCreateBinary();

	// initialize pointers as nullptr
	g_ki_param_ptr = NULL;
	g_kp_param_ptr = NULL;
	g_sp_param_ptr = NULL;
}


void OS_setup(){
	hardware_init();
	middleware_init();
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

	/*
	// uart configuration
	UART_CTRL &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // tx rx enable = 0
	UART_CTRL |= (XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // tx rx disable = 1
	UART_MODE = 0;
	// uart_ref_clk | data = 8-bit | no parity | 1 stop bit | normal mode
	UART_MODE |= (XUARTPS_MR_CHARLEN_8_BIT | XUARTPS_MR_PARITY_NONE | XUARTPS_MR_STOPMODE_1_BIT | XUARTPS_MR_CHMODE_NORM);
	UART_BAUD_DIV = 6; // prescaler
	UART_BAUD_GEN = 124;
	UART_CTRL |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // tx rx logic reset
	UART_CTRL &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // tx rx disable = 0
	UART_CTRL |= (XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN); // tx rx enable = 1
	*/




}
