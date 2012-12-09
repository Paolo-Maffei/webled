#ifndef _LED7_H_
#define _LED7_H_

#include "LPC11xx.h" 

static void init_4511B(void);

//��4511B��D3~D0д��data�ĵ�4λ
static void write_4511B(uint8_t data);

//��4028B��D~Aд��data�ĵ�4λ
static void write_4028B(uint8_t data);

//init 4028B's A B C D pin to output 0b1010
static void init_4028B(void);

//��ʼ��timer4
static void init_timer(void);


//�ܵĳ�ʼ�� 
void Init_LED7(void);
//��4511Bд��Ҫ��ʾ����ֵ��0~9
void WriteNum(uint8_t num);
//��4511Bд��blank
void WriteBlank(void);
//ʹ�ܵ�num�������
void LightLed7(uint8_t num);
//�ر��������ʾ
void Led7Off(void);
//ɨ������ܣ�����ʾ����disp_buf�����ݸ��£���ʱ���жϵ���
void refresh_disp(void);

/*
  disp_headΪҪ��ʾ��ҳ����
  disp_tailΪҪ��ʾ�Ĵ�����ֵ
*/
extern volatile uint8_t disp_head;
extern volatile uint32_t disp_tail;
#endif /*_LED7_H_*/