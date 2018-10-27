#include "sys.h"
#include "delay.h"
#include "beep.h"
#include "usart2.h"
#include "key.h"
#include "led.h"
#include "stmflash.h"
#include "string.h"
#include "oled.h" 
#include "as608.h"
#include "timer2.h"
#include "usart.h"
#include "bmp.h"
#include "adc.h"
#include "usart3.h"
#include "timer3.h"
#include "gizwits_product.h"

//ALIENTEK ս��V3&��Ӣ STM32������ ��չʵ��14
//ATK-AS608ָ��ʶ��ģ��ʵ��-�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾    
//���ߣ�����ԭ�� @ALIENTEK 
u8 *in_pwd1=0;
u8 *in_pwd2=0;
u8 *in_pwd3=0;
u8 *in_pwd4=0;
u8 *in_pwd5=0;
u8 *in_pwd6=0;
u8 *in_pwd7=0;
u8 *in_pwd8=0;
u8 *str=0; 	
int fall_time=0;  
int in_time=0;
int mykey=0;//����in_timeÿ��һ������ֻ��һ�εĿ���
u8 root_pwd[8]="66666666";//����Ա����
u8 open_pwd[8]="";//��������
#define SIZE_PWD sizeof(open_pwd)
#define FLASH_SAVE_ADDR  0X08070000	

#define usart2_baund  57600//����2�����ʣ�����ָ��ģ�鲨���ʸ���

SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чָ�Ƹ���

void root_h(void);//����Ա����

void Add_FR(void);	//¼ָ��
void Del_FR(void);	//ɾ��ָ��
void press_FR(void);//ˢָ��
u16 GET_NUM(void);//��ȡ��ֵ
void AS608_Details(void);//��ȡģ������
 
extern u8  find_string_flag;
extern u8  receive_mode;
//Э���ʼ��
void Gizwits_Init(void)
{
	
	TIM3_Int_Init(9,7199);//1MSϵͳ��ʱ
    usart3_init(9600);//WIFI��ʼ��
	userInit();//�豸״̬�ṹ���ʼ��
	gizwitsInit();//��������ʼ��
}

int main(void)
{    
	u8 key_num=0;	
    u8 *str=0;
    int  ret;
    u16 adcx;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init();  	//��ʼ����ʱ����
	usart2_init(usart2_baund);//��ʼ������2,������ָ��ģ��ͨѶ
    USART1_Init_Config(115200);  //����1��������A6ģ��ͨ��
	Timer2_Init_Config();        //��ʱ��2��ʼ��������A6ģ��
	PS_StaGPIO_Init();	//��ʼ��FR��״̬����
	BEEP_Init();  			//��ʼ��������    			     
	KEY_Init();
	OLED_Init();    
    LED_Init();
    Adc_Init();
    Gizwits_Init();         //Э���ʼ��
        
    receive_mode = 1;
    
	OLED_Clear();
	//��AS608ģ������
	do
    {
        Moto=1;
        OLED_ShowString(0,0,"AS608 Ready",8);
		delay_ms(400);
        OLED_Clear();
		OLED_ShowString(0,2,"try connection",8);
		OLED_ShowString(0,5,"wait...",8);
        delay_ms(800); 
        OLED_Clear(); 
                
	}
    while(PS_HandShake(&AS608Addr));
    
	
	while(1)
	{
        STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)open_pwd,SIZE_PWD);
        	
	    OLED_ShowString(0,0,"Enter 8 password",16);
        OLED_ShowString(0,2,"Fingerprint",16);      
        OLED_ShowNum(0,5,adcx,4,12);//��ʾADC��ֵ
        OLED_DrawBMP(88,3,128,8,BMP2); 
        
        
/***************************ָ�ƽ���*************************************/ 
        
        if(PS_Sta)	 //���PS_Sta״̬���������ָ����
		{
			press_FR();//ˢָ��			
		}
        
