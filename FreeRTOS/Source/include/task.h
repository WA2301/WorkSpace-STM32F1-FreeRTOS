/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved
*/


#ifndef INC_TASK_H
#define INC_TASK_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h must appear in source files before include task.h"
#endif

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/

#define tskKERNEL_VERSION_NUMBER "V8.2.3"
#define tskKERNEL_VERSION_MAJOR 8
#define tskKERNEL_VERSION_MINOR 2
#define tskKERNEL_VERSION_BUILD 3

/**
 * task. h
 *
 * Type by which tasks are referenced.  For example, a call to xTaskCreate
 * returns (via a pointer parameter) an TaskHandle_t variable that can then
 * be used as a parameter to vTaskDelete to delete the task.
 *
 * \defgroup TaskHandle_t TaskHandle_t
 * \ingroup Tasks
 */
typedef void * TaskHandle_t;

/*
 * Defines the prototype to which the application task hook function must
 * conform.
 */
typedef BaseType_t (*TaskHookFunction_t)( void * );

/* Task states returned by eTaskGetState. */
typedef enum
{
	eRunning = 0,	/* A task is querying the state of itself, so must be running. */
	eReady,			/* The task being queried is in a read or pending ready list. */
	eBlocked,		/* The task being queried is in the Blocked state. */
	eSuspended,		/* The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
	eDeleted		/* The task being queried has been deleted, but its TCB has not yet been freed. */
} eTaskState;

/* Actions that can be performed when vTaskNotify() is called. */
typedef enum
{
	eNoAction = 0,				/* Notify the task without updating its notify value. */
	eSetBits,					/* Set bits in the task's notification value. */
	eIncrement,					/* Increment the task's notification value. */
	eSetValueWithOverwrite,		/* Set the task's notification value to a specific value even if the previous value has not yet been read by the task. */
	eSetValueWithoutOverwrite	/* Set the task's notification value if the previous value has been read by the task. */
} eNotifyAction;

/*
 * Used internally only.
 */
typedef struct xTIME_OUT
{
	BaseType_t xOverflowCount;
	TickType_t xTimeOnEntering;
} TimeOut_t;

/*
 * Defines the memory ranges allocated to the task when an MPU is used.
 */
typedef struct xMEMORY_REGION
{
	void *pvBaseAddress;
	uint32_t ulLengthInBytes;
	uint32_t ulParameters;
} MemoryRegion_t;

/*
 * Parameters required to create an MPU protected task.
 */
typedef struct xTASK_PARAMETERS
{
	TaskFunction_t pvTaskCode;
	const char * const pcName;	/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	uint16_t usStackDepth;
	void *pvParameters;
	UBaseType_t uxPriority;
	StackType_t *puxStackBuffer;
	MemoryRegion_t xRegions[ portNUM_CONFIGURABLE_REGIONS ];
} TaskParameters_t;

/* Used with the uxTaskGetSystemState() function to return the state of each task
in the system. */
typedef struct xTASK_STATUS
{
	TaskHandle_t xHandle;			/* The handle of the task to which the rest of the information in the structure relates. */
//	const char *pcTaskName;			/* A pointer to the task's name.  This value will be invalid if the task was deleted since the structure was populated! */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	UBaseType_t xTaskNumber;		/* A number unique to the task. */
	eTaskState eCurrentState;		/* The state in which the task existed when the structure was populated. */
	UBaseType_t uxCurrentPriority;	/* The priority at which the task was running (may be inherited) when the structure was populated. */
	UBaseType_t uxBasePriority;		/* The priority to which the task will return if the task's current priority has been inherited to avoid unbounded priority inversion when obtaining a mutex.  Only valid if configUSE_MUTEXES is defined as 1 in FreeRTOSConfig.h. */
	uint32_t ulRunTimeCounter;		/* The total run time allocated to the task so far, as defined by the run time stats clock.  See http://www.freertos.org/rtos-run-time-stats.html.  Only valid when configGENERATE_RUN_TIME_STATS is defined as 1 in FreeRTOSConfig.h. */
	uint16_t usStackHighWaterMark;	/* The minimum amount of stack space that has remained for the task since the task was created.  The closer this value is to zero the closer the task has come to overflowing its stack. */
} TaskStatus_t;

/* Possible return values for eTaskConfirmSleepModeStatus(). */
typedef enum
{
	eAbortSleep = 0,		/* A task has been made ready or a context switch pended since portSUPPORESS_TICKS_AND_SLEEP() was called - abort entering a sleep mode. */
	eStandardSleep,			/* Enter a sleep mode that will not last any longer than the expected idle time. */
	eNoTasksWaitingTimeout	/* No tasks are waiting for a timeout so it is safe to enter a sleep mode that can only be exited by an external interrupt. */
} eSleepModeStatus;


/**
 * Defines the priority used by the idle task.  This must not be modified.
 *
 * \ingroup TaskUtils
 */
#define tskIDLE_PRIORITY			( ( UBaseType_t ) 0U )

/**
 * task. h
 *
 * Macro for forcing a context switch.
 *
 * \defgroup taskYIELD taskYIELD
 * \ingroup SchedulerControl
 */
#define taskYIELD()					portYIELD()

