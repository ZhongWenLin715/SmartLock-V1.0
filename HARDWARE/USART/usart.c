/**********************************************************************************
 * 文件名  ：usart.c
 * 描述    ：usart1 应用函数库          
 * 实验平台：NiRen_TwoHeart系统板
 * 硬件连接：TXD(PB9)  -> 外部串口RXD     
 *           RXD(PB10) -> 外部串口TXD      
 *           GND	   -> 外部串口GND 
 * 库版本  ：ST_v3.5
**********************************************************************************/

#include "usart.h"
#include "delay.h"
#include "GA6_module_errors.h"
#include "string.h"

char Uart1_Buf[Buf1_Max];//串口2接收缓存
u8 First_Int = 0;
char error_result[20];

u8  receive_mode     = 0;
u8  find_string_flag = 0;
u8  has_call_flag = 0;
char num_message[4];//暂时假设最多只能存999条短信
char AT_CMGR[15];

//static char *phone_num = "ATD10010;"; //拨打电话，修改这里可以修改拨打的电话
static char  *phone_num = "AT+CMGS=\"18843153389\""; //修改这里可以修改发送短信的电话号码
//PDU转换参考此网页:http://www.multisilicon.com/blog/a22201774~/pdu.htm
//注:(1) 短信中心号码保持为空，也就是不要填写。  (2)接收方号码需要 加上+86，例如: +8615815555555  (3)字符位数 选中16  (4)PDU消息的内容不要多复制空格
char *AT_CMGS = "AT+CMGS=27";	//用于存放AT+CMGS=XX的指令,XX数值要参考网页上面转换后的结果值
char *pdu_content = "0011000D916xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

/*******************************************************************************
* 函数名  : USART1_Init_Config
* 描述    : USART1初始化配置
* 输入    : bound：波特率(常用：2400、4800、9600、19200、38400、115200等)
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void USART1_Init_Config(u32 bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	/*使能USART1和GPIOA外设时钟*/  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	

	/*复位串口1*/
	USART_DeInit(USART1);  

	/*USART1_GPIO初始化设置*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//USART1_TXD(PA.9)     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//设置引脚输出最大速率为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//调用库函数中的GPIO初始化函数，初始化USART1_TXD(PA.9)  


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//USART1_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//调用库函数中的GPIO初始化函数，初始化USART1_RXD(PA.10)


	/*USART1 初始化设置*/
	USART_InitStructure.USART_BaudRate = bound;										//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//工作模式设置为收发模式
	USART_Init(USART1, &USART_InitStructure);										//初始化串口1

	/*Usart1 NVIC配置*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//从优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							//根据指定的参数初始化VIC寄存器 

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//使能串口1接收中断

	USART_Cmd(USART1, ENABLE);                    			//使能串口 
	USART_ClearFlag(USART1, USART_FLAG_TC);					//清除发送完成标志
}

/*******************************************************************************
* 函数名  : UART1_SendString
* 描述    : USART1发送字符串
* 输入    : *s字符串指针
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void UART1_SendString(char* s)
{
	while(*s)//检测字符串结束符
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
		USART_SendData(USART1 ,*s++);//发送当前字符
	}
}

void UART1_Send_Command(char* s)
{
	CLR_Buf(); //清空接收数据的buffer
	UART1_SendString(s); //发出字符串
	UART1_SendString("\r\n"); //再自动发送 \r\n两个字符
}

/*******************************************************************************
* 函数名 : Second_AT_Command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、希望收到的应答、发送等待时间(ms)
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/

u8 UART1_Send_AT_Command(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //如果没有找到 就继续再发一次指令 再进行查找目标字符串                
	{
		UART1_Send_Command(b);//串口2发送 b 字符串 他会自动发送\r\n  相当于发送了一个指令
		delay_ms(interval_time); //等待一定时间
        delay_ms(interval_time); //等待一定时间        
		if(Find(a))            //查找需要应答的字符串 a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

void UART1_Send_Command_END(char* s)
{
	CLR_Buf(); //清空接收数据的buffer
	UART1_SendString(s); //发出字符串
}

u8 UART1_Send_AT_Command_End(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //如果没有找到 就继续再发一次指令 再进行查找目标字符串                
	{
		UART1_Send_Command_END(b);//串口2发送 b 字符串 这里不发送\r\n 
		delay_ms(interval_time); //等待一定时间
        delay_ms(interval_time); //等待一定时间
        delay_ms(interval_time); //等待一定时间
        delay_ms(interval_time); //等待一定时间
        delay_ms(interval_time); //等待一定时间
        delay_ms(interval_time); //等待一定时间        
		if(Find(a))            //查找需要应答的字符串 a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

/*******************************************************************************
* 函数名  : USART1_IRQHandler
* 描述    : 串口1中断服务程序
* 输入    : 无
* 返回    : 无 
* 说明    : 
*******************************************************************************/
void USART1_IRQHandler(void)                	
{
	u8 Res=0;
	Res = USART_ReceiveData(USART1);
	Uart1_Buf[First_Int] = Res;  	  //将接收到的字符串存到缓存中
	First_Int++;                	  //缓存指针向后移动
	if(First_Int > Buf1_Max)       	  //如果缓存满,将缓存指针指向缓存的首地址
	{
		First_Int = 0;
	}
    if(receive_mode == 1)
	{
		if(First_Int >= 6)
		{
			find_string_flag = 1;
		}
	}
} 	

