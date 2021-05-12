#ifndef __WHILE_H
#define __WHILE_H
#include "sys.h"
#include "delay.h"
#include "usart.h"

/******************************************************************************
 �����������ߣ�
 
												����																����
			ǰ�֣�M3_C_L-->PC6				M3_C_R-->PC7		 M4_C_L-->PC8    		M4_C_R-->PC9
			���֣�M1_C_L-->PC5				M1_C_R-->PC4     M2_C_L-->PC1    		M2_C_L-->PD2

*******************************************************************************/

#define up_left_a      PCout(6)	  //������ǰ��A���ƶ�
#define up_left_b      PCout(7) 	//������ǰ��B���ƶ�

#define up_right_a		 PCout(8)		//������ǰ��A���ƶ�
#define up_right_b		 PCout(9)		//������ǰ��B���ƶ�

#define back_left_a		 PCout(5)		//���������A���ƶ�
#define back_left_b		 PCout(4)		//���������B���ƶ�

#define back_right_a	 PCout(1)		//�����Һ���A���ƶ�
#define back_right_b	 PDout(2)		//�����Һ���B���ƶ�



void car_stop(void);//����ֹͣ������
void car_Forced_stop(void);//ǿ���ƶ�
void car_go(u16 speed);//ǰ������   speed���ٶ�
void car_back(u16 speed);//���˺���  speed���ٶ�	
void car_shift_left(u16 speed);//���ƺ���  speed:�ٶ�
void car_shift_right(u16 speed);//���ƺ���  speed:�ٶ�
void car_go_left(u16 speed);//��ǰ�ƶ�  speed���ٶ�
void car_go_right(u16 speed);//��ǰ�ƶ�  speed���ٶ�
void car_back_left(u16 speed);//����ƶ�  speed���ٶ�
void car_back_right(u16 speed);//�Һ��ƶ�  speed���ٶ�
void car_left_wheel(u16 speed);//����ת   speed���ٶ�
void car_right_wheel(u16 speed);//����ת   speed���ٶ�

#define STOP 						'0'//ֹͣ����
#define GO 							'1'//ǰ��
#define BACK 						'2'//����
#define SHIFT_LEFT 			'3'//����
#define SHIFT_RIGHR		  '4'//����
#define GO_LEFT					'5'//��ǰ�ƶ�
#define GO_RIGHT 				'6'//��ǰ�ƶ�
#define BACK_LEFT 			'7'//�����ƶ�
#define BACK_RIGHT		  '8'//�����ƶ�
#define LEFT_WHEEL 			'A'//����ת
#define RIGHT_WHEEL 		'B'//����ת
#define FORCED_STOP 		'C'//ǿ���ƶ�

void car_while_init(void);	//С������������ƶ˳�ʼ��
void car_action(u8 action);//����ִ���жϺ���
#endif
