#include "zynq_registers.h"
#include "OS_includes.h"
#include "xil_printf.h"


void test_task(void *params){
	AXI_LED_DATA = 1;
	for(;;){

		xil_printf("hello from test task\n");
		if((AXI_LED_DATA & 0xF) == 0xF){
			AXI_LED_DATA = 1;
		}
		else {
			AXI_LED_DATA |= (AXI_LED_DATA & 0xF)<<1;
		}


		vTaskDelay(500);
	}
}
