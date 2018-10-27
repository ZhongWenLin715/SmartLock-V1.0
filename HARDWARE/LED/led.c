#include "led.h"
#include "delay.h"
#include "oled.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ��ӢSTM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.5 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5�˿�����, �������
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOB,GPIO_Pin_5); 						 //PE.5�����

    
}
 
void fall_for_10s(void)
{
    OLED_ShowString(10,2," Wait for 10S",8);	
	LED0=!LED0;//�����˸��ʾ1s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
   
    OLED_ShowString(10,2," Wait for 9 S",8);    
	LED0=!LED0;//�����˸��ʾ2s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 8 S",8);
	LED0=!LED0;//�����˸��ʾ3s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 7 S",8);
	LED0=!LED0;//�����˸��ʾ4s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 6 S",8);
	LED0=!LED0;//�����˸��ʾ5s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 5 S",8);
	LED0=!LED0;//�����˸��ʾ6s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 4 S",8);
	LED0=!LED0;//�����˸��ʾ7s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 3 S",8);
	LED0=!LED0;//�����˸��ʾ8s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 2 S",8);
	LED0=!LED0;//�����˸��ʾ9s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 1 S",8);
	LED0=!LED0;//�����˸��ʾ10s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//�����˸��ʾ
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
}