/*******************************************************************************
* 函数名  : TIM2_IRQHandler
* 描述    : 定时器2中断断服务函数
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void TIM2_IRQHandler(void)   //TIM2中断
{
	static u8 flag =1;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIM2更新中断标志 
	
		if(flag)
		{
			//LED4_ON(); 
			flag=0;
		}
		else
		{
			//LED4_OFF(); 
			flag=1;
		}
	}	
}

/*******************************************************************************
* 函数名 : CLR_Buf
* 描述   : 清除串口2缓存数据
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void CLR_Buf(void)
{
	u16 k;
	for(k=0;k<Buf1_Max;k++)      //将缓存内容清零
	{
		Uart1_Buf[k] = 0x00;
	}
    First_Int = 0;              //接收字符串的起始存储位置
}

/*******************************************************************************
* 函数名 : Wait_CREG
* 描述   : 等待模块注册成功
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
u8 Wait_CREG(u8 query_times)
{
	u8 i;
	u8 k;
	u8 j;
	i = 0;
	CLR_Buf();
	while(i == 0)        			
	{

		UART1_Send_Command("AT+CREG?");
		delay_ms(1000);
        delay_ms(1000);        
		
		for(k=0;k<Buf1_Max;k++)      			
		{
			if((Uart1_Buf[k] == '+')&&(Uart1_Buf[k+1] == 'C')&&(Uart1_Buf[k+2] == 'R')&&(Uart1_Buf[k+3] == 'E')&&(Uart1_Buf[k+4] == 'G')&&(Uart1_Buf[k+5] == ':'))
			{
					 
				if((Uart1_Buf[k+7] == '1')&&((Uart1_Buf[k+9] == '1')||(Uart1_Buf[k+9] == '5')))
				{
					i = 1;
					return 1;
				}
				
			}
		}
		j++;
		if(j > query_times)
		{
			return 0;
		}
		
	}
	return 0;
}

/*******************************************************************************
* 函数名 : Find
* 描述   : 判断缓存中是否含有指定的字符串
* 输入   : 
* 输出   : 
* 返回   : unsigned char:1 找到指定字符，0 未找到指定字符 
* 注意   : 
*******************************************************************************/

u8 Find(char *a)
{ 
	if(strstr(Uart1_Buf, a)!=NULL)
	{
		return 1;
	}	
	else
	{
		return 0;
	}
		
}
/*******************************************************************/


/****************************检查模块状态****************************/

int check_status(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT","OK",3,500);//测试通信是否成功
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CPIN?","READY",2,500);//查询卡是否插上
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//查询卡是否注册到网络
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	return 1;
}

/*********************************************************************/


/********************** 设置接收短信位置方式 / 读短信 ********************/
int set_text_mode(void)
{
	u8 ret;
	
	//设置存储位置
	ret = UART1_Send_AT_Command("AT+CPMS=\"SM\",\"ME\",\"SM\"","OK",3,1000);
	if(ret == 0)
	{
		return AT_CPMS_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CMGF=1","OK",3,500);//配置为TEXT模式
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,500);//设置字符格式
	if(ret == 0)
	{
		return AT_CSCS_ERROR;
	}
	return 1;
}

