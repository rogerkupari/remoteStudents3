#include "operation_mode_task.h"

#include "xil_printf.h"
#include "OS_includes.h"
#include <stdbool.h>
#include <stdio.h>



volatile mode_of_operation_def_t mode_of_operation = MODE_CONF;
volatile active_parameter_def_t active_parameter = PARAM_KI;
volatile bool printed = false;


// for button based operations
volatile Button_operations_def_t button_operations = {0};

// parameters
volatile float kp_param_value = 0.0;
volatile float ki_param_value = 0.0;
volatile float sp_param_value = 0.0;

void operation_mode_task(void *params){


	g_ki_param_ptr = &ki_param_value;
	g_kp_param_ptr = &kp_param_value;
	g_sp_param_ptr = &sp_param_value;


	for(;;){


		read_buttons(&button_operations);
		check_mode_and_buttons(&button_operations);
		led_indications(&mode_of_operation);


		// 100ms approximately
		vTaskDelay(100);
	}
}



void check_mode_and_buttons(volatile Button_operations_def_t *o){
	switch (mode_of_operation) {
		case MODE_CONF:
			if(is_semaphore_free()){
				take_semaphore();
			}
			if(o->btn1){
				active_parameter = active_parameter == PARAM_KI ? PARAM_KP : PARAM_KI;
				printed = false;
				o->done = true;
			}
			if(!printed) print_message(MODE_CONF);
			if(o->btn2){
				switch (active_parameter) {
					case PARAM_KI:
						increase_ki();
						print_parameter_value();
						break;
					case PARAM_KP:
						increase_kp();
						print_parameter_value();
						break;
					default:
						break;
				}
			}
			if(o->btn3){
				switch (active_parameter) {
					case PARAM_KI:
						decrease_ki();
						print_parameter_value();
						break;
					case PARAM_KP:
						decrease_kp();
						print_parameter_value();
						break;
					default:
						break;
				}
			}
			if(o->btn0){
				mode_of_operation = MODE_IDLE;
				printed = false;
				o->done = true;
			}
			break;
		case MODE_IDLE:
			if(is_semaphore_free()){
				take_semaphore();
			}
			if(!printed) print_message(MODE_IDLE);
			if(o->btn0){
				mode_of_operation = MODE_MODULATION;
				printed = false;
				o->done = true;
			}

			break;
		case MODE_MODULATION:
			if(!is_semaphore_free()){
				release_semaphore();
			} else {
				if(!printed) print_message(MODE_MODULATION);
				if(o->btn2){
					increase_sp();
				}
				if(o->btn3){
					decrease_sp();
				}
				if(o->btn0){
					mode_of_operation = MODE_CONF;
					printed = false;
					o->done = true;
				}
			}
			break;
		default:
			break;
	}
}

void read_buttons(volatile Button_operations_def_t *o) {

	// clear operations if button released
	if(AXI_BTN_DATA == 0b0000){
		o->btn0 = 0;
		o->btn1 = 0;
		o->btn2 = 0;
		o->btn3 = 0;
		o->done = false;
	}

	/* Check buttons 0 - 3 and enable button operations if disabled
	 *
	 */

	if((AXI_BTN_DATA == 0b0001) && !o->btn0 && !o->done){
		o->btn0 =1;
	}
	else {
		o->btn0 =0;
	}
	if((AXI_BTN_DATA == 0b0010) && !o->btn1 && !o->done){
		o->btn1 =1;
	}
	else {
		o->btn1 =0;
	}
	if((AXI_BTN_DATA  == 0b0100) && !o->btn2 && !o->done){
		o->btn2 =1;
	}
	else {
		o->btn2 = 0;
	}
	if((AXI_BTN_DATA == 0b1000) && !o->btn3 && !o->done){
		o->btn3 =1;
	}
	else {
		o->btn3 = 0;
	}


}

void print_message(int mode){
	char b[50];
	if(mode == MODE_CONF){
		xil_printf("Configuration mode, active parameter = ");
		if(active_parameter == PARAM_KI){
			sprintf(b,"ki (%.2f)\n", *g_ki_param_ptr);
		}
		if(active_parameter == PARAM_KP){
			sprintf(b,"kp (%.2f)\n", *g_kp_param_ptr);
		}

	}

	if(mode == MODE_IDLE){
			sprintf(b,"IDLE mode\n");

	}

	if(mode == MODE_MODULATION){
			sprintf(b,"MODULATION STARTED\n");
	}

	xil_printf(b);
	printed = true;
}

void led_indications(volatile mode_of_operation_def_t *mode){
	AXI_LED_DATA = (1<<*mode);
}

bool take_semaphore(){
	// already taken
	if(!is_semaphore_free()){
		return true;
	}

	if(xSemaphoreTake(g_disable_modulation_semaphore, (TickType_t) 10) != pdTRUE){
		return false;
	}
	return true;
}
bool release_semaphore(){
	// already free
	if(is_semaphore_free()){
		return true;
	}

	if(xSemaphoreGive(g_disable_modulation_semaphore) != pdTRUE){
		return false;
	}

	return true;
}
bool is_semaphore_free(){
	return is_g_modulation_semaphore_free();
}

void increase_ki(void){
	ki_param_value += K_PARAMETER_CHANGE;
}
void decrease_ki(void){
	ki_param_value -= K_PARAMETER_CHANGE;
}
void increase_kp(void){
	kp_param_value += K_PARAMETER_CHANGE;
}
void decrease_kp(void){
	kp_param_value -= K_PARAMETER_CHANGE;
}
void increase_sp(void){
	sp_param_value += SP_PARAMETER_CHANGE;
}
void decrease_sp(void){
	sp_param_value -= SP_PARAMETER_CHANGE;
}
void print_parameter_value(void){
	char b[50];
	if(active_parameter == PARAM_KI){
		sprintf(b, "Ki value: %f\n", *g_ki_param_ptr);
	}
	if(active_parameter == PARAM_KP){
		sprintf(b, "Kp value: %f\n", *g_kp_param_ptr);
	}
	xil_printf(b);
}
