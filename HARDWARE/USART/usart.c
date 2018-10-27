/**********************************************************************************
 * �ļ���  ��usart.c
 * ����    ��usart1 Ӧ�ú�����          
 * ʵ��ƽ̨��NiRen_TwoHeartϵͳ��
 * Ӳ�����ӣ�TXD(PB9)  -> �ⲿ����RXD     
 *           RXD(PB10) -> �ⲿ����TXD      
 *           GND	   -> �ⲿ����GND 
 * ��汾  ��ST_v3.5
**********************************************************************************/

#include "usart.h"
#include "delay.h"
#include "GA6_module_errors.h"
#include "string.h"

char Uart1_Buf[Buf1_Max];//����2���ջ���
u8 First_Int = 0;
char error_result[20];

u8  receive_mode     = 0;
u8  find_string_flag = 0;
u8  has_call_flag = 0;
char num_message[4];//��ʱ�������ֻ�ܴ�999������
char AT_CMGR[15];

//static char *phone_num = "ATD10010;"; //����绰���޸���������޸Ĳ���ĵ绰
static char  *phone_num = "AT+CMGS=\"18843153389\""; //�޸���������޸ķ��Ͷ��ŵĵ绰����
//PDUת���ο�����ҳ:http://www.multisilicon.com/blog/a22201774~/pdu.htm
//ע:(1) �������ĺ��뱣��Ϊ�գ�Ҳ���ǲ�Ҫ��д��  (2)���շ�������Ҫ ����+86������: +8615815555555  (3)�ַ�λ�� ѡ��16  (4)PDU��Ϣ�����ݲ�Ҫ�ิ�ƿո�
char *AT_CMGS = "AT+CMGS=27";	//���ڴ��AT+CMGS=XX��ָ��,XX��ֵҪ�ο���ҳ����ת����Ľ��ֵ
char *pdu_content = "0011000D916xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

