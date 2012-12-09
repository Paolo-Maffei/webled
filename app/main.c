
/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include "LPC11xx.h"			/* LPC11xx Peripheral Registers */
#include "led7.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "uip.h"
#include "uip_task.h"

#include "led7_task.h"

/*-----------------------------------------------------------*/

/* Priorities/stacks for the tasks. */
#define mainUIP_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define mainUIP_TASK_STACK_SIZE		( 250 )

#define mainLED7_PRIORITY		( tskIDLE_PRIORITY + 4 )
#define mainLED7_TASK_STACK_SIZE	( 50 )
/*mac addr*/
struct uip_eth_addr mac_addr={{0x00,0x04,0xa3,0x00,0x00,0x01}};

xQueueHandle comQueue;
xQueueHandle comQueue2;


/*Configure the HW  */
static void prvSetupHardware( void );

/*-----------------------------------------------------------*/

int main( void )
{
	prvSetupHardware();
        
        comQueue = xQueueCreate(1, ( unsigned portBASE_TYPE ) sizeof( unsigned char ) );
        comQueue2 = xQueueCreate(1, ( unsigned portBASE_TYPE ) sizeof( unsigned char ) );

        xTaskCreate( vuIP_TASK, "uIP", mainUIP_TASK_STACK_SIZE, &mac_addr, mainUIP_PRIORITY, NULL );
	xTaskCreate( led7_Task, "LED7", mainLED7_TASK_STACK_SIZE, NULL, mainLED7_PRIORITY, NULL );
        
	vTaskStartScheduler();

	return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
        SystemInit();          /*初始化系统和外设总线时钟*/
        LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);  /* 初始化GPIO AHB时钟*/
        Init_LED7();
}




