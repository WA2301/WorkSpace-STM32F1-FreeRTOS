/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    1 tab == 4 spaces!
*/

/* Standard includes. */
#include <stdlib.h>

/* FreeRTOS includes. */
#include "task.h"


/*
 * Defines the size, in words, of the stack allocated to the idle task.
 */
#define tskIDLE_STACK_SIZE	( ( unsigned short ) 130 )//Min size


/*
 * Task control block.  A task control block (TCB) is allocated for each task,
 * and stores task state information, including a pointer to the task's context
 * (the task's run time environment, including register values)
 */
typedef struct tskTaskControlBlock
{
	volatile StackType_t	*pxTopOfStack;	/*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */

	ListItem_t			xGenericListItem;	/*< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
	ListItem_t			xEventListItem;		/*< Used to reference a task from an event list. */
	UBaseType_t			uxPriority;			/*< The priority of the task.  0 is the lowest priority. */
	StackType_t			*pxStack;			/*< Points to the start of the stack. */

} tskTCB;

/* The old tskTCB name is maintained above then typedefed to the new TCB_t name
below to enable the use of older kernel aware debuggers. */
typedef tskTCB TCB_t;

 TCB_t * volatile pxCurrentTCB = NULL;

/* Lists for ready and blocked tasks. --------------------*/
 static List_t pxReadyTasksLists[ configMAX_PRIORITIES ];/*< Prioritised ready tasks. */
 static List_t xDelayedTaskList1;						/*< Delayed tasks. */
 static List_t xDelayedTaskList2;						/*< Delayed tasks (two lists are used - one for delays that have overflowed the current tick count. */
 static List_t * volatile pxDelayedTaskList;				/*< Points to the delayed task list currently being used. */
 static List_t * volatile pxOverflowDelayedTaskList;		/*< Points to the delayed task list currently being used to hold tasks that have overflowed the current tick count. */
 static List_t xPendingReadyList;						/*< Tasks that have been readied while the scheduler was suspended.  They will be moved to the ready list when the scheduler is resumed. */



/* Other file private variables. --------------------------------*/
 static volatile UBaseType_t uxCurrentNumberOfTasks 	= ( UBaseType_t ) 0U;
 static volatile TickType_t xTickCount 				= ( TickType_t ) 0U;
 static volatile UBaseType_t uxTopReadyPriority 		= 0;
 static volatile BaseType_t xSchedulerRunning 		= pdFALSE;
 static volatile UBaseType_t uxPendedTicks 			= ( UBaseType_t ) 0U;
 static volatile BaseType_t xYieldPending 			= pdFALSE;
 static volatile BaseType_t xNumOfOverflows 			= ( BaseType_t ) 0;
 static UBaseType_t uxTaskNumber 					= ( UBaseType_t ) 0U;
 static volatile TickType_t xNextTaskUnblockTime		= ( TickType_t ) 0U; /* Initialised to portMAX_DELAY before the scheduler starts. */

/* Context switches are held pending while the scheduler is suspended.  Also,
interrupts must not manipulate the xGenericListItem of a TCB, or any of the
lists the xGenericListItem can be referenced from, if the scheduler is suspended.
If an interrupt needs to unblock a task while the scheduler is suspended then it
moves the task's event list item into the xPendingReadyList, ready for the
kernel to move the task from the pending ready list into the real ready list
when the scheduler is unsuspended.  The pending ready list itself can only be
accessed from a critical section. */
 static volatile UBaseType_t uxSchedulerSuspended	= ( UBaseType_t ) pdFALSE;

/*-----------------------------------------------------------*/

/*
 * Place the task represented by pxTCB into the appropriate ready list for
 * the task.  It is inserted at the end of the list.
 */
#define prvAddTaskToReadyList( pxTCB )																\
	/*traceMOVED_TASK_TO_READY_STATE( pxTCB );	*/													\
	/*taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority );										 **/ \
    ( uxTopReadyPriority ) |= ( 1UL << ( ( pxTCB )->uxPriority ) );                                 \
	vListInsertEnd( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), &( ( pxTCB )->xGenericListItem ) )
