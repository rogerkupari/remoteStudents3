#include "OS_includes.h"
#include "zynq_registers.h"
#include "test_task.h"
#include "xil_printf.h"



void initialize_tasks(){
	if(xTaskCreate(test_task, "test task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS){
		xil_printf("test task creation failed\n");
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
}
