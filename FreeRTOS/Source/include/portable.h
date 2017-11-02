/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved
*/

/*-----------------------------------------------------------
 * Portable layer API.  Each function must be defined for each port.
 *----------------------------------------------------------*/

#ifndef PORTABLE_H
#define PORTABLE_H

/* If portENTER_CRITICAL is not defined then including deprecated_definitions.h
did not result in a portmacro.h header file being included - and it should be
included here.  In this case the path to the correct portmacro.h header file
must be set in the compiler's include path. */
#ifndef portENTER_CRITICAL
	#include "portmacro.h"
#endif


#if portBYTE_ALIGNMENT == 8
	#define portBYTE_ALIGNMENT_MASK ( 0x0007 )
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
	StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters ) ;

    


/*
 * Map to the memory management routines required for the port.
 */
void *pvPortMalloc( size_t xSize ) ;
void vPortFree( void *pv ) ;
//void vPortInitialiseBlocks( void ) ;
size_t xPortGetFreeHeapSize( void ) ;
//size_t xPortGetMinimumEverFreeHeapSize( void ) ;

/*
 * Setup the hardware ready for the scheduler to take control.  This generally
 * sets up a tick interrupt and sets timers for the correct tick frequency.
 */
BaseType_t xPortStartScheduler( void ) ;


#ifdef __cplusplus
}
#endif

#endif /* PORTABLE_H */