int read_message(void)
{
	char *buffer_ptr;
	u8 i,len;
	u8 ret;
	
	memset(num_message,'\0',4);
	memset(AT_CMGR,'\0',15);
	buffer_ptr = strstr(Uart1_Buf,"\"SM\",");
	
	len = strlen("\"SM\",");

	i = len;
	if(buffer_ptr != NULL)
	{
		while(*(buffer_ptr + i) != 0x0D && i < (len+3))
		{
			num_message[i-len] = *(buffer_ptr + i);
			i++;
		}
	}
	else
	{
		return NO_SMS_LEN_ERROR;
	}
	
	memset(AT_CMGR,'\0',15);
	strcpy(AT_CMGR,"AT+CMGR=");
	strcat(AT_CMGR,num_message);
	
	ret = UART1_Send_AT_Command(AT_CMGR,"OK",3,500);

	return ret;
}

/***************************************************************/


/***************************发送英文短信************************/
int send_text_message(char *content)
{
	u8 ret;
	char end_char[2];
	
	end_char[0] = 0x1A;//结束字符
	end_char[1] = '\0';
	
	//设置存储位置
	ret = UART1_Send_AT_Command("AT+CPMS=\"SM\",\"ME\",\"SM\"","OK",3,1000);
	if(ret == 0)
	{
		return AT_CPMS_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CMGF=1","OK",3,500);//配置为TEXT模式
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,500);//设置字符格式
	if(ret == 0)
	{
		return AT_CSCS_ERROR;
	}
	
	ret = UART1_Send_AT_Command(phone_num,">",3,500);//输入收信人的电话号码
	if(ret == 0)
	{
		return AT_CMGS_ERROR;
	}
	
	UART1_SendString(content);
	ret = UART1_Send_AT_Command_End(end_char,"OK",1,1000);//发送结束符，等待返回ok,等待5S发一次，因为短信成功发送的状态时间比较长
	if(ret == 0)
	{
		return END_CHAR_ERROR;
	}
	
	return 1;
}
/****************************************************************/


/*************************发送中文短信***************************/
int send_pdu_message(char *content)
{
	int ret;
	char end_char[2];
	
	end_char[0] = 0x1A;
	end_char[1] = '\0';
	
	//设置存储位置，不做返回值判断了
	ret = UART1_Send_AT_Command("AT+CPMS=\"SM\",\"ME\",\"SM\"","OK",3,1000);
	if(ret == 0)
	{
		return AT_CPMS_ERROR;
	}
		
	ret = UART1_Send_AT_Command("AT+CMGF=0","OK",3,500);//配置为PDU模式
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART1_Send_AT_Command(AT_CMGS,">",3,500);//发送字符个数指令
	
	if(ret == 0)
	{
		return AT_CMGS_ERROR;
	}
	
	UART1_SendString(content);
	ret = UART1_Send_AT_Command_End(end_char,"OK",3,500);//发送结束符，等待返回ok,等待5S发一次，因为短信成功发送的状态时间比较长
	if(ret == 0)
	{
		return END_CHAR_ERROR;
	}
	
	return 1;
}

/****************************************************************/


/************************* 打电话 ******************************/

int call_phone_num(char *phone)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT","OK",3,500);//测试通信是否成功
	
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CPIN?","READY",2,500);//查询卡是否插上
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//查询卡是否注册到网络
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	
	
	ret = UART1_Send_AT_Command(phone,"OK",2,700);//拨号
	if(ret == 0)
	{
		return CALL_ERROR;
	}
	
	return 1;
}

int redail_phone_num(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT+DLST","OK",3,500);//重拨 如果返回OK 说明指令发送成功 不代表正在呼叫
	
	if(ret == 0)
	{
		return AT_DLST_OK_ERROR;
	}
    delay_ms(1000);
    delay_ms(1000);    
 //再等待2秒  不然拨号的时候的串口信息没法收上来
    if(Find("\"CALL\",1") == 0)
    {
        return AT_DLST_CALL_ERROR;
    }
    
    return 1;
}

/****************************************************************/


/************************ 接电话/挂电话 *************************/

int ack_call(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT","OK",3,500);//测试通信是否成功
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CPIN?","READY",2,500);//查询卡是否插上
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//查询卡是否注册到网络
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	
	
	ret = UART1_Send_AT_Command("ATA","CONNECT",2,500);//接听电话
	delay_ms(1000);
    delay_ms(1000);     
    //延时为了能够就收到模块的返回数据
    if(ret == 0)
	{
		return ATA_ERROR;
	}
	
	return 1;
}

int hang_call(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("ATH","OK",2,500);//挂断电话
	if(ret == 0)
	{
		return ATH_ERROR;
	}
	
	return 1;
}

/****************************************************************/












