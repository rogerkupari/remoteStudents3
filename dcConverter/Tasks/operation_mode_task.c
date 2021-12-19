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

	xil_printf("button 0 = configuration/idle/modulation mode\n");
	xil_printf("button 1 = change kp or ki / none / none \n");
	xil_printf("button 2 = decrease active parameter / none / decrease set point \n");
	xil_printf("button 3 = increase active parameter / none / increase set point \n");

	xil_printf("uart '0' = configuration mode\n");
	xil_printf("uart '1' = idle mode\n");
	xil_printf("uart '2' = modulation mode\n");
	xil_printf("uart '*' = change ki or kp in configuration mode \n");
	xil_printf("uart '+' = increase active parameter in configuration mode \n");
	xil_printf("uart '+' = increase setpoint in modulation mode \n");
	xil_printf("uart '-' = decrease active parameter in configuration mode \n");
	xil_printf("uart '-' = decrease setpoint in modulation mode \n");




	for(;;){

		check_messages();
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
			if(is_modulation_semaphore_free()){
				take_modulation_semaphore();
			}
			if(!printed) print_message(MODE_CONF);
			operate_by_buttons(mode_of_operation, o);
			break;
		case MODE_IDLE:
			if(is_modulation_semaphore_free()){
				take_modulation_semaphore();
			}
			if(!printed) print_message(MODE_IDLE);
			operate_by_buttons(mode_of_operation, o);

			break;
		case MODE_MODULATION:
			if(!is_modulation_semaphore_free()){
				release_modulation_semaphore();
			} else {
				if(!printed) print_message(MODE_MODULATION);
				operate_by_buttons(mode_of_operation, o);
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

bool take_modulation_semaphore(){
	// already taken
	if(!is_modulation_semaphore_free()){
		return true;
	}

	if(xSemaphoreTake(g_disable_modulation_semaphore, (TickType_t) 10) != pdTRUE){
		return false;
	}
	return true;
}
bool release_modulation_semaphore(){
	// already free
	if(is_modulation_semaphore_free()){
		return true;
	}

	if(xSemaphoreGive(g_disable_modulation_semaphore) != pdTRUE){
		return false;
	}

	return true;
}
bool is_modulation_semaphore_free(){
	return is_g_modulation_semaphore_free();
}

bool take_console_semaphore(){
	// already taken
	if(!is_console_semaphore_free()){
		return true;
	}

	if(xSemaphoreTake(g_console_act_semaphore, (TickType_t) 10) != pdTRUE){
		return false;
	}
	return true;
}
bool release_console_semaphore(){
	// already free
	if(is_console_semaphore_free()){
		return true;
	}

	if(xSemaphoreGive(g_console_act_semaphore) != pdTRUE){
		return false;
	}

	return true;
}
bool is_console_semaphore_free(){
	return is_g_console_act_semaphore_free();
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

void operate_by_buttons(volatile mode_of_operation_def_t mode,volatile Button_operations_def_t *o){
	// if configuration by console, do nothing
	if(!is_console_semaphore_free()){
		return;
	}

	// if operation needed, do it by single button and jump out
	switch (mode) {
		case MODE_CONF:
			if(o->btn1){
				active_parameter = active_parameter == PARAM_KI ? PARAM_KP : PARAM_KI;
				printed = false;
				o->done = true;
				return;
			}
			if (o->btn3) {
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
				return;
			}
			if (o->btn2) {
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
				return;
			}
			if (o->btn0) {
				mode_of_operation = MODE_IDLE;
				printed = false;
				o->done = true;
				return;
			}
			break;
		case MODE_IDLE:
			if(o->btn0){
				mode_of_operation = MODE_MODULATION;
				printed = false;
				o->done = true;
			}
			break;
		case MODE_MODULATION:
			if (o->btn3) {
				increase_sp();
				return;
			}
			if (o->btn2) {
				decrease_sp();
				return;
			}
			if (o->btn0) {
				mode_of_operation = MODE_CONF;
				printed = false;
				o->done = true;
				return;
			}
		default:
			break;
	}
}

void check_messages(){

	char rx = uart_receive();

	bool sem_is_free = is_console_semaphore_free();

	if(mode_of_operation == MODE_CONF && !sem_is_free){
		if(rx == '*'){
			active_parameter = active_parameter == PARAM_KI ? PARAM_KP : PARAM_KI;
			printed = false;
		}
		if(rx == '-'){
			switch (active_parameter) {
				case PARAM_KI:
					decrease_ki();
					print_parameter_value();
					break;
				case PARAM_KP:
					decrease_kp();
					print_parameter_value();
				default:
					break;
			}
		}
		if(rx == '+'){
			switch (active_parameter) {
			case PARAM_KI:
				increase_ki();
				print_parameter_value();
				break;
			case PARAM_KP:
				increase_kp();
				print_parameter_value();
			default:
				break;
			}
		}
	}

	if(mode_of_operation == MODE_MODULATION){
			if(rx == '+'){
				increase_sp();
			}
			if(rx == '-'){
				decrease_sp();
			}
		}


	if(rx == '0'){
		if(sem_is_free){
			take_console_semaphore();

		}
		if(mode_of_operation != MODE_CONF){
			mode_of_operation = MODE_CONF;
			printed = false;
		}


	}
	if(rx == '1'){
		if(mode_of_operation != MODE_IDLE){
			mode_of_operation = MODE_IDLE;
			printed = false;
		}
		if(!sem_is_free){
			release_console_semaphore();

		}
	}
	if(rx == '2'){
		if(mode_of_operation != MODE_MODULATION){
			mode_of_operation = MODE_MODULATION;
			printed = false;
		}
		if(!sem_is_free){
			release_console_semaphore();

		}
	}
}
