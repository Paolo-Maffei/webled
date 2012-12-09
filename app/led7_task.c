
#include "led7.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void led7_Task(void *pvPara)
{
  unsigned char on_status = 0;
  for(;;)
  {
    xQueueReceive(comQueue,&on_status,100);
    if(on_status)
    {
      Led7On();
    }
    else
    {
      Led7Off();
    }
  }
  
}