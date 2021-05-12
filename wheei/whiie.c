#include "while.h"

void car_while_init(void)//小车轮子外设控制端初始化
{
	GPIO_InitTypeDef GPIO_Inittest;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE );	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE );
	
  GPIO_Inittest.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Inittest.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_1;
	GPIO_Inittest.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_Inittest);
	
	GPIO_Inittest.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_Inittest);
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_1);
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
}

void car_action(u8 action)//动作执行判断函数
{
	switch(action)
	{
		case STOP:					car_stop();						break;//停止不动
		case GO:						car_go(0);						break;//前进
		case BACK: 					car_back(0);					break;//后退
		case SHIFT_LEFT:		car_shift_left(0);		break;//左行
		case SHIFT_RIGHR:		car_shift_right(0);		break;//右行
		case GO_LEFT:				car_go_left(0);				break;//左前移动
		case GO_RIGHT:			car_go_right(0);			break;//右前移动
		case BACK_LEFT:			car_back_left(0);			break;//左退移动
		case BACK_RIGHT:		car_back_right(0);		break;//右退移动
		case LEFT_WHEEL:		car_left_wheel(0);		break;//左旋转
		case RIGHT_WHEEL:		car_right_wheel(0);		break;//右旋转
		case FORCED_STOP:		car_Forced_stop();		break;//强力制动
		case'D':break;
		case'E':break;
		case'F':break;
		
	}
}


void car_stop(void)//车辆停止不运行
{
	up_left_a=0;
	up_left_b=0;
	
	up_right_a=0;
	up_right_b=0;
	
	back_left_a=0;
	back_left_b=0;
	
	back_right_a=0;
	back_right_b=0;
}

void car_Forced_stop(void)//强力制动
{
	up_left_a=1;
	up_left_b=1;
	
	up_right_a=1;
	up_right_b=1;
	
	back_left_a=1;
	back_left_b=1;
	
	back_right_a=1;
	back_right_b=1;
}

void car_go(u16 speed)//前进函数   speed：速度
{
	up_left_a=0;
	up_left_b=1;
	
	up_right_a=0;
	up_right_b=1;
	
	back_left_a=0;
	back_left_b=1;
	
	back_right_a=0;
	back_right_b=1;
}



void car_back(u16 speed)//后退函数  speed：速度
{
	up_left_a=1;
	up_left_b=0;
	
	up_right_a=1;
	up_right_b=0;
	
	back_left_a=1;
	back_left_b=0;
	
	back_right_a=1;
	back_right_b=0;
}


void car_shift_left(u16 speed)//左移函数  speed:速度
{
		up_left_a=1;
	up_left_b=0;
	
	up_right_a=1;
	up_right_b=0;
	
	back_left_a=0;
	back_left_b=1;
	
	back_right_a=0;
	back_right_b=1;
}	


void car_shift_right(u16 speed)//右移函数  speed:速度
{
	up_left_a=0;
	up_left_b=1;
	
	up_right_a=0;
	up_right_b=1;
	
	back_left_a=1;
	back_left_b=0;
	
	back_right_a=1;
	back_right_b=0;
}	


void car_go_left(u16 speed)//左前移动  speed：速度
{
	up_left_a=0;
	up_left_b=1;
	
	up_right_a=0;
	up_right_b=0;
	
	back_left_a=0;
	back_left_b=0;
	
	back_right_a=0;
	back_right_b=1;
}	


void car_go_right(u16 speed)//右前移动  speed：速度
{
	up_left_a=1;
	up_left_b=0;
	
	up_right_a=0;
	up_right_b=0;
	
	back_left_a=0;
	back_left_b=0;
	
	back_right_a=1;
	back_right_b=0;
}	


void car_back_left(u16 speed)//左后移动  speed：速度
{
	up_left_a=0;
	up_left_b=0;
	
	up_right_a=0;
	up_right_b=1;
	
	back_left_a=0;
	back_left_b=1;
	
	back_right_a=0;
	back_right_b=0;
}


void car_back_right(u16 speed)//右后移动  speed：速度
{
	up_left_a=0;
	up_left_b=0;
	
	up_right_a=1;
	up_right_b=0;
	
	back_left_a=1;
	back_left_b=0;
	
	back_right_a=0;
	back_right_b=0;
}

void car_left_wheel(u16 speed)//左旋转   speed：速度
{
		up_left_a=1;
	up_left_b=0;
	
	up_right_a=0;
	up_right_b=1;
	
	back_left_a=1;
	back_left_b=0;
	
	back_right_a=0;
	back_right_b=1;
}

void car_right_wheel(u16 speed)//右旋转   speed：速度
{
	up_left_a=0;
	up_left_b=1;
	
	up_right_a=1;
	up_right_b=0;
	
	back_left_a=0;
	back_left_b=1;
	
	back_right_a=1;
	back_right_b=0;
}