/*-----------------------------------------------------------*/

	#define taskEVENT_LIST_ITEM_VALUE_IN_USE	0x80000000UL

/* File private functions. --------------------------------*/

/*
 * Utility to ready a TCB for a given task.  Mainly just copies the parameters
 * into the TCB structure.
 */
static void prvInitialiseTCBVariables( TCB_t * const pxTCB, const char * const pcName, UBaseType_t uxPriority, const MemoryRegion_t * const xRegions, const uint16_t usStackDepth ) ; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

/*
 * Utility to ready all the lists used by the scheduler.  This is called
 * automatically upon the creation of the first task.
 */
static void prvInitialiseTaskLists( void ) ;

/*
 * The idle task, which as all tasks is implemented as a never ending loop.
 * The idle task is automatically created and added to the ready lists upon
 * creation of the first user task.
 *
 * The portTASK_FUNCTION_PROTO() macro is used to allow port/compiler specific
 * language extensions.  The equivalent prototype for this function is:
 *
 * void prvIdleTask( void *pvParameters );
 *
 */
//static portTASK_FUNCTION_PROTO( prvIdleTask, pvParameters );//等效替换
static void prvIdleTask( void *pvParameters );

/*
 * The currently executing task is entering the Blocked state.  Add the task to
 * either the current or the overflow delayed task list.
 */
static void prvAddCurrentTaskToDelayedList( const TickType_t xTimeToWake ) ;

/*
 * Allocates memory from the heap for a TCB and associated stack.  Checks the
 * allocation was successful.
 */
static TCB_t *prvAllocateTCBAndStack( const uint16_t usStackDepth, StackType_t * const puxStackBuffer ) ;

/*
 * Set xNextTaskUnblockTime to the time at which the next Blocked state task
 * will exit the Blocked state.
 */
static void prvResetNextTaskUnblockTime( void );

/*-----------------------------------------------------------*/

BaseType_t xTaskGenericCreate( TaskFunction_t pxTaskCode, \
                               const uint16_t usStackDepth, \
                               UBaseType_t uxPriority ) /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
{
BaseType_t xReturn;
TCB_t * pxNewTCB;
StackType_t *pxTopOfStack;

	/* Allocate the memory required by the TCB and stack for the new task,
	checking that the allocation was successful. */
	pxNewTCB = prvAllocateTCBAndStack( usStackDepth, NULL );

	if( pxNewTCB != NULL )
	{
		/* Calculate the top of stack address.  This depends on whether the
		stack grows from high memory to low (as per the 80x86) or vice versa.
		portSTACK_GROWTH is used to make the result positive or negative as
		required by the port. */
//		#if( portSTACK_GROWTH < 0 )
//		{
			pxTopOfStack = pxNewTCB->pxStack + ( usStackDepth - ( uint16_t ) 1 );
			pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) ); 
        /*lint !e923 MISRA exception.  Avoiding casts between pointers and integers is not practical.  Size differences accounted for using portPOINTER_SIZE_TYPE type. */