/***************************���Ž���*************************************/
      /*  set_text_mode();//���ö���
        CLR_Buf();        
        if(find_string_flag == 1)
		{
            
			delay_ms(200);//��ʱһ�㣬�ô��ڰ����ݽ������
			if(Find("+CMTI"))//˵�����յ��˶���
			{
				ret = read_message();
				if(ret == 1)
				{
					if(Find("ledon"))
					{
						CLR_Buf();
						find_string_flag = 0;
                        delay_ms(200);
                        
                        OLED_Clear();//���֮ǰ���������
					    OLED_ShowString(30,0,"Welcome",8);	 
					    OLED_ShowString(0,2,"open the door!",8);
                        delay_ms(1000);
                        OLED_Clear();
					}
					if(Find("ledoff"))
					{						
						CLR_Buf();
						find_string_flag = 0;
                        delay_ms(200);
                        
                        OLED_Clear();//���֮ǰ���������	 
					    OLED_ShowString(0,2,"close the door!",8);
                        delay_ms(1000);
                        OLED_Clear();
					}
				}
			}
        } */
 /***************************�������**********************************/       
		key_num=keyscan();	
        if(key_num)
        {
           switch(key_num)
           {
                case 1:Del_FR();        key_num=0;break; //ɾָ��
                case 2:AS608_Details(); key_num=0;break; //����
                case 3:Add_FR();        key_num=0;break; //¼ָ��
               
                case 13:gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-linkģʽ����
                        LED0 = 0;
                        delay_ms(1000);
                        LED0 = 1;
                        key_num=0;
                        break;
               
                case 16: ret = check_status();
                         if(ret == 1)
                         { 
                             ret = send_text_message("SOS!This is a message for help"); 
                             delay_ms(1000);
                             delay_ms(1000);                              
                         };
                         key_num=0;
                         break;
                         
				default: break;	
           }
            
        }
		if(key_num>0)
		{	
                        
            switch(key_num)
			{  
				case 14:str="0";break;			   
				case 4:str="1";break;	    
				case 5:str="2";break;	    
				case 6:str="3";break;		 
				case 7:str="4";break;		  
				case 8:str="5";break;	   
				case 9:str="6";break;		  
				case 10:str="7";break;		  
				case 11:str="8";break;		   
				case 12:str="9";break;
                default: break;	                
			}
			
			if(mykey==0)
			{
				in_time++;
					switch(in_time)
					{
						case 1:in_pwd1=str;OLED_ShowString(0,8,str,8);delay_ms(200);OLED_ShowString(0,8,"*",8);break;
						case 2:in_pwd2=str;OLED_ShowString(11,8,str,8);delay_ms(200);OLED_ShowString(11,8,"*",8);break;
						case 3:in_pwd3=str;OLED_ShowString(23,8,str,8);delay_ms(200);OLED_ShowString(23,8,"*",8);break;
						case 4:in_pwd4=str;OLED_ShowString(35,8,str,8);delay_ms(200);OLED_ShowString(35,8,"*",8);break;
						case 5:in_pwd5=str;OLED_ShowString(47,8,str,8);delay_ms(200);OLED_ShowString(47,8,"*",8);break;
						case 6:in_pwd6=str;OLED_ShowString(59,8,str,8);delay_ms(200);OLED_ShowString(59,8,"*",8);break;
						case 7:in_pwd7=str;OLED_ShowString(71,8,str,8);delay_ms(200);OLED_ShowString(71,8,"*",8);break;
						case 8:in_pwd8=str;OLED_ShowString(83,8,str,8);delay_ms(200);OLED_ShowString(83,8,"*",8);break;

					}								
				mykey=1;//����Ϊ1��ֹ�ٴν���
			}
			
			if(in_time==8)
			{
				if( //��ͨ�û���Գɹ�
					*in_pwd3==open_pwd[2]&&
				    *in_pwd4==open_pwd[3]&&
				    *in_pwd5==open_pwd[4]&&
			        *in_pwd6==open_pwd[5])
				{
					fall_time=0;
					OLED_Clear();//���֮ǰ���������
                    delay_ms(200);
					OLED_ShowString(30,0,"Welcome",8);	 
					OLED_ShowString(0,2,"open the door!",8);
                    Moto=0;
                    delay_ms(1000);
                    Moto=1;
                    delay_ms(1000);
                    OLED_Clear();
				}
				else if( *in_pwd1==root_pwd[0]&&//����Ա�û���Գɹ�
						 *in_pwd2==root_pwd[1]&&
						 *in_pwd3==root_pwd[2]&&
						 *in_pwd4==root_pwd[3]&&
						 *in_pwd5==root_pwd[4]&&
						 *in_pwd6==root_pwd[5]&&
						 *in_pwd7==root_pwd[6]&&
						 *in_pwd8==root_pwd[7])
				{
					 root_h();                     
				}
				else//���ʧ��
				{
					OLED_Clear();//���֮ǰ���������
                    delay_ms(200);
					OLED_ShowString(0,0,"*Wrong Password*",8);
                    OLED_ShowString(0,3,"please try again",8);
					fall_time++;
					LED0=!LED0;//�����˸��ʾ
					delay_ms(300);
					LED0=!LED0;
					delay_ms(300);
					LED0=!LED0;
					delay_ms(300);
					LED0=!LED0;
					LED0=1;
                    OLED_Clear();
                    
				}
				in_time=0;
			}
		    if(fall_time==5)
		    {
                ret = check_status();
                if(ret == 1)
                { 
                    ret = send_text_message("You have entered 5 error passwords");
                    delay_ms(1000);  
                    delay_ms(1000);                    
                }
                         
			    fall_time=0;
			    OLED_Clear();//���׼����ʾ����	
			    fall_for_10s();
			    LED0=1;
			    OLED_Clear();//���Has been locked please wait 	
		    }
		}else{ delay_ms(10); mykey=0;}
        
