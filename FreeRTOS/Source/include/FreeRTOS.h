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

/*
 * Defines the prototype to which task functions must conform.  Defined in this
 * file to ensure the type is known before portable.h is included.
 */
typedef void (*TaskFunction_t)( void * );

#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )

#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )
   
    
    
    
    
/* Definitions specific to the port being used. */
#include "portable.h"



#ifndef portPOINTER_SIZE_TYPE
	#define portPOINTER_SIZE_TYPE uint32_t
#endif

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

