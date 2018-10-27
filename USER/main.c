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

//ALIENTEK 战舰V3&精英 STM32开发板 扩展实验14
//ATK-AS608指纹识别模块实验-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 
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
int mykey=0;//控制in_time每按一个按键只加一次的坎儿
u8 root_pwd[8]="66666666";//管理员密码
u8 open_pwd[8]="";//开锁密码
#define SIZE_PWD sizeof(open_pwd)
#define FLASH_SAVE_ADDR  0X08070000	

#define usart2_baund  57600//串口2波特率，根据指纹模块波特率更改

SysPara AS608Para;//指纹模块AS608参数
u16 ValidN;//模块内有效指纹个数

void root_h(void);//管理员函数

void Add_FR(void);	//录指纹
void Del_FR(void);	//删除指纹
void press_FR(void);//刷指纹
u16 GET_NUM(void);//获取数值
void AS608_Details(void);//获取模块详情
 
extern u8  find_string_flag;
extern u8  receive_mode;
//协议初始化
void Gizwits_Init(void)
{
	
	TIM3_Int_Init(9,7199);//1MS系统定时
    usart3_init(9600);//WIFI初始化
	userInit();//设备状态结构体初始化
	gizwitsInit();//缓冲区初始化
}

int main(void)
{    
	u8 key_num=0;	
    u8 *str=0;
    int  ret;
    u16 adcx;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();  	//初始化延时函数
	usart2_init(usart2_baund);//初始化串口2,用于与指纹模块通讯
    USART1_Init_Config(115200);  //串口1，用于与A6模块通信
	Timer2_Init_Config();        //定时器2初始化，用于A6模块
	PS_StaGPIO_Init();	//初始化FR读状态引脚
	BEEP_Init();  			//初始化蜂鸣器    			     
	KEY_Init();
	OLED_Init();    
    LED_Init();
    Adc_Init();
    Gizwits_Init();         //协议初始化
        
    receive_mode = 1;
    
	OLED_Clear();
	//与AS608模块握手
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
        OLED_ShowNum(0,5,adcx,4,12);//显示ADC的值
        OLED_DrawBMP(88,3,128,8,BMP2); 
        
        
/***************************指纹解锁*************************************/ 
        
        if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
		{
			press_FR();//刷指纹			
		}
        
/***************************短信解锁*************************************/
      /*  set_text_mode();//设置短信
        CLR_Buf();        
        if(find_string_flag == 1)
		{
            
			delay_ms(200);//延时一点，让串口把数据接收完成
			if(Find("+CMTI"))//说明接收到了短信
			{
				ret = read_message();
				if(ret == 1)
				{
					if(Find("ledon"))
					{
						CLR_Buf();
						find_string_flag = 0;
                        delay_ms(200);
                        
                        OLED_Clear();//清除之前输入的密码
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
                        
                        OLED_Clear();//清除之前输入的密码	 
					    OLED_ShowString(0,2,"close the door!",8);
                        delay_ms(1000);
                        OLED_Clear();
					}
				}
			}
        } */
 /***************************密码解锁**********************************/       
		key_num=keyscan();	
        if(key_num)
        {
           switch(key_num)
           {
                case 1:Del_FR();        key_num=0;break; //删指纹
                case 2:AS608_Details(); key_num=0;break; //详情
                case 3:Add_FR();        key_num=0;break; //录指纹
               
                case 13:gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-link模式接入
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
				mykey=1;//设置为1防止再次进入
			}
			
			if(in_time==8)
			{
				if( //普通用户配对成功
					*in_pwd3==open_pwd[2]&&
				    *in_pwd4==open_pwd[3]&&
				    *in_pwd5==open_pwd[4]&&
			        *in_pwd6==open_pwd[5])
				{
					fall_time=0;
					OLED_Clear();//清除之前输入的密码
                    delay_ms(200);
					OLED_ShowString(30,0,"Welcome",8);	 
					OLED_ShowString(0,2,"open the door!",8);
                    Moto=0;
                    delay_ms(1000);
                    Moto=1;
                    delay_ms(1000);
                    OLED_Clear();
				}
				else if( *in_pwd1==root_pwd[0]&&//管理员用户配对成功
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
				else//配对失败
				{
					OLED_Clear();//清除之前输入的密码
                    delay_ms(200);
					OLED_ShowString(0,0,"*Wrong Password*",8);
                    OLED_ShowString(0,3,"please try again",8);
					fall_time++;
					LED0=!LED0;//红灯闪烁提示
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
			    OLED_Clear();//清除准备显示锁定	
			    fall_for_10s();
			    LED0=1;
			    OLED_Clear();//清除Has been locked please wait 	
		    }
		}else{ delay_ms(10); mykey=0;}
        
/***************************紧急情况**********************************/        
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
      userHandle();//用户采集 
      gizwitsHandle((dataPoint_t *)&currentDataPoint);//协议处理
        
		        
	} 	
}
/***************************** 主函数结束 ***********************************************/





/******************************获取数值*************************************/
u16 GET_NUM(void)
{
	u8  key_num=0;
	u16 num=0;
	while(1)
	{
		key_num=keyscan();	
		if(key_num)
		{
			if(key_num==1)return 0xFFFF;//‘返回’键
			if(key_num==3)return 0xFF00;//		
			if(key_num>3&&key_num<13&&num<99)//‘1-9’键(限制输入3位数)
				num =num*10+key_num-3;		
			if(key_num==13)num =num/10;//‘Del’键			
			if(key_num==14&&num<99)num =num*10;//‘0’键
			if(key_num==15)return num;  //‘Enter’键
		}
	}	
}
/****************************************************************************/


/******************************管理员**************************************/
void root_h(void)//管理员用户函数
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
	
	

	OLED_Clear();//		清除普通用户界面提示
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
					OLED_Clear();//清除输入新密码提示以及新密码
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
				
				mykey=1;//设置为1防止再次进入
			}
			
			if(in_time==8)
			{
				in_time=0;
				OLED_Clear();//清除输入新密码提示以及新密码
				OLED_ShowString(10,3,"RESET COMPLETE",8);
				delay_ms(700);
                OLED_Clear();
				key_num=82;
			}
				

		}
		else 
		{
			delay_ms(10);	  
			mykey=0;//放手时，归零以便下次重新计数
		}
	}	 
}
/****************************************************************************/


