/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved
*/

#ifndef INC_FREERTOS_H
#define INC_FREERTOS_H

/*
 * Include the generic headers required for the FreeRTOS port being used.
 */
#include <stddef.h>

/*
 * If stdint.h cannot be located then:
 *   + If using GCC ensure the -nostdint options is *not* being used.
 *   + Ensure the project's include path includes the directory in which your
 *     compiler stores stdint.h.
 *   + Set any compiler options necessary for it to support C99, as technically
 *     stdint.h is only mandatory with C99 (FreeRTOS does not require C99 in any
 *     other way).
 *   + The FreeRTOS download includes a simple stdint.h definition that can be
 *     used in cases where none is provided by the compiler.  The files only
 *     contains the typedefs required to build FreeRTOS.  Read the instructions
 *     in FreeRTOS/source/stdint.readme for more information.
 */
#include <stdint.h> /* READ COMMENT ABOVE. */

#ifdef __cplusplus
extern "C" {
#endif

/* Application specific configuration options. */
#include "FreeRTOSConfig.h"

///* Basic FreeRTOS definitions. */
//#include "projdefs.h"

/*
 * Defines the prototype to which task functions must conform.  Defined in this
 * file to ensure the type is known before portable.h is included.
 */
typedef void (*TaskFunction_t)( void * );

#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )

#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )


/* FreeRTOS error definitions. */
#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY	( -1 )
    
    
    
    
    
/* Definitions specific to the port being used. */
#include "portable.h"

/*
 * Check all the required application specific macros have been defined.
 * These macros are application specific and (as downloaded) are defined
 * within FreeRTOSConfig.h.
 */

#ifndef configMINIMAL_STACK_SIZE
	#error Missing definition:  configMINIMAL_STACK_SIZE must be defined in FreeRTOSConfig.h.  configMINIMAL_STACK_SIZE defines the size (in words) of the stack allocated to the idle task.  Refer to the demo project provided for your port for a suitable value.
#endif

#ifndef configMAX_PRIORITIES
	#error Missing definition:  configMAX_PRIORITIES must be defined in FreeRTOSConfig.h.  See the Configuration section of the FreeRTOS API documentation for details.
#endif

#ifndef configUSE_PREEMPTION
	#error Missing definition:  configUSE_PREEMPTION must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif

//#ifndef configUSE_IDLE_HOOK
//	#error Missing definition:  configUSE_IDLE_HOOK must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
//#endif

//#ifndef configUSE_TICK_HOOK
//	#error Missing definition:  configUSE_TICK_HOOK must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
//#endif

//#ifndef INCLUDE_vTaskPrioritySet
//	#error Missing definition:  INCLUDE_vTaskPrioritySet must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
//#endif

//#ifndef INCLUDE_uxTaskPriorityGet
//	#error Missing definition:  INCLUDE_uxTaskPriorityGet must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
//#endif

#ifndef INCLUDE_vTaskDelete
	#error Missing definition:  INCLUDE_vTaskDelete must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif

#ifndef INCLUDE_vTaskSuspend
	#error Missing definition:  INCLUDE_vTaskSuspend must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif

#ifndef INCLUDE_vTaskDelayUntil
	#error Missing definition:  INCLUDE_vTaskDelayUntil must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif

#ifndef INCLUDE_vTaskDelay
	#error Missing definition:  INCLUDE_vTaskDelay must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif

#ifndef configUSE_16_BIT_TICKS
	#error Missing definition:  configUSE_16_BIT_TICKS must be defined in FreeRTOSConfig.h as either 1 or 0.  See the Configuration section of the FreeRTOS API documentation for details.
#endif

#ifndef configMAX_PRIORITIES
	#error configMAX_PRIORITIES must be defined to be greater than or equal to 1.
#endif

#ifndef configUSE_CO_ROUTINES
	#define configUSE_CO_ROUTINES 0
#endif

#if configUSE_CO_ROUTINES != 0
	#ifndef configMAX_CO_ROUTINE_PRIORITIES
		#error configMAX_CO_ROUTINE_PRIORITIES must be greater than or equal to 1.
	#endif
#endif


#ifndef portSET_INTERRUPT_MASK_FROM_ISR
	#define portSET_INTERRUPT_MASK_FROM_ISR() 0
#endif

#ifndef portCLEAR_INTERRUPT_MASK_FROM_ISR
	#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedStatusValue ) ( void ) uxSavedStatusValue
#endif

#ifndef portCLEAN_UP_TCB
	#define portCLEAN_UP_TCB( pxTCB ) ( void ) pxTCB
#endif

#ifndef portPRE_TASK_DELETE_HOOK
	#define portPRE_TASK_DELETE_HOOK( pvTaskToDelete, pxYieldPending )
#endif

#ifndef portSETUP_TCB
	#define portSETUP_TCB( pxTCB ) ( void ) pxTCB
#endif

//#ifndef configQUEUE_REGISTRY_SIZE
//	#define configQUEUE_REGISTRY_SIZE 0U
//#endif

//#if ( configQUEUE_REGISTRY_SIZE < 1 )
//	#define vQueueAddToRegistry( xQueue, pcName )
//	#define vQueueUnregisterQueue( xQueue )
//#endif

#ifndef portPOINTER_SIZE_TYPE
	#define portPOINTER_SIZE_TYPE uint32_t
#endif


//#ifndef configCHECK_FOR_STACK_OVERFLOW
//	#define configCHECK_FOR_STACK_OVERFLOW 0
//#endif


//#ifndef configGENERATE_RUN_TIME_STATS
//	#define configGENERATE_RUN_TIME_STATS 0
//#endif

//#if ( configGENERATE_RUN_TIME_STATS == 1 )

//	#ifndef portCONFIGURE_TIMER_FOR_RUN_TIME_STATS
//		#error If configGENERATE_RUN_TIME_STATS is defined then portCONFIGURE_TIMER_FOR_RUN_TIME_STATS must also be defined.  portCONFIGURE_TIMER_FOR_RUN_TIME_STATS should call a port layer function to setup a peripheral timer/counter that can then be used as the run time counter time base.
//	#endif /* portCONFIGURE_TIMER_FOR_RUN_TIME_STATS */

//	#ifndef portGET_RUN_TIME_COUNTER_VALUE
//		#ifndef portALT_GET_RUN_TIME_COUNTER_VALUE
//			#error If configGENERATE_RUN_TIME_STATS is defined then either portGET_RUN_TIME_COUNTER_VALUE or portALT_GET_RUN_TIME_COUNTER_VALUE must also be defined.  See the examples provided and the FreeRTOS web site for more information.
//		#endif /* portALT_GET_RUN_TIME_COUNTER_VALUE */
//	#endif /* portGET_RUN_TIME_COUNTER_VALUE */

//#endif /* configGENERATE_RUN_TIME_STATS */


#ifndef portPRIVILEGE_BIT
	#define portPRIVILEGE_BIT ( ( UBaseType_t ) 0x00 )
#endif

#ifndef portYIELD_WITHIN_API
	#define portYIELD_WITHIN_API portYIELD
#endif

#ifndef pvPortMallocAligned
	#define pvPortMallocAligned( x, puxStackBuffer ) ( ( ( puxStackBuffer ) == NULL ) ? ( pvPortMalloc( ( x ) ) ) : ( puxStackBuffer ) )
#endif

#ifndef vPortFreeAligned
	#define vPortFreeAligned( pvBlockToFree ) vPortFree( pvBlockToFree )
#endif

#ifndef portSUPPRESS_TICKS_AND_SLEEP
	#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime )
#endif

#ifndef configEXPECTED_IDLE_TIME_BEFORE_SLEEP
	#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP 2
#endif

#if configEXPECTED_IDLE_TIME_BEFORE_SLEEP < 2
	#error configEXPECTED_IDLE_TIME_BEFORE_SLEEP must not be less than 2
#endif


#ifndef configUSE_TIME_SLICING
	#define configUSE_TIME_SLICING 1
#endif

#ifndef configAPPLICATION_ALLOCATED_HEAP
	#define configAPPLICATION_ALLOCATED_HEAP 0
#endif

//#ifndef configUSE_TASK_NOTIFICATIONS
//	#define configUSE_TASK_NOTIFICATIONS 1
//#endif

//#ifndef portTICK_TYPE_IS_ATOMIC
//	#define portTICK_TYPE_IS_ATOMIC 0
//#endif

#if( portTICK_TYPE_IS_ATOMIC == 0 )
	/* Either variables of tick type cannot be read atomically, or
	portTICK_TYPE_IS_ATOMIC was not set - map the critical sections used when
	the tick count is returned to the standard critical section macros. */
//	#define portTICK_TYPE_ENTER_CRITICAL() portENTER_CRITICAL()
//	#define portTICK_TYPE_EXIT_CRITICAL() portEXIT_CRITICAL()
//	#define portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()
//	#define portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( x ) portCLEAR_INTERRUPT_MASK_FROM_ISR( ( x ) )
#else
	/* The tick type can be read atomically, so critical sections used when the
	tick count is returned can be defined away. */
//	#define portTICK_TYPE_ENTER_CRITICAL()
//	#define portTICK_TYPE_EXIT_CRITICAL()
//	#define portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR() 0
//	#define portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( x ) ( void ) x
#endif

/* Definitions to allow backward compatibility with FreeRTOS versions prior to
V8 if desired. */
#ifndef configENABLE_BACKWARD_COMPATIBILITY
	#define configENABLE_BACKWARD_COMPATIBILITY 1
#endif

#if configENABLE_BACKWARD_COMPATIBILITY == 1
	#define eTaskStateGet eTaskGetState
	#define portTickType TickType_t
	#define xTaskHandle TaskHandle_t
	#define xQueueHandle QueueHandle_t
	#define xSemaphoreHandle SemaphoreHandle_t
	#define xQueueSetHandle QueueSetHandle_t
	#define xQueueSetMemberHandle QueueSetMemberHandle_t
	#define xTimeOutType TimeOut_t
	#define xMemoryRegion MemoryRegion_t
	#define xTaskParameters TaskParameters_t
	#define xTaskStatusType	TaskStatus_t
	#define xTimerHandle TimerHandle_t
	#define xCoRoutineHandle CoRoutineHandle_t
	#define pdTASK_HOOK_CODE TaskHookFunction_t
	#define portTICK_RATE_MS portTICK_PERIOD_MS

	/* Backward compatibility within the scheduler code only - these definitions
	are not really required but are included for completeness. */
	#define tmrTIMER_CALLBACK TimerCallbackFunction_t
	#define pdTASK_CODE TaskFunction_t
	#define xListItem ListItem_t
	#define xList List_t
#endif /* configENABLE_BACKWARD_COMPATIBILITY */

/* Set configUSE_TASK_FPU_SUPPORT to 0 to omit floating point support even
if floating point hardware is otherwise supported by the FreeRTOS port in use.
This constant is not supported by all FreeRTOS ports that include floating 
point support. */
#ifndef configUSE_TASK_FPU_SUPPORT
	#define configUSE_TASK_FPU_SUPPORT 1
#endif

#ifdef __cplusplus
}
#endif

#endif /* INC_FREERTOS_H */

