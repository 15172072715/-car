#ifndef __WHILE_H
#define __WHILE_H
#include "sys.h"
#include "delay.h"
#include "usart.h"

/******************************************************************************
 电机驱动板接线：
 
												左轮																右轮
			前轮：M3_C_L-->PC6				M3_C_R-->PC7		 M4_C_L-->PC8    		M4_C_R-->PC9
			后轮：M1_C_L-->PC5				M1_C_R-->PC4     M2_C_L-->PC1    		M2_C_L-->PD2

*******************************************************************************/

#define up_left_a      PCout(6)	  //定义左前轮A控制端
#define up_left_b      PCout(7) 	//定义左前轮B控制端

#define up_right_a		 PCout(8)		//定义右前轮A控制端
#define up_right_b		 PCout(9)		//定义右前轮B控制端

#define back_left_a		 PCout(5)		//定义左后轮A控制端
#define back_left_b		 PCout(4)		//定义左后轮B控制端

#define back_right_a	 PCout(1)		//定义右后轮A控制端
#define back_right_b	 PDout(2)		//定义右后轮B控制端



void car_stop(void);//车辆停止不运行
void car_Forced_stop(void);//强力制动
void car_go(u16 speed);//前进函数   speed：速度
void car_back(u16 speed);//后退函数  speed：速度	
void car_shift_left(u16 speed);//左移函数  speed:速度
void car_shift_right(u16 speed);//右移函数  speed:速度
void car_go_left(u16 speed);//左前移动  speed：速度
void car_go_right(u16 speed);//右前移动  speed：速度
void car_back_left(u16 speed);//左后移动  speed：速度
void car_back_right(u16 speed);//右后移动  speed：速度
void car_left_wheel(u16 speed);//左旋转   speed：速度
void car_right_wheel(u16 speed);//右旋转   speed：速度

#define STOP 						'0'//停止不动
#define GO 							'1'//前进
#define BACK 						'2'//后退
#define SHIFT_LEFT 			'3'//左行
#define SHIFT_RIGHR		  '4'//右行
#define GO_LEFT					'5'//左前移动
#define GO_RIGHT 				'6'//右前移动
#define BACK_LEFT 			'7'//左退移动
#define BACK_RIGHT		  '8'//右退移动
#define LEFT_WHEEL 			'A'//左旋转
#define RIGHT_WHEEL 		'B'//右旋转
#define FORCED_STOP 		'C'//强力制动

void car_while_init(void);	//小车轮子外设控制端初始化
void car_action(u8 action);//动作执行判断函数
#endif
