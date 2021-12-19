#ifndef OS_INCLUDES_H
#define OS_INCLUDES_H


#include "OS_setup.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define BUFFER_SIZE 100

#include <stdbool.h>
#include <xuartps_hw.h>
#include "zynq_registers.h"

// ki, kp and setpoint parameters for PI
volatile float *g_ki_param_ptr;
volatile float *g_kp_param_ptr;
volatile float *g_sp_param_ptr;

// semaphore for handling the modulation on/off state
volatile SemaphoreHandle_t g_disable_modulation_semaphore;
bool is_g_modulation_semaphore_free(void);

// semaphore for handling the console configuration activity (button disablement)
volatile SemaphoreHandle_t g_console_act_semaphore;
bool is_g_console_act_semaphore_free(void);

// semaphore for handling the button activity (console disablement)
volatile SemaphoreHandle_t g_button_act_semaphore;
bool is_g_button_act_semaphore_free(void);

// uart communication functions
void uart_send(char c);
void uart_send_string(char str[BUFFER_SIZE]);
char uart_receive(void);

#endif
