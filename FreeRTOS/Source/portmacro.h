/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved
*/


//#ifndef PORTMACRO_H
//#define PORTMACRO_H

//#ifdef __cplusplus
//extern "C" {
//#endif

///*-----------------------------------------------------------
// * Port specific definitions.
// *
// * The settings in this file configure FreeRTOS correctly for the
// * given hardware and compiler.
// *
// * These settings should not be altered.
// *-----------------------------------------------------------
// */

///* Type definitions. */
//#define portCHAR		char
//#define portFLOAT		float
//#define portDOUBLE		double
//#define portLONG		long
//#define portSHORT		short
//#define portSTACK_TYPE	uint32_t
//#define portBASE_TYPE	long

//typedef portSTACK_TYPE StackType_t;
//typedef long BaseType_t;
//typedef unsigned long UBaseType_t;


//	typedef uint32_t TickType_t;
//	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

//	/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
//	not need to be guarded with a critical section. */
//	#define portTICK_TYPE_IS_ATOMIC 1

///*-----------------------------------------------------------*/

///* Architecture specifics. */
//#define portSTACK_GROWTH			( -1 )
////#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
//#define portBYTE_ALIGNMENT			8
///*-----------------------------------------------------------*/

///* Scheduler utilities. */
//extern void vPortYield( void );
//#define portNVIC_INT_CTRL_REG		( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
//#define portNVIC_PENDSVSET_BIT		( 1UL << 28UL )
///*-----------------------------------------------------------*/

///* Critical section management. */
//extern uint32_t ulPortSetInterruptMask( void );
//extern void vPortClearInterruptMask( uint32_t ulNewMask );
//extern void vPortEnterCritical( void );
//extern void vPortExitCritical( void );
///*-----------------------------------------------------------*/


//#ifdef __cplusplus
//}
//#endif

//#endif /* PORTMACRO_H */