//        }
//		#endif /* portSTACK_GROWTH */

		/* Setup the newly allocated TCB with the initial state of the task. */
		prvInitialiseTCBVariables( pxNewTCB, "", uxPriority, NULL, usStackDepth );

		/* Initialize the TCB stack to look as if the task was already running,
		but had been interrupted by the scheduler.  The return address is set
		to the start of the task function. Once the stack has been initialised
		the	top of stack variable is updated. */
		
			pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, NULL );
				

		/* Ensure interrupts don't access the task lists while they are being
		updated. */
		vPortEnterCritical();
		{
			uxCurrentNumberOfTasks++;
			if( pxCurrentTCB == NULL )
			{
				/* There are no other tasks, or all the other tasks are in
				the suspended state - make this the current task. */
				pxCurrentTCB =  pxNewTCB;

				if( uxCurrentNumberOfTasks == ( UBaseType_t ) 1 )
				{
					/* This is the first task to be created so do the preliminary
					initialisation required.  We will not recover if this call
					fails, but we will report the failure. */
					prvInitialiseTaskLists();
				}
                
			}
			else
			{
				/* If the scheduler is not already running, make this task the
				current task if it is the highest priority task to be created
				so far. */
				if( xSchedulerRunning == pdFALSE )
				{
					if( pxCurrentTCB->uxPriority <= uxPriority )
					{
						pxCurrentTCB = pxNewTCB;
					}
					
				}
				
			}

			uxTaskNumber++;


			prvAddTaskToReadyList( pxNewTCB );

			xReturn = pdPASS;
            
		}
		vPortExitCritical();
	}
	else
	{
		xReturn = -1;//errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
	}

	if( xReturn == pdPASS )
	{
		if( xSchedulerRunning != pdFALSE )
		{
			/* If the created task is of a higher priority than the current task
			then it should run now. */
			if( pxCurrentTCB->uxPriority < uxPriority )
			{
				vPortYield();
			}
			
		}
		
	}

	return xReturn;
}



	void vTaskDelay( const TickType_t xTicksToDelay )
	{
	TickType_t xTimeToWake;
	BaseType_t xAlreadyYielded = pdFALSE;


		/* A delay time of zero just forces a reschedule. */
		if( xTicksToDelay > ( TickType_t ) 0U )
		{
			vTaskSuspendAll();
			{

				/* Calculate the time to wake - this may overflow but this is
				not a problem. */
				xTimeToWake = xTickCount + xTicksToDelay;

				/* We must remove ourselves from the ready list before adding
				ourselves to the blocked list as the same list item is used for
				both lists. */
				if( uxListRemove( &( pxCurrentTCB->xGenericListItem ) ) == ( UBaseType_t ) 0 )
				{
					/* The current task must be in a ready list, so there is
					no need to check, and the port reset macro can be called
					directly. */
					//portRESET_READY_PRIORITY( pxCurrentTCB->uxPriority, uxTopReadyPriority );//替换
                    ( uxTopReadyPriority ) &= ~( 1UL << ( pxCurrentTCB->uxPriority ) );
				}
                
				prvAddCurrentTaskToDelayedList( xTimeToWake );
			}
			xAlreadyYielded = xTaskResumeAll();
		}
        

		/* Force a reschedule if xTaskResumeAll has not already done so, we may
		have put ourselves to sleep. */
		if( xAlreadyYielded == pdFALSE )
		{
			vPortYield();
		}
        
	}

    
void vTaskStartScheduler( void )
{
BaseType_t xReturn;

    
    /* Create the idle task without storing its handle. */
	xReturn = xTaskGenericCreate( prvIdleTask, tskIDLE_STACK_SIZE, ( 0x00  ) );  /*lint !e961 MISRA exception, justified as it is not a redundant explicit cast to all supported compilers. */
	
    
	if( xReturn == pdPASS )
	{
		/* Interrupts are turned off here, to ensure a tick does not occur
		before or during the call to xPortStartScheduler().  The stacks of
		the created tasks contain a status word with interrupts switched on
		so interrupts will automatically get re-enabled when the first task
		starts to run. */
		ulPortSetInterruptMask();

        
		xNextTaskUnblockTime = portMAX_DELAY;
		xSchedulerRunning = pdTRUE;
		xTickCount = ( TickType_t ) 0U;

        

		/* Setting up the timer tick is hardware specific and thus in the
		portable interface. */
		if( xPortStartScheduler() != pdFALSE )
		{
			/* Should not reach here as if the scheduler is running the
			function will not return. */
		}
        
	}
    
}

/*----------------------------------------------------------*/

