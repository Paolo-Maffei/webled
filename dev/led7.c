/*7 segment led driver */

#include"led7.h"

static void init_4511B(void)
{
  //初始化连接4511B的A~D引脚为，0b1100, output blank
  //A:  P1.5
  LPC_IOCON->PIO1_5 |= 0x00;                                          // gpio
  LPC_GPIO1->DIR    |= 0x020;                                         // direction:output 
  LPC_GPIO1->DATA   &= ~0x020;                                         // init high level
  //B:  P2.0
  LPC_IOCON->PIO2_0 |= 0x00;                                          // gpio
  LPC_GPIO2->DIR    |= 0x001;                                         // direction:output 
  LPC_GPIO2->DATA   &= ~0x001;                                         // init high level
  //C:  P2.6
  LPC_IOCON->PIO2_6 |= 0x00;                                          // gpio
  LPC_GPIO2->DIR    |= 0x040;                                         // direction:output 
  LPC_GPIO2->DATA   &= ~0x040;                                         // init high level
  //D:  P3.3
  LPC_IOCON->PIO3_3 |= 0x00;                                          // gpio
  LPC_GPIO3->DIR    |= 0x008;                                         // direction:output 
  LPC_GPIO3->DATA   |= 0x008;                                         // init high level  
}

//向4511B的D3~D0写入data的低4位
static void write_4511B(uint8_t data)
{
  if(0x01&data)
    LPC_GPIO1->DATA   |= 0x020;
  else
    LPC_GPIO1->DATA   &= ~0x020;
  
  if(0x02&data)
    LPC_GPIO2->DATA   |= 0x001;
  else
    LPC_GPIO2->DATA   &= ~0x001;
  
  if(0x04&data)
    LPC_GPIO2->DATA   |= 0x040;
  else
    LPC_GPIO2->DATA   &= ~0x040;
    
  if(0x08&data)
    LPC_GPIO3->DATA   |= 0x008;
  else
    LPC_GPIO3->DATA   &= ~0x008;
}
//总的初始化 
void Init_LED7(void)
{
  init_4511B();
  
  //DP:  P1.8
  LPC_IOCON->PIO1_8 |= 0x00;                                          // gpio
  LPC_GPIO1->DIR    |= 0x100;                                         // direction:output 
  LPC_GPIO1->DATA   |= 0x100;                                         // init high level
  
  //LED1:  P2.1
  LPC_IOCON->PIO2_1 |= 0x00;                                          // gpio
  LPC_GPIO2->DIR    |= 0x002;                                         // direction:output 
  LPC_GPIO2->DATA   &= ~0x002;                                         // init high level
  
  //LED2:  P2.8
  LPC_IOCON->PIO2_8 |= 0x00;                                          // gpio
  LPC_GPIO2->DIR    |= 0x100;                                         // direction:output 
  LPC_GPIO2->DATA   &= ~0x100;                                         // init high level
  
  //LED3:  P2.7
  LPC_IOCON->PIO2_7 |= 0x00;                                          // gpio
  LPC_GPIO2->DIR    |= 0x080;                                         // direction:output 
  LPC_GPIO2->DATA   &= ~0x080;                                         // init high level
  
  //LED:  P0.2
  LPC_IOCON->PIO0_2 |= 0x00;                                          // gpio
  LPC_GPIO0->DIR    |= 0x004;                                         // direction:output 
  LPC_GPIO0->DATA   &= ~0x004;                                         // init high level
}



//向4511B写入要显示的数值：0~9
void WriteNum(uint8_t num)
{
  write_4511B(num);
}

//向4511B写入blank
void WriteBlank(void)
{
  write_4511B(0x0C);
}

//使能第num个数码管

//点亮显示
void Led7On(void)
{
  //DP:  P1.8
  LPC_GPIO1->DATA   |= 0x100;                                        
  //LED1:  P2.1
  LPC_GPIO2->DATA   &= ~0x002;                                         
  //LED2:  P2.8
  LPC_GPIO2->DATA   &= ~0x100;                                         
  //LED3:  P2.7
  LPC_GPIO2->DATA   &= ~0x080;                                         
  //LED:  P0.2
  LPC_GPIO0->DATA   &= ~0x004;                                       
}
//关闭数码管显示
void Led7Off(void)
{
  //DP:  P1.8
  LPC_GPIO1->DATA   &= ~0x100;                                        
  //LED1:  P2.1
  LPC_GPIO2->DATA   |= 0x002;                                         
  //LED2:  P2.8
  LPC_GPIO2->DATA   |= 0x100;                                         
  //LED3:  P2.7
  LPC_GPIO2->DATA   |= 0x080;                                         
  //LED:  P0.2
  LPC_GPIO0->DATA   |= 0x004;           
}
//清除数码管显示
//只需将disp_tail设置为100000000

//初始化timer4


//扫描数码管，将显示缓存disp_buf的内容更新，由时钟中断调用
void refresh_disp(void)
{
}


  