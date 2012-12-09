#ifndef _LED7_H_
#define _LED7_H_

#include "LPC11xx.h" 

static void init_4511B(void);

//向4511B的D3~D0写入data的低4位
static void write_4511B(uint8_t data);

//向4028B的D~A写入data的低4位
static void write_4028B(uint8_t data);

//init 4028B's A B C D pin to output 0b1010
static void init_4028B(void);

//初始化timer4
static void init_timer(void);


//总的初始化 
void Init_LED7(void);
//向4511B写入要显示的数值：0~9
void WriteNum(uint8_t num);
//向4511B写入blank
void WriteBlank(void);
//使能第num个数码管
void LightLed7(uint8_t num);
//关闭数码管显示
void Led7Off(void);
//扫描数码管，将显示缓存disp_buf的内容更新，由时钟中断调用
void refresh_disp(void);

/*
  disp_head为要显示的页数，
  disp_tail为要显示的打码数值
*/
extern volatile uint8_t disp_head;
extern volatile uint32_t disp_tail;
#endif /*_LED7_H_*/