void vTaskSuspendAll( void )
{
	/* A critical section is not required as the variable is of type
	BaseType_t.  Please read Richard Barry's reply in the following link to a
	post in the FreeRTOS support forum before reporting this as a bug! -
	http://goo.gl/wu4acr */
	++uxSchedulerSuspended;
}


/*----------------------------------------------------------*/

BaseType_t xTaskResumeAll( void )
{
TCB_t *pxTCB;
BaseType_t xAlreadyYielded = pdFALSE;

	/* It is possible that an ISR caused a task to be removed from an event
	list while the scheduler was suspended.  If this was the case then the
	removed task will have been added to the xPendingReadyList.  Once the
	scheduler has been resumed it is safe to move all the pending ready
	tasks from this list into their appropriate ready list. */
	vPortEnterCritical();
	{
		--uxSchedulerSuspended;

		if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
		{
			if( uxCurrentNumberOfTasks > ( UBaseType_t ) 0U )
			{
				/* Move any readied tasks from the pending list into the
				appropriate ready list. */
				while( listLIST_IS_EMPTY( &xPendingReadyList ) == pdFALSE )
				{
					pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( ( &xPendingReadyList ) );
					( void ) uxListRemove( &( pxTCB->xEventListItem ) );
					( void ) uxListRemove( &( pxTCB->xGenericListItem ) );
					prvAddTaskToReadyList( pxTCB );

					/* If the moved task has a priority higher than the current
					task then a yield must be performed. */
					if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )
					{
						xYieldPending = pdTRUE;
					}
                    
				}

				/* If any ticks occurred while the scheduler was suspended then
				they should be processed now.  This ensures the tick count does
				not	slip, and that any delayed tasks are resumed at the correct
				time. */
				if( uxPendedTicks > ( UBaseType_t ) 0U )
				{
					while( uxPendedTicks > ( UBaseType_t ) 0U )
					{
						if( xTaskIncrementTick() != pdFALSE )
						{
							xYieldPending = pdTRUE;
						}
                        
						--uxPendedTicks;
					}
				}
                

				if( xYieldPending == pdTRUE )
				{
                    
					xAlreadyYielded = pdTRUE;
                    
					vPortYield();
				}
                
			}
		}
        
	}
	vPortExitCritical();

	return xAlreadyYielded;
}
/*-----------------------------------------------------------*/

/*----------------------------------------------------------*/

