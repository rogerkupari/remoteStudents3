#ifndef OPERATION_MODE_TASK_H
#define OPERATION_MODE_TASK_H

#include "zynq_registers.h"
#include <stdbool.h>

#define K_PARAMETER_CHANGE 0.05f
#define SP_PARAMETER_CHANGE 1.00f


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

bool take_semaphore(void);
bool release_semaphore(void);
bool is_semaphore_free(void);



#endif