/**
 * task. h
 *
 * Macro to mark the start of a critical code region.  Preemptive context
 * switches cannot occur when in a critical region.
 *
 * NOTE: This may alter the stack (depending on the portable implementation)
 * so must be used with care!
 *
 * \defgroup taskENTER_CRITICAL taskENTER_CRITICAL
 * \ingroup SchedulerControl
 */
#define taskENTER_CRITICAL()		portENTER_CRITICAL()
#define taskENTER_CRITICAL_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()

/**
 * task. h
 *
 * Macro to mark the end of a critical code region.  Preemptive context
 * switches cannot occur when in a critical region.
 *
 * NOTE: This may alter the stack (depending on the portable implementation)
 * so must be used with care!
 *
 * \defgroup taskEXIT_CRITICAL taskEXIT_CRITICAL
 * \ingroup SchedulerControl
 */
#define taskEXIT_CRITICAL()			portEXIT_CRITICAL()
#define taskEXIT_CRITICAL_FROM_ISR( x ) portCLEAR_INTERRUPT_MASK_FROM_ISR( x )
/**
 * task. h
 *
 * Macro to disable all maskable interrupts.
 *
 * \defgroup taskDISABLE_INTERRUPTS taskDISABLE_INTERRUPTS
 * \ingroup SchedulerControl
 */
#define taskDISABLE_INTERRUPTS()	portDISABLE_INTERRUPTS()

/**
 * task. h
 *
 * Macro to enable microcontroller interrupts.
 *
 * \defgroup taskENABLE_INTERRUPTS taskENABLE_INTERRUPTS
 * \ingroup SchedulerControl
 */
#define taskENABLE_INTERRUPTS()		portENABLE_INTERRUPTS()

/* Definitions returned by xTaskGetSchedulerState().  taskSCHEDULER_SUSPENDED is
0 to generate more optimal code when configASSERT() is defined as the constant
is used in assert() statements. */
#define taskSCHEDULER_SUSPENDED		( ( BaseType_t ) 0 )
#define taskSCHEDULER_NOT_STARTED	( ( BaseType_t ) 1 )
#define taskSCHEDULER_RUNNING		( ( BaseType_t ) 2 )


#define xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask ) xTaskGenericCreate( ( pvTaskCode ), ( pcName ), ( usStackDepth ), ( pvParameters ), ( uxPriority ), ( pxCreatedTask ), ( NULL ), ( NULL ) )

#define xTaskCreateRestricted( x, pxCreatedTask ) xTaskGenericCreate( ((x)->pvTaskCode), ((x)->pcName), ((x)->usStackDepth), ((x)->pvParameters), ((x)->uxPriority), (pxCreatedTask), ((x)->puxStackBuffer), ((x)->xRegions) )


void vTaskDelay( const TickType_t xTicksToDelay ) PRIVILEGED_FUNCTION;

void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement ) PRIVILEGED_FUNCTION;

void vTaskStartScheduler( void ) PRIVILEGED_FUNCTION;



void vTaskSuspendAll( void ) PRIVILEGED_FUNCTION;

BaseType_t xTaskResumeAll( void ) PRIVILEGED_FUNCTION;


BaseType_t xTaskGenericNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue ) PRIVILEGED_FUNCTION;
#define xTaskNotify( xTaskToNotify, ulValue, eAction ) xTaskGenericNotify( ( xTaskToNotify ), ( ulValue ), ( eAction ), NULL )
#define xTaskNotifyAndQuery( xTaskToNotify, ulValue, eAction, pulPreviousNotifyValue ) xTaskGenericNotify( ( xTaskToNotify ), ( ulValue ), ( eAction ), ( pulPreviousNotifyValue ) )

BaseType_t xTaskGenericNotifyFromISR( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue, BaseType_t *pxHigherPriorityTaskWoken ) PRIVILEGED_FUNCTION;
#define xTaskNotifyFromISR( xTaskToNotify, ulValue, eAction, pxHigherPriorityTaskWoken ) xTaskGenericNotifyFromISR( ( xTaskToNotify ), ( ulValue ), ( eAction ), NULL, ( pxHigherPriorityTaskWoken ) )
#define xTaskNotifyAndQueryFromISR( xTaskToNotify, ulValue, eAction, pulPreviousNotificationValue, pxHigherPriorityTaskWoken ) xTaskGenericNotifyFromISR( ( xTaskToNotify ), ( ulValue ), ( eAction ), ( pulPreviousNotificationValue ), ( pxHigherPriorityTaskWoken ) )

BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

#define xTaskNotifyGive( xTaskToNotify ) xTaskGenericNotify( ( xTaskToNotify ), ( 0 ), eIncrement, NULL )

void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken ) PRIVILEGED_FUNCTION;

uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

BaseType_t xTaskNotifyStateClear( TaskHandle_t xTask );

BaseType_t xTaskIncrementTick( void ) PRIVILEGED_FUNCTION;

/*
 * Generic version of the task creation function which is in turn called by the
 * xTaskCreate() and xTaskCreateRestricted() macros.
 */
BaseType_t xTaskGenericCreate( TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask, StackType_t * const puxStackBuffer, const MemoryRegion_t * const xRegions ) PRIVILEGED_FUNCTION; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */


#ifdef __cplusplus
}
#endif
#endif /* INC_TASK_H */



