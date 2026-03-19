#ifndef __9834_H_
#define __9834_H_

#include "main.h"

//TRIANGLE_WAVE,SINE_WAVE,SQUARE_WAVEĬ��"λ"����

#define TRIANGLE_WAVE 	0x2002 //���ǲ�
#define SINE_WAVE 			0x2008 // ���Ҳ�
#define SQUARE_WAVE 		0x2028 // ����
#define PIN_SW    			0x200

/*AD9834����Ƶ��75MHZ*/
#define AD9834_SYSTEM_COLCK	75000000UL

/* AD9834�������� */

#define CS_0()  HAL_GPIO_WritePin(FSYNC_GPIO_Port,FSYNC_Pin,GPIO_PIN_RESET)
#define	CS_1()  HAL_GPIO_WritePin(FSYNC_GPIO_Port,FSYNC_Pin,GPIO_PIN_SET)

#define RESET_0()  HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_RESET)
#define	RESET_1()  HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_SET)

//===================��λ����=======================
#define AD9834_PS_SET HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET)
#define AD9834_PS_CLR HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET)

//===================Ƶ�ʿ���=======================
#define AD9834_FS_SET HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET)
#define AD9834_FS_CLR HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)





#define FREQ_0      0 

#define FREQ_1      1 

#define PHASE_0     0

#define PHASE_1     1

 

#define DB15        0 

#define DB14        0 

#define DB13        B28 

#define DB12        HLB 

#define DB11        FSEL 

#define DB10        PSEL 

#define DB9         PIN_SW 

#define DB8         RESET 

#define DB7         SLEEP1 

#define DB6         SLEEP12 

#define DB5         OPBITEN 

#define DB4         SIGN_PIB 

#define DB3         DIV2 

#define DB2         0 

#define DB1         MODE 

#define DB0         0 

#define CONTROL_REGISTER    (DB15<<15)|(DB14<<14)|(DB13<<13)|(DB12<<12)|(DB11<<11)|(DB10<<10)|(DB9<<9)|(DB8<<8)|(DB7<<7)|(DB6<<6)|(DB5<<5)|(DB4<<4)|(DB3<<3)|(DB2<<2)|(DB1<<1)|(DB0<<0) 

/* AD9834�������� */ 

 void AD9834_Write_16Bits(unsigned int data) ;  //дһ���ֵ�AD9834 

 void AD9834_Select_Wave(unsigned int initdata) ; //ѡ��������� 

void AD9834_Init(void);  					//��ʼ������ 

 void AD9834_Set_Freq(unsigned char freq_number, unsigned long freq) ;//ѡ������Ĵ��������Ƶ��ֵ 
 
 void AD9834_Set_Phase(unsigned char phase_number, float phase_in_degrees);//ѡ����λ�Ĵ����������λֵ 

#endif