/***************************�������**********************************/        
         adcx=Get_Adc_Average(ADC_Channel_9,10);
         if(adcx>1500)
         {
              OLED_Clear();
              delay_ms(200);	 
			  OLED_ShowString(0,0,"Warning",8);
             ret = check_status();
             if(ret == 1)
             { 
                ret = send_text_message("Harmful gas is high, please pay attention to ventilation"); 
                delay_ms(1000);
                delay_ms(1000);                              
             };
              OLED_Clear();    
         }             
      userHandle();//�û��ɼ� 
      gizwitsHandle((dataPoint_t *)&currentDataPoint);//Э�鴦��
        
		        
	} 	
}
/***************************** ���������� ***********************************************/





/******************************��ȡ��ֵ*************************************/
u16 GET_NUM(void)
{
	u8  key_num=0;
	u16 num=0;
	while(1)
	{
		key_num=keyscan();	
		if(key_num)
		{
			if(key_num==1)return 0xFFFF;//�����ء���
			if(key_num==3)return 0xFF00;//		
			if(key_num>3&&key_num<13&&num<99)//��1-9����(��������3λ��)
				num =num*10+key_num-3;		
			if(key_num==13)num =num/10;//��Del����			
			if(key_num==14&&num<99)num =num*10;//��0����
			if(key_num==15)return num;  //��Enter����
		}
	}	
}
/****************************************************************************/


