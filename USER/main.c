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
	 u8 action;//ִ�ж�������
	 
	 led_Init();
	 delay_init();
		my_usart1_init(115200);
	 PS2_Init();//PS2��ʼ��
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
	  key=PS2_DataKey();//��ֵ		 
		angle=atan2(((PS2_AnologData(PSS_LY)-127)*1.0) ,((PS2_AnologData(PSS_LX)-128)*1.0))*180/3.1415926;//��Ƕ�
		
		 
			 if(key!=0)															{if(key==10)car_action(FORCED_STOP);}				
																																													//�а������£�ִ��ǿ���ƶ�
			 else if(angle<-25&&angle>-70)					{action=GO_RIGHT;			}	//��ǰ��
			 else if(angle<-70&&angle>-120)					{action=GO;						}	//ǰ��
			 else if(angle<-120&&angle>-170)				{action=GO_LEFT;			}	//��ǰ��			 
			 else if(angle>170)											{action=SHIFT_LEFT;		}	//����			
			 else if(angle<170&&angle>120)					{action=BACK_LEFT;		}	//���			 
			 else if(angle<120&&angle>70)						{action=BACK;					}	//����		 
			 else if(angle<70&&angle>25)						{action=BACK_RIGHT;		}	//�Һ�
			 else if(PS2_AnologData(PSS_RX)>130)		{action=RIGHT_WHEEL;	}	//��ת
			 else if(PS2_AnologData(PSS_RX)<120)		{action=LEFT_WHEEL;		}	//��ת
			 else if((PS2_AnologData(PSS_LX)-128!=0||PS2_AnologData(PSS_LY)-127!=0)&&(angle<25&&angle>-25))		
																							{action=SHIFT_RIGHR;	}	//����
			 else 																	{action=STOP;					}	//ֹͣ
				car_action(action);
				delay_ms(10);
				
		} 
 }

 
// printf("%5d %5d %5d %5d %5d %5d   ",key,PS2_AnologData(PSS_LX),PS2_AnologData(PSS_LY),
//		                              PS2_AnologData(PSS_RX),PS2_AnologData(PSS_RY),angle );	//��ӡ��ֵ
//		 
//			 if(key!=0)															{if(key==10)car_action(FORCED_STOP);printf("ǿ���ƶ�\r\n");}				
//																																													//�а������£�ִ��ǿ���ƶ�
//			 else if(angle<-25&&angle>-70)					{action=GO_RIGHT;			printf("��ǰ\r\n");}	//��ǰ��
//			 else if(angle<-70&&angle>-120)					{action=GO;						printf("ǰ��\r\n");}	//ǰ��
//			 else if(angle<-120&&angle>-170)				{action=GO_LEFT;			printf("��ǰ\r\n");}	//��ǰ��			 
//			 else if(angle>170)											{action=SHIFT_LEFT;		printf("����\r\n");}	//����			
//			 else if(angle<170&&angle>120)					{action=BACK_LEFT;		printf("���\r\n");}	//���			 
//			 else if(angle<120&&angle>70)						{action=BACK;					printf("����\r\n");}	//����		 
//			 else if(angle<70&&angle>25)						{action=BACK_RIGHT;		printf("�Һ�\r\n");}	//�Һ�
//			 else if(PS2_AnologData(PSS_RX)>130)		{action=RIGHT_WHEEL;	printf("��ת\r\n");}	//��ת
//			 else if(PS2_AnologData(PSS_RX)<120)		{action=LEFT_WHEEL;		printf("��ת\r\n");}	//��ת
//			 else if((PS2_AnologData(PSS_LX)-128!=0||PS2_AnologData(PSS_LY)-127!=0)&&(angle<25&&angle>-25))		
//																							{action=SHIFT_RIGHR;	printf("����\r\n");}	//����
//			 else 																	{action=STOP;					printf("ֹͣ\r\n");}	//ֹͣ
//				car_action(action);
//				delay_ms(200);
