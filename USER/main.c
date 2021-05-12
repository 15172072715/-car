#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "usart.h"	
#include "pstwo.h"
#include "math.h"
#include "while.h"

int main(void)
 {	
	 u8 key;
	 int angle;
	 u8 action;//执行动作变量
	 
	 led_Init();
	 delay_init();
		my_usart1_init(115200);
	 PS2_Init();//PS2初始化
	 PS2_ClearData();
	 car_while_init();
	 car_action(STOP);
	 while(PS2_RedLight())
	 {
			GPIO_SetBits(GPIOA, GPIO_Pin_8);
			delay_ms(500);
			GPIO_ResetBits(GPIOA, GPIO_Pin_8);
			delay_ms(500); 
	 }


	 while(1)
	 {
	  key=PS2_DataKey();//读值		 
		angle=atan2(((PS2_AnologData(PSS_LY)-127)*1.0) ,((PS2_AnologData(PSS_LX)-128)*1.0))*180/3.1415926;//求角度
		
		 
			 if(key!=0)															{if(key==10)car_action(FORCED_STOP);}				
																																													//有按键按下，执行强制制动
			 else if(angle<-25&&angle>-70)					{action=GO_RIGHT;			}	//右前行
			 else if(angle<-70&&angle>-120)					{action=GO;						}	//前进
			 else if(angle<-120&&angle>-170)				{action=GO_LEFT;			}	//左前行			 
			 else if(angle>170)											{action=SHIFT_LEFT;		}	//左移			
			 else if(angle<170&&angle>120)					{action=BACK_LEFT;		}	//左后			 
			 else if(angle<120&&angle>70)						{action=BACK;					}	//后退		 
			 else if(angle<70&&angle>25)						{action=BACK_RIGHT;		}	//右后
			 else if(PS2_AnologData(PSS_RX)>130)		{action=RIGHT_WHEEL;	}	//右转
			 else if(PS2_AnologData(PSS_RX)<120)		{action=LEFT_WHEEL;		}	//左转
			 else if((PS2_AnologData(PSS_LX)-128!=0||PS2_AnologData(PSS_LY)-127!=0)&&(angle<25&&angle>-25))		
																							{action=SHIFT_RIGHR;	}	//右移
			 else 																	{action=STOP;					}	//停止
				car_action(action);
				delay_ms(10);
				
		} 
 }

 
// printf("%5d %5d %5d %5d %5d %5d   ",key,PS2_AnologData(PSS_LX),PS2_AnologData(PSS_LY),
//		                              PS2_AnologData(PSS_RX),PS2_AnologData(PSS_RY),angle );	//打印数值
//		 
//			 if(key!=0)															{if(key==10)car_action(FORCED_STOP);printf("强制制动\r\n");}				
//																																													//有按键按下，执行强制制动
//			 else if(angle<-25&&angle>-70)					{action=GO_RIGHT;			printf("右前\r\n");}	//右前行
//			 else if(angle<-70&&angle>-120)					{action=GO;						printf("前进\r\n");}	//前进
//			 else if(angle<-120&&angle>-170)				{action=GO_LEFT;			printf("左前\r\n");}	//左前行			 
//			 else if(angle>170)											{action=SHIFT_LEFT;		printf("左移\r\n");}	//左移			
//			 else if(angle<170&&angle>120)					{action=BACK_LEFT;		printf("左后\r\n");}	//左后			 
//			 else if(angle<120&&angle>70)						{action=BACK;					printf("后退\r\n");}	//后退		 
//			 else if(angle<70&&angle>25)						{action=BACK_RIGHT;		printf("右后\r\n");}	//右后
//			 else if(PS2_AnologData(PSS_RX)>130)		{action=RIGHT_WHEEL;	printf("右转\r\n");}	//右转
//			 else if(PS2_AnologData(PSS_RX)<120)		{action=LEFT_WHEEL;		printf("左转\r\n");}	//左转
//			 else if((PS2_AnologData(PSS_LX)-128!=0||PS2_AnologData(PSS_LY)-127!=0)&&(angle<25&&angle>-25))		
//																							{action=SHIFT_RIGHR;	printf("右移\r\n");}	//右移
//			 else 																	{action=STOP;					printf("停止\r\n");}	//停止
//				car_action(action);
//				delay_ms(200);