/******************************����Ա**************************************/
void root_h(void)//����Ա�û�����
{

	u8 key_num;
	in_time=0;
	mykey=0;
	in_pwd1="0";
	in_pwd2="0";
	in_pwd3="0";
	in_pwd4="0";
	in_pwd5="0";
	in_pwd6="0";
	in_pwd7="0";
	in_pwd8="0";
    Moto=0;
    delay_ms(1000);
    Moto=1;
	
	

	OLED_Clear();//		�����ͨ�û�������ʾ
	OLED_ShowString(30,0,"ROOT MODE",8);	
	OLED_ShowString(0,2,"* you can change",8);
	OLED_ShowString(0,4,"* password here",8);   
	while(key_num!=82)
	{
		key_num=keyscan();      
		if(key_num)
		{		  
			switch(key_num)
			{
				case 14:str="0";break;			   
				case 4:str="1";break;	    
				case 5:str="2";break;	    
				case 6:str="3";break;		 
				case 7:str="4";break;		  
				case 8:str="5";break;	   
				case 9:str="6";break;		  
				case 10:str="7";break;		  
				case 11:str="8";break;		   
				case 12:str="9";break;
				default: break;		 
			}
			if(mykey==0)
			{
				in_time++;				
					switch(in_time)
					{
						case 1:in_pwd1=str;OLED_ShowString(0,6,str,8);delay_ms(200);OLED_ShowString(0,6,"*",8);break;
						case 2:in_pwd2=str;OLED_ShowString(11,6,str,8);delay_ms(200);OLED_ShowString(11,6,"*",8);break;
						case 3:in_pwd3=str;OLED_ShowString(23,6,str,8);delay_ms(200);OLED_ShowString(23,6,"*",8);break;
						case 4:in_pwd4=str;OLED_ShowString(35,6,str,8);delay_ms(200);OLED_ShowString(35,6,"*",8);break;
						case 5:in_pwd5=str;OLED_ShowString(47,6,str,8);delay_ms(200);OLED_ShowString(47,6,"*",8);break;
						case 6:in_pwd6=str;OLED_ShowString(59,6,str,8);delay_ms(200);OLED_ShowString(59,6,"*",8);break;
						case 7:in_pwd7=str;OLED_ShowString(71,8,str,8);delay_ms(200);OLED_ShowString(71,8,"*",8);break;
						case 8:in_pwd8=str;OLED_ShowString(83,8,str,8);delay_ms(200);OLED_ShowString(83,8,"*",8);break;
					}
				
				
				if(in_pwd1=="6"&&in_pwd2=="6"&&in_pwd3=="6"&&in_pwd4=="6"&&in_pwd5=="6"&&in_pwd6=="6"&&in_pwd7=="6"&&in_pwd8=="6")
				{
					OLED_Clear();//���������������ʾ�Լ�������
					OLED_ShowString(0,0,"Coudle not set",8);
                    OLED_ShowString(0,4,"REC:12345678",8);
					open_pwd[0]=*"1";
					open_pwd[1]=*"2";
					open_pwd[2]=*"3";
					open_pwd[3]=*"4";
					open_pwd[4]=*"5";
					open_pwd[5]=*"6";
					open_pwd[6]=*"7";
					open_pwd[7]=*"8";
                    STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)open_pwd,SIZE_PWD);
                    delay_ms(500);
					delay_ms(1000);
				}
                else
                {
                    open_pwd[0]=*in_pwd1;
                    open_pwd[1]=*in_pwd2;
                    open_pwd[2]=*in_pwd3;
                    open_pwd[3]=*in_pwd4;
                    open_pwd[4]=*in_pwd5;
                    open_pwd[5]=*in_pwd6;
					open_pwd[6]=*in_pwd7;
					open_pwd[7]=*in_pwd8;
					STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)open_pwd,SIZE_PWD);
                    delay_ms(500);
					delay_ms(1000);
                }
				
				mykey=1;//����Ϊ1��ֹ�ٴν���
			}
			
			if(in_time==8)
			{
				in_time=0;
				OLED_Clear();//���������������ʾ�Լ�������
				OLED_ShowString(10,3,"RESET COMPLETE",8);
				delay_ms(700);
                OLED_Clear();
				key_num=82;
			}
				

		}
		else 
		{
			delay_ms(10);	  
			mykey=0;//����ʱ�������Ա��´����¼���
		}
	}	 
}
/****************************************************************************/


/***************************AS608����************************************/
void AS608_Details(void)
{
    u8 ensure;
    ensure=PS_ReadSysPara(&AS608Para);  //������ 
    if(ensure==0x00)
    {
        ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	    if(ensure==0x00)
	    {		    
            OLED_Clear();//���������������ʾ�Լ�������
			OLED_ShowString(25,0,"AS608_Details",8);
            
            OLED_ShowString(0,2,"*Remainder:",8);            
            OLED_ShowString(0,4,"*Level:",8);
            OLED_ShowNum(90,2,AS608Para.PS_max-ValidN,3,12);
            OLED_ShowNum(90,4,AS608Para.PS_level,3,12);
            delay_ms(1000);
            delay_ms(1000);
            OLED_Clear();
	    }
    }
}


/****************************************************************************/


