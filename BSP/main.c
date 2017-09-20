/**
  ******************************************************************************
  * @file    main.c 
  * @author  Windy Albert
  * @version V1.0
  * @date    26-August-2015
  * @brief   Main program body.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
//#include "minos.h"	                                 /* Header file for MinOS */

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/************************    Task00 Configuration    **************************/

void Task00( void *pvParameters ) ;


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	//SysTick_Config(SystemCoreClock/1000);
	

/************* Task00 Creation ************************************************/
    xTaskCreate( Task00, ( const char * ) "LED1_Task", 1000, NULL, 1, NULL );
    

    vTaskStartScheduler();
    
    return 0;

}


/******************* (C) COPYRIGHT 2015 Windy Albert************END OF FILE****/