BaseType_t xTaskIncrementTick( void )
{
TCB_t * pxTCB;
TickType_t xItemValue;
BaseType_t xSwitchRequired = pdFALSE;

	if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
	{
		/* Increment the RTOS tick, switching the delayed and overflowed
		delayed lists if it wraps to 0. */
		++xTickCount;

		{
			/* Minor optimisation.  The tick count cannot change in this
			block. */
			const TickType_t xConstTickCount = xTickCount;

			if( xConstTickCount == ( TickType_t ) 0U )
			{
//				taskSWITCH_DELAYED_LISTS();
//                #define taskSWITCH_DELAYED_LISTS()															
                {																									\
                    List_t *pxTemp;																					\
                                                                                                                    \
                    /* The delayed tasks list should be empty when the lists are switched. */						\
                                                                                                                    \
                    pxTemp = pxDelayedTaskList;																		\
                    pxDelayedTaskList = pxOverflowDelayedTaskList;													\
                    pxOverflowDelayedTaskList = pxTemp;																\
                    xNumOfOverflows++;																				\
                    prvResetNextTaskUnblockTime();																	\
                }
			}
            

			/* See if this tick has made a timeout expire.  Tasks are stored in
			the	queue in the order of their wake time - meaning once one task
			has been found whose block time has not expired there is no need to
			look any further down the list. */
			if( xConstTickCount >= xNextTaskUnblockTime )
			{
				for( ;; )
				{
					if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
					{
						/* The delayed list is empty.  Set xNextTaskUnblockTime
						to the maximum possible value so it is extremely
						unlikely that the
						if( xTickCount >= xNextTaskUnblockTime ) test will pass
						next time through. */
						xNextTaskUnblockTime = portMAX_DELAY;
						break;
					}
					else
					{
						/* The delayed list is not empty, get the value of the
						item at the head of the delayed list.  This is the time
						at which the task at the head of the delayed list must
						be removed from the Blocked state. */
						pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
						xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xGenericListItem ) );

						if( xConstTickCount < xItemValue )
						{
							/* It is not time to unblock this item yet, but the
							item value is the time at which the task at the head
							of the blocked list must be removed from the Blocked
							state -	so record the item value in
							xNextTaskUnblockTime. */
							xNextTaskUnblockTime = xItemValue;
							break;
						}
                        

						/* It is time to remove the item from the Blocked state. */
						( void ) uxListRemove( &( pxTCB->xGenericListItem ) );

						/* Is the task waiting on an event also?  If so remove
						it from the event list. */
						if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
						{
							( void ) uxListRemove( &( pxTCB->xEventListItem ) );
						}
                        

						/* Place the unblocked task into the appropriate ready
						list. */
						prvAddTaskToReadyList( pxTCB );

                        
							/* Preemption is on, but a context switch should
							only be performed if the unblocked task has a
							priority that is equal to or higher than the
							currently executing task. */
							if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )
							{
								xSwitchRequired = pdTRUE;
							}
                            
					}
				}
			}
		}

    }
	else
	{
		++uxPendedTicks;
	}

		if( xYieldPending != pdFALSE )
		{
			xSwitchRequired = pdTRUE;
		}

	return xSwitchRequired;
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void vTaskSwitchContext( void )
{
	if( uxSchedulerSuspended != ( UBaseType_t ) pdFALSE )
	{
		/* The scheduler is currently suspended - do not allow a context
		switch. */
		xYieldPending = pdTRUE;
	}
	else
	{
		xYieldPending = pdFALSE;

		/* Select a new task to run using either the generic C or port
		optimised asm code. */
        /* #define taskSELECT_HIGHEST_PRIORITY_TASK()**/
        {																								
        UBaseType_t uxTopPriority;																		
                                                                                                        
            /* Find the highest priority queue that contains ready tasks. */							
//            portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );
            uxTopPriority = ( 31 - __clz( ( uxTopReadyPriority ) ) );
            
            listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );
        } /* taskSELECT_HIGHEST_PRIORITY_TASK() */
	}
}
/*-----------------------------------------------------------*/


/*
 * -----------------------------------------------------------
 * The Idle task.
 * ----------------------------------------------------------
 *
 * The portTASK_FUNCTION() macro is used to allow port/compiler specific
 * language extensions.  The equivalent prototype for this function is:
 *
 * void prvIdleTask( void *pvParameters );
 *
 */
static void prvIdleTask( void *pvParameters )
{
	/* Stop warnings. */
	( void ) pvParameters;

	for( ;; ){ }
}

/*-----------------------------------------------------------*/

static void prvInitialiseTCBVariables( TCB_t * const pxTCB, const char * const pcName, UBaseType_t uxPriority, const MemoryRegion_t * const xRegions, const uint16_t usStackDepth ) /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
{


	/* This is used as an array index so must ensure it's not too large.  First
	remove the privilege bit if one is present. */
	if( uxPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
	{
		uxPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;
	}
    

	pxTCB->uxPriority = uxPriority;
    

//	vListInitialiseItem( &( pxTCB->xGenericListItem ) );
//	vListInitialiseItem( &( pxTCB->xEventListItem ) );
    
    /* Make sure the list item is not recorded as being on a list. */
    (&( pxTCB->xGenericListItem ))->pvContainer = NULL;
    (&( pxTCB->xEventListItem   ))->pvContainer = NULL;
    

	/* Set the pxTCB as a link back from the ListItem_t.  This is so we can get
	back to	the containing TCB from a generic item in a list. */
//	listSET_LIST_ITEM_OWNER( &( pxTCB->xGenericListItem ), pxTCB );
    ( ( &( pxTCB->xGenericListItem ) )->pvOwner = ( void * ) ( pxTCB ) );

	/* Event lists are always in priority order. */
	listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) uxPriority ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
//	listSET_LIST_ITEM_OWNER( &( pxTCB->xEventListItem ), pxTCB );

    ( ( &( pxTCB->xEventListItem ) )->pvOwner = ( void * ) ( pxTCB ) );
    
}
/*-----------------------------------------------------------*/