/*******************************¼ָ��*************************************/
void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	u16 ID;
    PS_ReadSysPara(&AS608Para);  //������
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
				OLED_Clear();
				OLED_ShowString(0,2,"Press the finger",8);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer1);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						OLED_Clear();
				        OLED_ShowString(0,2,"Normal",8);
						i=0;
						processnum=1;//�����ڶ���						
					}				
				}						
				break;
			
			case 1:
				i++;
				OLED_Clear();
				OLED_ShowString(0,2,"Confirm again",8);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer2);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						OLED_Clear();
				        OLED_ShowString(0,2,"Normal",8);
						i=0;
						processnum=2;//����������
					}	
				}		
				break;

			case 2:
				OLED_Clear();
				OLED_ShowString(0,2,"Compare...",8);
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					OLED_Clear();
				    OLED_ShowString(0,2,"OK",8);
					processnum=3;//�������Ĳ�
				}
				else 
				{
					OLED_Clear();
				    OLED_ShowString(0,2,"Fail",8);
				    OLED_ShowString(0,4,"try again",8);
					i=0;
					processnum=0;//���ص�һ��		
				}
				delay_ms(1200);
				break;

			case 3:
				OLED_Clear();
				OLED_ShowString(0,2,"Saving...",8);
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					OLED_Clear();
				    OLED_ShowString(0,2,"Save success",8);
					processnum=4;//�������岽
				}else {processnum=0;}
				delay_ms(1200);
				break;
				
			case 4:	
				OLED_Clear();
				OLED_ShowString(0,2,"Input ID",8);
				OLED_ShowString(0,4,"0=< ID <=299",8);
                OLED_ShowString(0,6,"Your ID:",8);
				do
                {                   
					ID=GET_NUM();
                    OLED_ShowNum(70,6,ID,3,12);
                    delay_ms(1000);
                 }
				while(!(ID<AS608Para.PS_max));//����ID����С��ָ�����������ֵ
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{			
					OLED_Clear();
				    OLED_ShowString(0,2,"Saved to flash",8);
                    delay_ms(500); 
					delay_ms(1000);
                    OLED_Clear();
					return ;
				}else {processnum=0;}					
				break;				
		}
		delay_ms(400);
		if(i==5)//����5��û�а���ָ���˳�
		{
			OLED_Clear();
			OLED_ShowString(0,2,"Out",8);
            delay_ms(300);
            OLED_Clear();
			break;	
		}				
	}
}
/****************************************************************************/


/*******************************ˢָ��*************************************/
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
    PS_ReadSysPara(&AS608Para);  //������
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		BEEP=1;//�򿪷�����	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			BEEP=0;//�رշ�����	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				OLED_Clear();
			    OLED_ShowString(30,0,"Welcome",8);	 
			    OLED_ShowString(0,2,"open the door!",8);
                OLED_ShowString(0,4,"ID:",8);
                OLED_ShowNum(20,4,seach.pageID,3,12);
                Moto=0;
                delay_ms(1000);
                Moto=1;
                delay_ms(1000);
                OLED_Clear();
                if(seach.pageID==99)
                {
                  send_text_message("Open the door under special circumstances"); 
                  delay_ms(1000);
                  delay_ms(1000);  
                 }
                                
			}
							
	  }
		
	 BEEP=0;//�رշ�����
	 delay_ms(600);
	}
		
}
/****************************************************************************/


/*******************************ɾ��ָ��*************************************/
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	OLED_Clear();
    OLED_ShowString(30,0,"Delete FR",8);
	OLED_ShowString(0,2,"Input ID",8);
	OLED_ShowString(0,4,"0=< ID <=299",8);
    OLED_ShowString(0,6,"Your ID:",8);
	delay_ms(50);
	num=GET_NUM();//��ȡ���ص���ֵ
	if(num==0xFFFF)
        goto MENU ; //������ҳ��
    else if(num==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else
    {        
        OLED_ShowNum(70,6,num,3,12);
        delay_ms(1000);
		ensure=PS_DeletChar(num,1);//ɾ������ָ��
    }        
	if(ensure==0)
	{
		OLED_Clear();
	    OLED_ShowString(0,2,"Delete success",8);		
	}			
	delay_ms(1000);
    OLED_Clear();
MENU:
	OLED_Clear();
	OLED_ShowString(0,0,"Enter 6 password",16);
    OLED_ShowString(0,2,"Fingerprint",16); 
    OLED_DrawBMP(88,3,128,8,BMP2); 
}







