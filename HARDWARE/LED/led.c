#include "led.h"
#include "delay.h"
#include "oled.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 精英STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.5 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5端口配置, 推挽输出
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOB,GPIO_Pin_5); 						 //PE.5输出低

    
}
 
void fall_for_10s(void)
{
    OLED_ShowString(10,2," Wait for 10S",8);	
	LED0=!LED0;//红灯闪烁提示1s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
   
    OLED_ShowString(10,2," Wait for 9 S",8);    
	LED0=!LED0;//红灯闪烁提示2s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 8 S",8);
	LED0=!LED0;//红灯闪烁提示3s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 7 S",8);
	LED0=!LED0;//红灯闪烁提示4s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 6 S",8);
	LED0=!LED0;//红灯闪烁提示5s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 5 S",8);
	LED0=!LED0;//红灯闪烁提示6s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 4 S",8);
	LED0=!LED0;//红灯闪烁提示7s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 3 S",8);
	LED0=!LED0;//红灯闪烁提示8s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 2 S",8);
	LED0=!LED0;//红灯闪烁提示9s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
    OLED_ShowString(10,2," Wait for 1 S",8);
	LED0=!LED0;//红灯闪烁提示10s
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	LED0=!LED0;//红灯闪烁提示
	delay_ms(100);
	LED0=!LED0;
	delay_ms(100);
	delay_ms(500);
    
}