static void prvInitialiseTaskLists( void )
{
UBaseType_t uxPriority;

	for( uxPriority = ( UBaseType_t ) 0U; uxPriority < ( UBaseType_t ) configMAX_PRIORITIES; uxPriority++ )
	{
		vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
	}

	vListInitialise( &xDelayedTaskList1 );
	vListInitialise( &xDelayedTaskList2 );
	vListInitialise( &xPendingReadyList );
    

	/* Start with pxDelayedTaskList using list1 and the pxOverflowDelayedTaskList
	using list2. */
	pxDelayedTaskList = &xDelayedTaskList1;
	pxOverflowDelayedTaskList = &xDelayedTaskList2;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static void prvAddCurrentTaskToDelayedList( const TickType_t xTimeToWake )
{
	/* The list item will be inserted in wake time order. */
	listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xGenericListItem ), xTimeToWake );

	if( xTimeToWake < xTickCount )
	{
		/* Wake time has overflowed.  Place this item in the overflow list. */
		vListInsert( pxOverflowDelayedTaskList, &( pxCurrentTCB->xGenericListItem ) );
	}
	else
	{
		/* The wake time has not overflowed, so the current block list is used. */
		vListInsert( pxDelayedTaskList, &( pxCurrentTCB->xGenericListItem ) );

		/* If the task entering the blocked state was placed at the head of the
		list of blocked tasks then xNextTaskUnblockTime needs to be updated
		too. */
		if( xTimeToWake < xNextTaskUnblockTime )
		{
			xNextTaskUnblockTime = xTimeToWake;
		}
        
	}
}
/*-----------------------------------------------------------*/

static TCB_t *prvAllocateTCBAndStack( const uint16_t usStackDepth, StackType_t * const puxStackBuffer1 )
{
TCB_t *pxNewTCB;

	/* If the stack grows down then allocate the stack then the TCB so the stack
	does not grow into the TCB.  Likewise if the stack grows up then allocate
	the TCB then the stack. */
	
    
	StackType_t *pxStack;

		/* Allocate space for the stack used by the task being created. */
        //等效替换，puxStackBuffer恒为null
//		pxStack = ( StackType_t * ) pvPortMallocAligned( ( ( ( size_t ) usStackDepth ) * sizeof( StackType_t ) ), puxStackBuffer ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
        pxStack = pvPortMalloc( ( ( ( size_t ) usStackDepth ) * sizeof( StackType_t ) ) );
    
    
		if( pxStack != NULL )
		{
			/* Allocate space for the TCB.  Where the memory comes from depends
			on the implementation of the port malloc function. */
			pxNewTCB = ( TCB_t * ) pvPortMalloc( sizeof( TCB_t ) );

			if( pxNewTCB != NULL )
			{
				/* Store the stack location in the TCB. */
				pxNewTCB->pxStack = pxStack;
			}
			else
			{
				/* The stack cannot be used as the TCB was not created.  Free it
				again. */
				vPortFree( pxStack );
			}
		}
		else
		{
			pxNewTCB = NULL;
		}
        

	return pxNewTCB;
}

    
/*-----------------------------------------------------------*/