/***************************AS608详情************************************/
void AS608_Details(void)
{
    u8 ensure;
    ensure=PS_ReadSysPara(&AS608Para);  //读参数 
    if(ensure==0x00)
    {
        ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	    if(ensure==0x00)
	    {		    
            OLED_Clear();//清除输入新密码提示以及新密码
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


/*******************************录指纹*************************************/
void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	u16 ID;
    PS_ReadSysPara(&AS608Para);  //读参数
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
					ensure=PS_GenChar(CharBuffer1);//生成特征
					BEEP=0;
					if(ensure==0x00)
					{
						OLED_Clear();
				        OLED_ShowString(0,2,"Normal",8);
						i=0;
						processnum=1;//跳到第二步						
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
					ensure=PS_GenChar(CharBuffer2);//生成特征
					BEEP=0;
					if(ensure==0x00)
					{
						OLED_Clear();
				        OLED_ShowString(0,2,"Normal",8);
						i=0;
						processnum=2;//跳到第三步
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
					processnum=3;//跳到第四步
				}
				else 
				{
					OLED_Clear();
				    OLED_ShowString(0,2,"Fail",8);
				    OLED_ShowString(0,4,"try again",8);
					i=0;
					processnum=0;//跳回第一步		
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
					processnum=4;//跳到第五步
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
				while(!(ID<AS608Para.PS_max));//输入ID必须小于指纹容量的最大值
				ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
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
		if(i==5)//超过5次没有按手指则退出
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


/*******************************刷指纹*************************************/
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
    PS_ReadSysPara(&AS608Para);  //读参数
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		BEEP=1;//打开蜂鸣器	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			BEEP=0;//关闭蜂鸣器	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//搜索成功
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
		
	 BEEP=0;//关闭蜂鸣器
	 delay_ms(600);
	}
		
}
/****************************************************************************/


/*******************************删除指纹*************************************/
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
	num=GET_NUM();//获取返回的数值
	if(num==0xFFFF)
        goto MENU ; //返回主页面
    else if(num==0xFF00)
		ensure=PS_Empty();//清空指纹库
	else
    {        
        OLED_ShowNum(70,6,num,3,12);
        delay_ms(1000);
		ensure=PS_DeletChar(num,1);//删除单个指纹
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







