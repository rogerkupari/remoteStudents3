#ifndef OS_SETUP_H
#define OS_SETUP_H

#include <stdint.h>

/* Timer configuration settings. */
typedef struct
{
	uint32_t OutputHz;	/* Output frequency. */
	uint16_t Interval;	/* Interval value. */
	uint8_t Prescaler;	/* Prescaler value. */
	uint16_t Options;	/* Option settings. */
} TmrCntrSetup;

void initialize_tasks(void);
void middleware_init(void);
void hardware_init(void);
void OS_setup(void);
void vInitialiseInterruptTimer( void );


#endif