/*******************************************************************************
* ������  : USART1_Init_Config
* ����    : USART1��ʼ������
* ����    : bound��������(���ã�2400��4800��9600��19200��38400��115200��)
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void USART1_Init_Config(u32 bound)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	/*ʹ��USART1��GPIOA����ʱ��*/  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	

	/*��λ����1*/
	USART_DeInit(USART1);  

	/*USART1_GPIO��ʼ������*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//USART1_TXD(PA.9)     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//������������������Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//���ÿ⺯���е�GPIO��ʼ����������ʼ��USART1_TXD(PA.9)  


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				//USART1_RXD(PA.10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//���ÿ⺯���е�GPIO��ʼ����������ʼ��USART1_RXD(PA.10)


	/*USART1 ��ʼ������*/
	USART_InitStructure.USART_BaudRate = bound;										//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//����ģʽ����Ϊ�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);										//��ʼ������1

	/*Usart1 NVIC����*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;	//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//ʹ�ܴ���1�����ж�

	USART_Cmd(USART1, ENABLE);                    			//ʹ�ܴ��� 
	USART_ClearFlag(USART1, USART_FLAG_TC);					//���������ɱ�־
}

/*******************************************************************************
* ������  : UART1_SendString
* ����    : USART1�����ַ���
* ����    : *s�ַ���ָ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void UART1_SendString(char* s)
{
	while(*s)//����ַ���������
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
		USART_SendData(USART1 ,*s++);//���͵�ǰ�ַ�
	}
}

void UART1_Send_Command(char* s)
{
	CLR_Buf(); //��ս������ݵ�buffer
	UART1_SendString(s); //�����ַ���
	UART1_SendString("\r\n"); //���Զ����� \r\n�����ַ�
}

/*******************************************************************************
* ������ : Second_AT_Command
* ����   : ����ATָ���
* ����   : �������ݵ�ָ�롢ϣ���յ���Ӧ�𡢷��͵ȴ�ʱ��(ms)
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/

u8 UART1_Send_AT_Command(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //���û���ҵ� �ͼ����ٷ�һ��ָ�� �ٽ��в���Ŀ���ַ���                
	{
		UART1_Send_Command(b);//����2���� b �ַ��� �����Զ�����\r\n  �൱�ڷ�����һ��ָ��
		delay_ms(interval_time); //�ȴ�һ��ʱ��
        delay_ms(interval_time); //�ȴ�һ��ʱ��        
		if(Find(a))            //������ҪӦ����ַ��� a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

void UART1_Send_Command_END(char* s)
{
	CLR_Buf(); //��ս������ݵ�buffer
	UART1_SendString(s); //�����ַ���
}

u8 UART1_Send_AT_Command_End(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //���û���ҵ� �ͼ����ٷ�һ��ָ�� �ٽ��в���Ŀ���ַ���                
	{
		UART1_Send_Command_END(b);//����2���� b �ַ��� ���ﲻ����\r\n 
		delay_ms(interval_time); //�ȴ�һ��ʱ��
        delay_ms(interval_time); //�ȴ�һ��ʱ��
        delay_ms(interval_time); //�ȴ�һ��ʱ��
        delay_ms(interval_time); //�ȴ�һ��ʱ��
        delay_ms(interval_time); //�ȴ�һ��ʱ��
        delay_ms(interval_time); //�ȴ�һ��ʱ��        
		if(Find(a))            //������ҪӦ����ַ��� a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

/*******************************************************************************
* ������  : USART1_IRQHandler
* ����    : ����1�жϷ������
* ����    : ��
* ����    : �� 
* ˵��    : 
*******************************************************************************/
void USART1_IRQHandler(void)                	
{
	u8 Res=0;
	Res = USART_ReceiveData(USART1);
	Uart1_Buf[First_Int] = Res;  	  //�����յ����ַ����浽������
	First_Int++;                	  //����ָ������ƶ�
	if(First_Int > Buf1_Max)       	  //���������,������ָ��ָ�򻺴���׵�ַ
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
* ������  : TIM2_IRQHandler
* ����    : ��ʱ��2�ж϶Ϸ�����
* ����    : ��
* ���    : ��
* ����    : �� 
* ˵��    : ��
*******************************************************************************/
void TIM2_IRQHandler(void)   //TIM2�ж�
{
	static u8 flag =1;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIM2�����жϱ�־ 
	
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
* ������ : CLR_Buf
* ����   : �������2��������
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void CLR_Buf(void)
{
	u16 k;
	for(k=0;k<Buf1_Max;k++)      //��������������
	{
		Uart1_Buf[k] = 0x00;
	}
    First_Int = 0;              //�����ַ�������ʼ�洢λ��
}

/*******************************************************************************
* ������ : Wait_CREG
* ����   : �ȴ�ģ��ע��ɹ�
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
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
* ������ : Find
* ����   : �жϻ������Ƿ���ָ�����ַ���
* ����   : 
* ���   : 
* ����   : unsigned char:1 �ҵ�ָ���ַ���0 δ�ҵ�ָ���ַ� 
* ע��   : 
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


/****************************���ģ��״̬****************************/

int check_status(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT","OK",3,500);//����ͨ���Ƿ�ɹ�
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CPIN?","READY",2,500);//��ѯ���Ƿ����
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//��ѯ���Ƿ�ע�ᵽ����
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	return 1;
}

/*********************************************************************/


/********************** ���ý��ն���λ�÷�ʽ / ������ ********************/
int set_text_mode(void)
{
	u8 ret;
	
	//���ô洢λ��
	ret = UART1_Send_AT_Command("AT+CPMS=\"SM\",\"ME\",\"SM\"","OK",3,1000);
	if(ret == 0)
	{
		return AT_CPMS_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CMGF=1","OK",3,500);//����ΪTEXTģʽ
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,500);//�����ַ���ʽ
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


/***************************����Ӣ�Ķ���************************/
int send_text_message(char *content)
{
	u8 ret;
	char end_char[2];
	
	end_char[0] = 0x1A;//�����ַ�
	end_char[1] = '\0';
	
	//���ô洢λ��
	ret = UART1_Send_AT_Command("AT+CPMS=\"SM\",\"ME\",\"SM\"","OK",3,1000);
	if(ret == 0)
	{
		return AT_CPMS_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CMGF=1","OK",3,500);//����ΪTEXTģʽ
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,500);//�����ַ���ʽ
	if(ret == 0)
	{
		return AT_CSCS_ERROR;
	}
	
	ret = UART1_Send_AT_Command(phone_num,">",3,500);//���������˵ĵ绰����
	if(ret == 0)
	{
		return AT_CMGS_ERROR;
	}
	
	UART1_SendString(content);
	ret = UART1_Send_AT_Command_End(end_char,"OK",1,1000);//���ͽ��������ȴ�����ok,�ȴ�5S��һ�Σ���Ϊ���ųɹ����͵�״̬ʱ��Ƚϳ�
	if(ret == 0)
	{
		return END_CHAR_ERROR;
	}
	
	return 1;
}
/****************************************************************/


/*************************�������Ķ���***************************/
int send_pdu_message(char *content)
{
	int ret;
	char end_char[2];
	
	end_char[0] = 0x1A;
	end_char[1] = '\0';
	
	//���ô洢λ�ã���������ֵ�ж���
	ret = UART1_Send_AT_Command("AT+CPMS=\"SM\",\"ME\",\"SM\"","OK",3,1000);
	if(ret == 0)
	{
		return AT_CPMS_ERROR;
	}
		
	ret = UART1_Send_AT_Command("AT+CMGF=0","OK",3,500);//����ΪPDUģʽ
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART1_Send_AT_Command(AT_CMGS,">",3,500);//�����ַ�����ָ��
	
	if(ret == 0)
	{
		return AT_CMGS_ERROR;
	}
	
	UART1_SendString(content);
	ret = UART1_Send_AT_Command_End(end_char,"OK",3,500);//���ͽ��������ȴ�����ok,�ȴ�5S��һ�Σ���Ϊ���ųɹ����͵�״̬ʱ��Ƚϳ�
	if(ret == 0)
	{
		return END_CHAR_ERROR;
	}
	
	return 1;
}

/****************************************************************/


/************************* ��绰 ******************************/

int call_phone_num(char *phone)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT","OK",3,500);//����ͨ���Ƿ�ɹ�
	
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CPIN?","READY",2,500);//��ѯ���Ƿ����
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//��ѯ���Ƿ�ע�ᵽ����
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	
	
	ret = UART1_Send_AT_Command(phone,"OK",2,700);//����
	if(ret == 0)
	{
		return CALL_ERROR;
	}
	
	return 1;
}

int redail_phone_num(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT+DLST","OK",3,500);//�ز� �������OK ˵��ָ��ͳɹ� ���������ں���
	
	if(ret == 0)
	{
		return AT_DLST_OK_ERROR;
	}
    delay_ms(1000);
    delay_ms(1000);    
 //�ٵȴ�2��  ��Ȼ���ŵ�ʱ��Ĵ�����Ϣû��������
    if(Find("\"CALL\",1") == 0)
    {
        return AT_DLST_CALL_ERROR;
    }
    
    return 1;
}

/****************************************************************/


/************************ �ӵ绰/�ҵ绰 *************************/

int ack_call(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("AT","OK",3,500);//����ͨ���Ƿ�ɹ�
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART1_Send_AT_Command("AT+CPIN?","READY",2,500);//��ѯ���Ƿ����
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//��ѯ���Ƿ�ע�ᵽ����
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	
	
	ret = UART1_Send_AT_Command("ATA","CONNECT",2,500);//�����绰
	delay_ms(1000);
    delay_ms(1000);     
    //��ʱΪ���ܹ����յ�ģ��ķ�������
    if(ret == 0)
	{
		return ATA_ERROR;
	}
	
	return 1;
}

int hang_call(void)
{
	int ret;
	
	ret = UART1_Send_AT_Command("ATH","OK",2,500);//�Ҷϵ绰
	if(ret == 0)
	{
		return ATH_ERROR;
	}
	
	return 1;
}

/****************************************************************/












