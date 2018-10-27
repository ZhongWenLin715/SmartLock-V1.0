#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

#define Buf1_Max 	  100 //串口2缓存长度


void USART1_Init_Config(u32 bound);
void UART1_SendString(char* s);
void UART1_SendLR(void);
void UART1_Send_Command(char* s);
u8 UART1_Send_AT_Command(char *b,char *a,u8 wait_time,u32 interval_time);
void UART1_Send_Command_END(char* s); 
u8 UART1_Send_AT_Command_End(char *b,char *a,u8 wait_time,u32 interval_time);


//串口2发送回车换行
#define UART1_SendLR() UART1_SendString("\r\n")

void CLR_Buf(void);
u8 Wait_CREG(u8 query_times);    //等待模块注册成功
u8 Find(char *a);

int check_status(void);
int set_text_mode(void);
int read_message(void);
int send_text_message(char *content);
int send_pdu_message(char *content);
int call_phone_num(char *phone);
int redail_phone_num(void);
int ack_call(void);
int hang_call(void);

#endif


