/**
 ******************************************************************************
 * @file    Task02.c
 * @author  Windy Albert
 * @date    08-April-2016
 * @brief   Task to ...
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_app.h"
#include "FreeRTOS.h"
#include "task.h"

float test2 = 0.0f;

/**
 * @brief		Task00 program.
 * @function  	None
 * @RunPeriod 	None
 */

void Task02( void *pvParameters ) 
{

	for (;;) 
    {
		//TODO:

        test2 += 0.01f;
        
        vTaskDelay( 10 );
        
		/******************************************************************************/
	} //for
}

/******************* (C) COPYRIGHT 2014 Windy Albert ***********END OF FILE****/
