#ifndef OS_INCLUDES_H
#define OS_INCLUDES_H


#include "OS_setup.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"



#include <stdbool.h>

volatile float *g_ki_param_ptr;
volatile float *g_kp_param_ptr;
volatile float *g_sp_param_ptr;

volatile SemaphoreHandle_t g_disable_modulation_semaphore;
bool is_g_modulation_semaphore_free(void);

#endif
