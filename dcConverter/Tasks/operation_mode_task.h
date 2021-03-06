#ifndef OPERATION_MODE_TASK_H
#define OPERATION_MODE_TASK_H

#include "zynq_registers.h"
#include <stdbool.h>

#define K_PARAMETER_CHANGE 1.00f
#define SP_PARAMETER_CHANGE 0.10f

#define BUTTON_PREVENT_UART_TIME_SEC 5
#define BUTTON_PREVENT_UART_TICS 	BUTTON_PREVENT_UART_TIME_SEC*1000UL


typedef struct {
	uint32_t btn0:1; //
	uint32_t btn1:1;
	uint32_t btn2:1;
	uint32_t btn3:1;
	bool done; // operation done
} Button_operations_def_t;

typedef enum {
	MODE_CONF,
	MODE_IDLE,
	MODE_MODULATION
} mode_of_operation_def_t;

typedef enum {
	PARAM_KI,
	PARAM_KP
} active_parameter_def_t;

/*
 *  *o parameter = button operations
 */

void operation_mode_task(void *params);
void check_mode_and_buttons(volatile Button_operations_def_t *o);
void read_buttons(volatile Button_operations_def_t *o);
void print_message(int mode);
void led_indications(volatile mode_of_operation_def_t *mode);

void increase_ki(void);
void decrease_ki(void);
void increase_kp(void);
void decrease_kp(void);
void increase_sp(void);
void decrease_sp(void);
void print_parameter_value(void);

bool take_modulation_semaphore(void);
bool release_modulation_semaphore(void);
bool is_modulation_semaphore_free(void);

bool take_console_semaphore(void);
bool release_console_semaphore(void);
bool is_console_semaphore_free(void);

bool take_button_semaphore(void);
bool release_button_semaphore(void);
bool is_button_semaphore_free(void);

void operate_by_buttons(volatile mode_of_operation_def_t mode, volatile Button_operations_def_t *o);
void check_uart_messages(void);



#endif