static void prvResetNextTaskUnblockTime( void )
{
TCB_t *pxTCB;

	if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
	{
		/* The new current delayed list is empty.  Set xNextTaskUnblockTime to
		the maximum possible value so it is	extremely unlikely that the
		if( xTickCount >= xNextTaskUnblockTime ) test will pass until
		there is an item in the delayed list. */
		xNextTaskUnblockTime = portMAX_DELAY;
	}
	else
	{
		/* The new current delayed list is not empty, get the value of
		the item at the head of the delayed list.  This is the time at
		which the task at the head of the delayed list should be removed
		from the Blocked state. */
		( pxTCB ) = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );
		xNextTaskUnblockTime = listGET_LIST_ITEM_VALUE( &( ( pxTCB )->xGenericListItem ) );
	}
}











/******** list.c ********************************************************************/

/*-----------------------------------------------------------
 * PUBLIC LIST API documented in list.h
 *----------------------------------------------------------*/

void vListInitialise( List_t * const pxList )
{
	/* The list structure contains a list item which is used to mark the
	end of the list.  To initialise the list the list end is inserted
	as the only list entry. */
	pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd );			/*lint !e826 !e740 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

	/* The list end value is the highest possible value in the list to
	ensure it remains at the end of the list. */
	pxList->xListEnd.xItemValue = portMAX_DELAY;

	/* The list end next and previous pointers point to itself so we know
	when the list is empty. */
	pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );	/*lint !e826 !e740 The mini list structure is used as the list end to save RAM.  This is checked and valid. */
	pxList->xListEnd.pxPrevious = ( ListItem_t * ) &( pxList->xListEnd );/*lint !e826 !e740 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

	pxList->uxNumberOfItems = ( UBaseType_t ) 0U;

}

/*-----------------------------------------------------------*/

void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem )
{
ListItem_t * const pxIndex = pxList->pxIndex;

	/* Insert a new list item into pxList, but rather than sort the list,
	makes the new list item the last item to be removed by a call to
	listGET_OWNER_OF_NEXT_ENTRY(). */
	pxNewListItem->pxNext = pxIndex;
	pxNewListItem->pxPrevious = pxIndex->pxPrevious;

	pxIndex->pxPrevious->pxNext = pxNewListItem;
	pxIndex->pxPrevious = pxNewListItem;

	/* Remember which list the item is in. */
	pxNewListItem->pvContainer = ( void * ) pxList;

	( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/

void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem )
{
ListItem_t *pxIterator;
const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

	/* Insert the new list item into the list, sorted in xItemValue order.

	If the list already contains a list item with the same item value then the
	new list item should be placed after it.  This ensures that TCB's which are
	stored in ready lists (all of which have the same xItemValue value) get a
	share of the CPU.  However, if the xItemValue is the same as the back marker
	the iteration loop below will not end.  Therefore the value is checked
	first, and the algorithm slightly modified if necessary. */
	if( xValueOfInsertion == portMAX_DELAY )
	{
		pxIterator = pxList->xListEnd.pxPrevious;
	}
	else
	{
		for( pxIterator = ( ListItem_t * ) &( pxList->xListEnd ); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext ) /*lint !e826 !e740 The mini list structure is used as the list end to save RAM.  This is checked and valid. */
		{
			/* There is nothing to do here, just iterating to the wanted
			insertion position. */
		}
	}

	pxNewListItem->pxNext = pxIterator->pxNext;
	pxNewListItem->pxNext->pxPrevious = pxNewListItem;
	pxNewListItem->pxPrevious = pxIterator;
	pxIterator->pxNext = pxNewListItem;

	/* Remember which list the item is in.  This allows fast removal of the
	item later. */
	pxNewListItem->pvContainer = ( void * ) pxList;

	( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/

UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
/* The list item knows which list it is in.  Obtain the list from the list
item. */
List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;

	pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
	pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;


	/* Make sure the index is left pointing to a valid item. */
	if( pxList->pxIndex == pxItemToRemove )
	{
		pxList->pxIndex = pxItemToRemove->pxPrevious;
	}

	pxItemToRemove->pvContainer = NULL;
	( pxList->uxNumberOfItems )--;

	return pxList->uxNumberOfItems;
}
/*-----------------------------------------------------------*/
