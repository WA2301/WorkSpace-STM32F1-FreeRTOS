/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM3 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "task.h"

//#if configMAX_SYSCALL_INTERRUPT_PRIORITY == 0
//	#error configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.  See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html
//#endif


/* Each task maintains its own interrupt status in the critical nesting
variable. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
//void xPortPendSVHandler( void );
//void xPortSysTickHandler( void );
//void vPortSVCHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void prvStartFirstTask( void );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );


/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
	*pxTopOfStack = ( 0x01000000 );//portINITIAL_XPSR;	/* xPSR */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pxCode;	/* PC */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) prvTaskExitError;	/* LR */

	pxTopOfStack -= 5;	/* R12, R3, R2 and R1. */
	*pxTopOfStack = ( StackType_t ) pvParameters;	/* R0 */
	pxTopOfStack -= 8;	/* R11, R10, R9, R8, R7, R6, R5 and R4. */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
//	configASSERT( uxCriticalNesting == ~0UL );
	ulPortSetInterruptMask();
	for( ;; );
}
/*-----------------------------------------------------------*/

__asm void vPortSVCHandler( void )
{
	PRESERVE8

	ldr	r3, =pxCurrentTCB	/* Restore the context. */
	ldr r1, [r3]			/* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
	ldr r0, [r1]			/* The first item in pxCurrentTCB is the task top of stack. */
	ldmia r0!, {r4-r11}		/* Pop the registers that are not automatically saved on exception entry and the critical nesting count. */
	msr psp, r0				/* Restore the task stack pointer. */
	isb
	mov r0, #0
	msr	basepri, r0
	orr r14, #0xd
	bx r14
}
/*-----------------------------------------------------------*/

__asm void prvStartFirstTask( void )
{
	PRESERVE8

	/* Use the NVIC offset register to locate the stack. */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0]

	/* Set the msp back to the start of the stack. */
	msr msp, r0
	/* Globally enable interrupts. */
	cpsie i
	cpsie f
	dsb
	isb
	/* Call SVC to start the first task. */
	svc 0
	nop
	nop
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
	/* Make PendSV and SysTick the lowest priority interrupts. */
	//portNVIC_SYSPRI2_REG |= ( ( ( uint32_t ) 255 ) << 16UL );//portNVIC_PENDSV_PRI;
    ( * ( ( volatile uint32_t * ) 0xe000ed20 ) ) |= ( ( ( uint32_t ) 255 ) << 16UL );//portNVIC_PENDSV_PRI;
	( * ( ( volatile uint32_t * ) 0xe000ed20 ) ) |= ( ( ( uint32_t ) 255 ) << 24UL );//portNVIC_SYSTICK_PRI;

    
    
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	vPortSetupTimerInterrupt();

	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

	/* Start the first task. */
	prvStartFirstTask();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortYield( void )
{
	/* Set a PendSV to request a context switch. */
	portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;

	/* Barriers are normally not required but do ensure the code is completely
	within the specified behaviour for the architecture. */
	__dsb( 15 );
	__isb( 15 );//portSY_FULL_READ_WRITE
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	ulPortSetInterruptMask();
	uxCriticalNesting++;
	__dsb( 15 );
	__isb( 15 );

}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
//	configASSERT( uxCriticalNesting );
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
		vPortClearInterruptMask( 0 );
	}
}
/*-----------------------------------------------------------*/

__asm void xPortPendSVHandler( void )
{
	extern uxCriticalNesting;
	extern pxCurrentTCB;
	extern vTaskSwitchContext;

	PRESERVE8

	mrs r0, psp
	isb

	ldr	r3, =pxCurrentTCB		/* Get the location of the current TCB. */
	ldr	r2, [r3]

	stmdb r0!, {r4-r11}			/* Save the remaining registers. */
	str r0, [r2]				/* Save the new top of stack into the first member of the TCB. */

	stmdb sp!, {r3, r14}
	mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r0
	bl vTaskSwitchContext
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3, r14}

	ldr r1, [r3]
	ldr r0, [r1]				/* The first item in pxCurrentTCB is the task top of stack. */
	ldmia r0!, {r4-r11}			/* Pop the registers and the critical nesting count. */
	msr psp, r0
	isb
	bx r14
	nop
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
	/* The SysTick runs at the lowest interrupt priority, so when this interrupt
	executes all interrupts must be unmasked.  There is therefore no need to
	save and then restore the interrupt mask value as its value is already
	known. */
	( void ) ulPortSetInterruptMask();
	{
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* A context switch is required.  Context switching is performed in
			the PendSV interrupt.  Pend the PendSV interrupt. */
			portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
		}
	}
	vPortClearInterruptMask( 0 );
}

/*-----------------------------------------------------------*/

/*
 * Setup the SysTick timer to generate the tick interrupts at the required
 * frequency.
 */

void vPortSetupTimerInterrupt( void )
{
    /* Configure SysTick to interrupt at the requested rate. */
//    portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
    ( * ( ( volatile uint32_t * ) 0xe000e014 ) ) = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
    
    //portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );
    ( * ( ( volatile uint32_t * ) 0xe000e010 ) )   = ( ( 1UL << 2UL ) | ( 1UL << 1UL ) | ( 1UL << 0UL ) );
}

/*-----------------------------------------------------------*/

__asm uint32_t ulPortSetInterruptMask( void )
{
	PRESERVE8

	mrs r0, basepri
	mov r1, #configMAX_SYSCALL_INTERRUPT_PRIORITY
	msr basepri, r1
	bx r14
}
/*-----------------------------------------------------------*/

__asm void vPortClearInterruptMask( uint32_t ulNewMask )
{
	PRESERVE8

	msr basepri, r0
	bx r14
}
/*-----------------------------------------------------------*/


