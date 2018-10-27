#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"

void KEY_Init(void) 
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);//使能PORTF时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;//KEY0-KEY3  矩阵键盘的行
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //设置成推挽输出  GPIO_Mode_Out_OD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//KEY4-KEY7  矩阵键盘的列
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      //设置成上拉输入  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7); 
	

}
u8 keyscan()
{
    uint8_t temp;
    uint8_t key=0;
    GPIO_Write(GPIOF, 0xFE);
    temp = GPIO_ReadInputData(GPIOF);
    temp = temp&0xf0;
    if(temp!=0xf0)
    {
	    delay_ms(10);
        temp = GPIO_ReadInputData(GPIOF);
        temp = temp&0xf0;
        if(temp!=0xf0)
        {
		    temp = GPIO_ReadInputData(GPIOF);
            switch(temp)
            {
			    case 0xee: key=1; break;
				case 0xde: key=2; break;
				case 0xbe: key=3; break;
				case 0x7e: key=13; break;
                default:break;
			}
            while(temp!=0xf0)
            {
			    temp = GPIO_ReadInputData(GPIOF);
                temp = temp&0xf0;    
			}			
		} 		
	}
	
	GPIO_Write(GPIOF, 0xFD);
    temp = GPIO_ReadInputData(GPIOF);
    temp = temp&0xf0;
    if(temp!=0xf0)
    {
	    delay_ms(10);
        temp = GPIO_ReadInputData(GPIOF);
        temp = temp&0xf0;
        if(temp!=0xf0)
        {
		    temp = GPIO_ReadInputData(GPIOF);
            switch(temp)
            {
			    case 0xed: key=4; break;
				case 0xdd: key=5; break;
				case 0xbd: key=6; break;
				case 0x7d: key=14; break;
                default:break;
			}
            while(temp!=0xf0)
            {
			    temp = GPIO_ReadInputData(GPIOF);
                temp = temp&0xf0;    
			}			
		} 		
	}
	
	GPIO_Write(GPIOF, 0xFB);
    temp = GPIO_ReadInputData(GPIOF);
    temp = temp&0xf0;
    if(temp!=0xf0)
    {
	    delay_ms(10);
        temp = GPIO_ReadInputData(GPIOF);
        temp = temp&0xf0;
        if(temp!=0xf0)
        {
		    temp = GPIO_ReadInputData(GPIOF);
            switch(temp)
            {
			    case 0xeb: key=7; break;
				case 0xdb: key=8; break;
				case 0xbb: key=9; break;
				case 0x7b: key=15; break;
                default:break;
			}
            while(temp!=0xf0)
            {
			    temp = GPIO_ReadInputData(GPIOF);
                temp = temp&0xf0;    
			}			
		} 		
	}
	
    GPIO_Write(GPIOF, 0xF7);
    temp = GPIO_ReadInputData(GPIOF);
    temp = temp&0xf0;
    if(temp!=0xf0)
    {
	    delay_ms(10);
        temp = GPIO_ReadInputData(GPIOF);
        temp = temp&0xf0;
        if(temp!=0xf0)
        {
		    temp = GPIO_ReadInputData(GPIOF);
            switch(temp)
            {
			    case 0xe7: key=10; break;
				case 0xd7: key=11; break;
				case 0xb7: key=12; break;
				case 0x77: key=16; break;
                default:break;
			}
            while(temp!=0xf0)
            {
			    temp = GPIO_ReadInputData(GPIOF);
                temp = temp&0xf0;    
			}			
		} 		
	}
    return key; 	 

}




