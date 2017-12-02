/**
 ******************************************************************************
 * @file    Task00.c
 * @author  Windy Albert
 * @date    08-April-2016
 * @brief   Task to ...
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_app.h"
#include "FreeRTOS.h"
#include "task.h"


float f_test = 1.0f;

u32 test;

struct 
{
    uint8_t byte0;
    uint8_t byte1;
    uint8_t bit0        :1;
    uint8_t bit1        :2;
    uint8_t bit2        :1;
    uint8_t bita0        :1;
    uint8_t bita1        :2;
    uint8_t bita2        :1;
    uint8_t bitb0        :1;
    uint8_t bitb1        :2;
    uint8_t bitb2        :1;
}Test;


TickType_t xLastWakeTime = 0xFFFFFFFF - 5000;
/**
 * @brief		Task00 program.
 * @function  	None
 * @RunPeriod 	None
 */

void Task00( void *pvParameters ) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/** Task Initialize *******************************************************/

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    Test.byte0 = 2;
    Test.byte1 = 3;
    Test.bit0 = 1;
    Test.bita1 = 3;
    Test.bit2 = 1;
    
    Test.bitb2 = 1;

	for (;;) 
    {
		//TODO:

        f_test += 0.1f;
		test++;
		*GPO_BAND(GPIOC_BASE, 13) ^= 1;
        
        vTaskDelay( 1000 );
                
//		*GPO_BAND(GPIOC_BASE, 13) ^= 1;
//        
//        vTaskDelayUntil( &xLastWakeTime , 1000 );
		/******************************************************************************/
	} //for
}

/******************* (C) COPYRIGHT 2014 Windy Albert ***********END OF FILE****/
