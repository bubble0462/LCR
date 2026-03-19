#include "9834.h" 
#include "main.h"
extern SPI_HandleTypeDef hspi1;
/**************************************************************** 

��������: AD9834_Write_16Bits 

��    ��: ��AD9834д��16Ϊ���� 

��    ��: data --  Ҫд���16λ���� 

����ֵ  : �� 

*****************************************************************/ 

void AD9834_Write_16Bits(unsigned int data) 
{ 
    
    CS_0() ; 

		HAL_SPI_Transmit(&hspi1,(uint8_t *)&data, 1 ,HAL_MAX_DELAY);
	
    CS_1() ; 
} 

/*********************************************************************************** 

�������ƣ�AD9834_Select_Wave 

��    �ܣ�����Ϊ���ƣ� 

    -------------------------------------------------- 

    IOUT���Ҳ� ��SIGNBITOUT���� ��дFREQREG0 ��дPHASE0 

    ad9834_write_16bit(0x2028)   һ����дFREQREG0 

    ad9834_write_16bit(0x0038)   ������дFREQREG0��LSB 

    ad9834_write_16bit(0x1038)   ������дFREQREG0��MSB 

    -------------------------------------------------- 

    IOUT���ǲ� ��дPHASE0 

    ad9834_write_16bit(0x2002)   һ����дFREQREG0 

    ad9834_write_16bit(0x0002)   ������дFREQREG0��LSB 

    ad9834_write_16bit(0x1008)   ������дFREQREG0��MSB 

��    ����initdata -- Ҫ��������� 

����ֵ  ���� 

************************************************************************************/ 
void AD9834_Select_Wave(unsigned int initdata) 

{ 

    CS_1(); 

    RESET_1(); 

    RESET_1(); 

    RESET_0();

      

    AD9834_Write_16Bits(initdata); 

} 

/**************************************************************** 

��������: Init_AD9834 

��    ��: ��ʼ��AD9834�������� ��    ��: �� 

����ֵ  : �� 

*****************************************************************/ 
// B6 FS  B5 PS    B7 FSY   B8 SCK   B9 SDA B10 RST 

void AD9834_Init() 

{

   
		AD9834_FS_CLR;
		AD9834_PS_CLR;

	AD9834_Write_16Bits(0x2100);
	AD9834_Write_16Bits(0x2038);
	AD9834_Write_16Bits(0XC000);
	AD9834_Write_16Bits(0x2100);
} 

/**************************************************************** 

��������: AD9834_Set_Freq 

��    ��: ����Ƶ��ֵ 

��    ��: freq_number -- Ҫд���ƽ�ʼĴ���(FREQ_0��FREQ_1) 

          freq -- Ƶ��ֵ (Freq_value(value)=Freq_data(data)*FCLK/2^28) 

����ֵ  : �� 

*****************************************************************/ 
void AD9834_Set_Freq(unsigned char freq_number, unsigned long freq) 
{
    unsigned long FREQREG = (unsigned long)(268435456.0/AD9834_SYSTEM_COLCK*freq); 
    unsigned int FREQREG_LSB_14BIT = (unsigned int)FREQREG; 
    unsigned int FREQREG_MSB_14BIT = (unsigned int)(FREQREG>>14); 
	
    if(freq_number == FREQ_0) 
    { 
        FREQREG_LSB_14BIT &= ~(1U<<15); 
        FREQREG_LSB_14BIT |= 1<<14; 
        FREQREG_MSB_14BIT &= ~(1U<<15); 
        FREQREG_MSB_14BIT |= 1<<14; 
    }
    else
    {
        FREQREG_LSB_14BIT &= ~(1<<14); 
        FREQREG_LSB_14BIT |= 1U<<15; 
        FREQREG_MSB_14BIT &= ~(1<<14); 
        FREQREG_MSB_14BIT |= 1U<<15; 
    } 
    AD9834_Write_16Bits(FREQREG_LSB_14BIT); 
    AD9834_Write_16Bits(FREQREG_MSB_14BIT);      
} 
//============================================================// 

/*   ע��ʹ��ʱ�ȵ��ó�ʼ��������ʼ��IO���ã�Ȼ�����AD9834_Select_Wave()ѡ��

���Σ����ѡ��Ƶ��ֵ��������� */ 

/**************************************************************** 
��������: AD9834_Set_Phase
��    ��: ������λֵ
��    ��: phase_number -- Ҫд�����λ�Ĵ��� (PHASE_0 �� PHASE_1)
          phase_in_degrees -- ��λֵ (0.0 �� 360.0 ��)
����ֵ  : ��
*****************************************************************/
void AD9834_Set_Phase(unsigned char phase_number, float phase_in_degrees)
{
    unsigned int phase_word = 0;
    unsigned int phase_val = 0;

    // 1. ���ݹ�ʽ����12λ����λֵ
    if (phase_in_degrees < 0.0) phase_in_degrees = 0.0;
    if (phase_in_degrees > 360.0) phase_in_degrees = 360.0;
    phase_val = (unsigned int)((phase_in_degrees / 360.0) * 4096.0);
    
    // 2. �����λֵ�Ƿ���12λ��Χ�� (0-4095)
    if (phase_val > 4095)
    {
        phase_val = 4095;
    }

    // 3. ����ѡ��ļĴ�������ϵ�ַ�����λֵ
    if (phase_number == PHASE_0)
    {
        // PHASE0 ��ַ���� 0xC000
        phase_word = 0xC000 | phase_val;
    }
    else // phase_number == PHASE_1
    {
        // PHASE1 ��ַ���� 0xE000
        phase_word = 0xE000 | phase_val;
    }

    // 4. ͨ��SPIд�����յ�16λ����
    AD9834_Write_16Bits(phase_word);
}

