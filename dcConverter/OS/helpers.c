#include "OS_includes.h"

/*
 * Global functions, returns the semaphore state free or not
 */

bool is_g_modulation_semaphore_free(){
	if(uxSemaphoreGetCount(g_disable_modulation_semaphore) != 1){
		return false;
	}
	return true;
}

bool is_g_console_act_semaphore_free(){
	if(uxSemaphoreGetCount(g_console_act_semaphore) != 1){
		return false;
	}
	return true;
}

bool is_g_button_act_semaphore_free(){
	if(uxSemaphoreGetCount(g_button_act_semaphore) != 1){
		return false;
	}
	return true;
}

/*
 *  Global functions for uart communication
 */

void uart_send(char c) {
  while (UART_STATUS & XUARTPS_SR_TNFUL);
  UART_FIFO = c;
  while (UART_STATUS & XUARTPS_SR_TACTIVE);
}

void uart_send_string(char str[BUFFER_SIZE]) {
  char *ptr = str;
  while (*ptr != '\0') {
    uart_send(*ptr);
    ptr++;
  }
}
char uart_receive() {
  if ((UART_STATUS & XUARTPS_SR_RXEMPTY) == XUARTPS_SR_RXEMPTY) return 0;
  return UART_FIFO;
}
