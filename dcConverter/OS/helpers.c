#include "OS_includes.h"

bool is_g_modulation_semaphore_free(void){
	if(uxSemaphoreGetCount(g_disable_modulation_semaphore) != 1){
		return false;
	}
	return true;
}
