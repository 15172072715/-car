#include "stm32f10x_rcc.h"
#include "sys.h"
#include "led.h"


void led_Init(void)
{
	GPIO_InitTypeDef GPIO_Inittest;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD,ENABLE );
	
  GPIO_Inittest.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Inittest.GPIO_Pin = GPIO_Pin_8;
	GPIO_Inittest.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Inittest);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	
//	GPIO_Inittest.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Inittest.GPIO_Pin = GPIO_Pin_2;
//	GPIO_Inittest.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOD,&GPIO_Inittest);
//	
//  GPIO_SetBits(GPIOD, GPIO_Pin_2);

}

