#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

#define RX_DIS USART3//�ı��λ�궨�壬��ɸı伸�Ŵ������
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

u8 usart1_temp;
u8* usart1_ptemp=&usart1_temp;
extern struct HC06_DOWN_PACK Hc06_d_pack;
extern u8 CAR_TEMP;
void my_usart1_init(u32 bound)//���ڳ�ʼ������
{
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO��ʼ���ṹ��
	USART_InitTypeDef USART_InitStruct;//���ڳ�ʼ���ṹ��
	NVIC_InitTypeDef NVIC_InitStruct;//�����ж�ʹ�ܻ�����
	//��һ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//GPIOʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//����ʱ��ʹ��
	
	//�ڶ��� 
	USART_DeInit(USART1);//���ڸ�λ
	
	//������ GPIO�˿�ģʽ����
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;//GPIOPA9��ΪTX��
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);//���������ֲ���ĵ�PA9ΪTX�ڣ�Ӧ����Ϊ�����������ģʽ
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;//GPIOPA10��ΪRX��
	//GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);//���������ֲ���ĵ�PA10ΪRX�ڣ�Ӧ����Ϊ��������
	

	
	//���Ĳ�  �����жϲ���ʼNVIC(�����Ҫ�����жϲ���Ҫ������裩
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;//����USART1�ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//���������ж�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;//������ռ���ȼ�Ϊ3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;//���������ȼ�Ϊ3
	NVIC_Init(&NVIC_InitStruct);//�ж����ȼ���ʼ��
	
		//���岽 
	USART_InitStruct.USART_BaudRate=bound;//����������Ϊ4800
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//����Ϊ��Ӳ����
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//����Ϊ�շ�ģʽ
	USART_InitStruct.USART_Parity=USART_Parity_No;//��żУ��λ����Ϊ��
	USART_InitStruct.USART_StopBits=USART_StopBits_1;//ֹͣλ����Ϊ1λ
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//�����ֳ�����Ϊ8λ
	USART_Init(USART1, &USART_InitStruct);//���ڲ�����ʼ��
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//��������1�ж�
	
	//������ 
	USART_Cmd(USART1,ENABLE);//ʹ�ܴ���
	
	//���߲� ��д�жϷ�����
}


void my_usart_dispor_init(u32 bound)//����3�ܽ� USART3 TX->PB10   RX->PB11
{
	GPIO_InitTypeDef GPIO_InitStruct;//����GPIO��ʼ���ṹ��
	NVIC_InitTypeDef NVIC_InitStruct;//����һ���жϳ�ʼ���ṹ��
	USART_InitTypeDef USART_InitStruct;//����һ��USART��ʼ���ṹ��
	//��һ��  ����ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//����3ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//�ڶ��� ���ڸ�λ
	USART_DeInit(USART3);
	
	//������ GPIOģʽ����
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//�������칦��
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);//PB10-TX����Ϊ�����������ģʽ
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStruct);//PB11-RX����Ϊ��������ģʽ
	
	//���Ĳ� �����жϲ�����NVIC
	NVIC_InitStruct.NVIC_IRQChannel= USART3_IRQn;//����USART����3���ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//����
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;//������ռ���ȼ�Ϊ3��
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=4;//���������ȼ�Ϊ4��
	NVIC_Init(&NVIC_InitStruct);//�ж����ȼ���ʼ��
	
	//���岽 ���ڳ�ʼ��
	USART_InitStruct.USART_BaudRate=bound;//���ò�����
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//��Ӳ����
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//����Ϊ�շ�ģʽ
	USART_InitStruct.USART_Parity=USART_Parity_No;//����Ϊ����żλ
	USART_InitStruct.USART_StopBits=USART_StopBits_1;//����1λֹͣλ
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//��λ���ݸ�ʽ
	USART_Init(USART3,&USART_InitStruct);
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//��������2�ж�
	
	USART_Cmd(USART3,ENABLE);//ʹ�ܴ����ж�
}


char rch[5][3];     //xxx:xxx,xxx,xxx,xxx
u8 i=0;	
u8 p=0;//
u8 cmd=0;		//==1
u8 cmdok=0;	//==1 
u8 ok=0x00;



/////////////////////////////////////////////////////////////////////////////////////
//��ȡ�����ַ�����
//1.���߷�ʽ TX-->TX RX->RX 
//2.���ڳ�ʼ�� ģ���Ĭ������Ϊ 115200 8λ����λ  1��ʼλ 1ֹͣλ ����żУ��
//3.���յ����ַ����ظ���һ�㺯��
//////////////////////////////////////////////////////////////////////////////////////

int UartGet (void)  { 

    while (!(USART3->SR & USART_FLAG_RXNE));

    return ((int)(USART3->DR & 0x1FF));
} 

extern u8 rx_flag_finished;
extern u8 RX_BUF[];
extern u8 rx_count;
extern u8 ok;

/////////////////////////////////////////////////////////////////////////////////////
//��ȡ����״̬����
//1.���߷�ʽ TX-->TX RX->RX 
//2.���ڳ�ʼ�� ģ���Ĭ������Ϊ 115200 8λ����λ  1��ʼλ 1ֹͣλ ����żУ��
//3.�����жϷ�ʽ����"OK\r\n"
//4.��λokȫ�ֱ�����ֱ��ok��������Ϊ0x0f��������ȡ"OK\r\n"�����������˳������������ѭ��
//////////////////////////////////////////////////////////////////////////////////////
void CheckBusy(void)  
{
	while(1)
	{
   if(ok==0x0f)
		 break;
	}		
	
	ok=0;
}

void get_var(unsigned char *val)
{	 
	for(;;)
	{ 
		if((RX_BUF[0]=='V')&&(RX_BUF[1]=='A')&&(RX_BUF[2]=='R'))
		{
			RX_BUF[0]=0;
			RX_BUF[1]=0;
			RX_BUF[2]=0;
			val[0]=RX_BUF[3];
			val[1]=RX_BUF[4];
			val[2]=RX_BUF[5];
			val[3]=RX_BUF[6];
			rx_flag_finished=0;	
			rx_count=0;
			break;				
		}	
	}

}


		

		
/////////////////////////////////////////////////////////////////////////////////////
//����ģ�鷢�ͻ������ַ�����
//1.���߷�ʽ TX-->TX RX->RX 
//2.���ڳ�ʼ�� ģ���Ĭ������Ϊ 115200 8λ����λ  1��ʼλ 1ֹͣλ ����żУ��
//3.��λokȫ�ֱ����ͳ�ȡVAR������ASCII �ַ�
//////////////////////////////////////////////////////////////////////////////////////
		
void getch(u8 c)
{	
	 if (c=='{')
	{	i=0;
		cmd=1;
		p=0;
	}
	else if(c=='}')
	{	cmd=0;
		cmdok=1;
	}
	else if((c==':')||(c==','))
	{
			p++;
			i=0;
	}		
	else if (cmd==1)
	{	
		if (i<3) rch[p][i]=c;
		i++;
	}
	else	if(c=='O')
	{
		ok=(ok&0x00)|(0x01);
	}		
	else if(c=='K')
	{
		ok=(ok&0x0d)|(0x02);
	}	
  else if(c=='\r')
	{
		ok=(ok&0x0b)|(0x04);
	}
  else if(c=='\n')
	{
	 ok=(ok&0x07)|(0x08);
	}
	
}		

unsigned char val[4];

/////////////////////////////////////////////////////////////////////////////////////
//VAR�������ν�������
//1.���߷�ʽ TX-->TX RX->RX 
//2.���ڳ�ʼ�� ģ���Ĭ������Ϊ 115200 8λ����λ  1��ʼλ 1ֹͣλ ����żУ��
//3.VAR������ASCII �ַ�ת��Ϊvalֵ�������ظ���һ�㺯��
//////////////////////////////////////////////////////////////////////////////////////
void GetValue(void)  
{	
	unsigned char m,n;
	while(1)
	{			
		if(cmdok==1)
		{			
			if ((rch[0][0]=='V')&&(rch[0][1]=='A')&&(rch[0][2]=='R'))
			{	
				/*��һ���ֽ�*/
				if ((rch[1][2]>=0x30) && (rch[1][2]<=0x39))//100���ϵ���  
				{		
					val[0]=(rch[1][0]-0x30)*100+(rch[1][1]-0x30)*10+(rch[1][2]-0x30);
					
				}
				else if((rch[1][1]>=0x30) && (rch[1][1]<=0x39)) //10���ϵ���
				{
					val[0]=(rch[1][0]-0x30)*10+(rch[1][1]-0x30);
				}		
				else if((rch[1][0]>=0x30) && (rch[1][0]<=0x39))
				{
					val[0]=(rch[1][0]-0x30);						
				}
				/*�ڶ����ֽ�*/
					if ((rch[2][2]>=0x30) && (rch[2][2]<=0x39))//100���ϵ���
				{		
					val[1]=(rch[2][0]-0x30)*100+(rch[2][1]-0x30)*10+(rch[2][2]-0x30);				
				}
				else if((rch[2][1]>=0x30) && (rch[2][1]<=0x39)) //10���ϵ���
				{
					val[1]=(rch[2][0]-0x30)*10+(rch[2][1]-0x30);
				}		
				else if((rch[2][0]>=0x30) && (rch[2][0]<=0x39))
				{
					val[1]=(rch[2][0]-0x30);						
				}
				
			/*�������ֽ�*/
					if ((rch[3][2]>=0x30) && (rch[3][2]<=0x39))//100���ϵ���
				{		
					val[2]=(rch[3][0]-0x30)*100+(rch[3][1]-0x30)*10+(rch[3][2]-0x30);
					
				}
			
				else if((rch[3][1]>=0x30) && (rch[3][1]<=0x39)) //10���ϵ���
				{
					val[2]=(rch[3][0]-0x30)*10+(rch[3][1]-0x30);
				}		
				else if((rch[3][0]>=0x30) && (rch[3][0]<=0x39))
				{
					val[2]=(rch[3][0]-0x30);						
				}
				/*���ĸ��ֽ�*/
					if ((rch[4][2]>=0x30) && (rch[4][2]<=0x39))//100���ϵ���
				{		
					val[3]=(rch[4][0]-0x30)*100+(rch[4][1]-0x30)*10+(rch[4][2]-0x30);
					
				}
			
				else if((rch[4][1]>=0x30) && (rch[4][1]<=0x39)) //10���ϵ���
				{
					val[3]=(rch[4][0]-0x30)*10+(rch[4][1]-0x30);
				}		
				else if((rch[4][0]>=0x30) && (rch[4][0]<=0x39))
				{
					val[3]=(rch[4][0]-0x30);						
				}		
				
			for(n=0;n<5;n++)
					for(m=0;m<3;m++)
				rch[n][m]=0;
				 cmdok=0;
					break;				
			}
			//	cmdok=0;		
		}		
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//���ڷ������ݺ���
//1.���߷�ʽ TX-->TX RX->RX 
//2.���ڳ�ʼ�� ģ���Ĭ������Ϊ 115200 8λ����λ  1��ʼλ 1ֹͣλ ����żУ��
//3.���������ַ�����ֱ���ַ�������
//4. databuf ΪҪ�����ַ����ĵ�ַ
//////////////////////////////////////////////////////////////////////////////////////  

int UartSend(char* databuf) //�����ڷ�����Ϣ
{ 
	//u8 i=0;
	while (1)
	{ 
		if ((*databuf)!=0)//ֱ�����ݶ�������� 
	{ 
		USART_SendData(USART3, *databuf); //����һ���ֽ�����
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}; //
		databuf++;//i++;
	}
	else return 0;
	}
}	
char buf[128];

void LED_init(void)
{
	unsigned char i,k;	
		delay_ms(500);
		for(i=0;i<7;i++)
		{
			sprintf(buf,"DIR(1);FSIMG(%2d,0,0,320,240,0);\r\n",2097152+320*480*2*i);//�ڣ�0,0��λ��������ȡ8������ͼƬ����һ�Ŵӵ�ַ2097152��ʼ��ͼƬ��С480x320��ģʽ0��ʾ������ʾ
			UartSend(buf);//�����ַ�ָ�
			CheckBusy();	//�ȴ�ģ��ִ���꣬Ҳ���Ǽ��ģ�鷢�ͻ�����"OK\r\n"
			delay_ms(50);
		}
	 
		UartSend("DIR(1);CLR(0);DC48(80,10,'�������ӽ�Ƽ�',4);\r\n");// ������ʾ��������ɫΪ��ɫ���û�ɫ��(80,10)Ϊ���,����Ļ����ʾ͸����'�������ӽ�Ƽ�'���ĸ���,�����СΪ48,4Ϊ��ɫ��ɫ����
		CheckBusy();

		UartSend("PL(0,60,480,60,15);\r\n");//��ֱ�������Ļ�ϵ�(0,60)λ�õĵ�ͣ�480,60��λ�õĵ��ð�ɫ(���15)��ֱ��������
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(0,60,480,60,15);\r\n");//��ֱ������ͬ��
		CheckBusy();
		delay_ms(800);
	
		UartSend("PL(0,140,480,140,15);\r\n");//��ֱ������ͬ��
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(0,230,480,230,15);\r\n");//��ֱ������ͬ��
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(0,319,480,320,15);\r\n");//��ֱ������ͬ��
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(0,60,0,319,15);\r\n");//��ֱ������ͬ��
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(160,60,160,319,15);\r\n");//��ֱ������ͬ��
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(340,60,340,319,15);\r\n");//��ֱ������ͬ��
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(479,60,479,319,15);\r\n");//��ֱ������ͬ��
		CheckBusy();
		delay_ms(800);
		
		UartSend("CIR(70,100,30,1);\r\n"); //(70,100)ΪԲ�� 30Ϊ�뾶�ú�ɫ��һ����Բ ,1Ϊ��ɫ��ɫ����
		CheckBusy();
		UartSend("CIRF(80,100,30,2);\r\n"); //(80,100)ΪԲ�� 30Ϊ�뾶����ɫ��һ�����Բ, ,2Ϊ��ɫ��ɫ����
		CheckBusy();
		delay_ms(800);
		
		UartSend("BOXF(190,75,310,130,3);\r\n");//�Խ���(190,75)��(310,130)������ ,3Ϊ��ɫ��ɫ����
		CheckBusy();
		
		UartSend("BOX(200,75,320,130,4);\r\n");//�Խ���(200,75)��(320,130)���߿� ,4Ϊ��ɫ��ɫ����
		CheckBusy();
		
		UartSend("PS(410,85,2);\r\n");//����Ļ��(410,85) λ������ɫ���һ����,2Ϊ��ɫ��ɫ����
		CheckBusy();	
		
		UartSend("PL(370,105,460,105,1);\r\n");//��ֱ������ͬ��
		CheckBusy();
		
		
		UartSend("DC16(50,180,'ABC',3);\r\n");//ˢ��������ͬ��
		CheckBusy();
		
		UartSend("SBC(15);DCV16(80,180,'ABC',0);\r\n"); //�ú�ɫ��(80,180)Ϊ���,����Ļ��ʾ����ɫ����ɫ��'ABC'���ĸ���ĸ,�����СΪ16,0Ϊ��ɫ��ɫ����	
		CheckBusy();
		
		UartSend("DC24(200,170,'123',2);\r\n");//ˢ��������ͬ��
		CheckBusy();
		UartSend("SBC(4);DCV24(240,170,'123',0);\r\n");//ˢ��������ͬ��
		CheckBusy();
		
		UartSend("DC32(380,170,'����',2);\r\n");//ˢ��������ͬ��
		CheckBusy();
		
		UartSend("SBC(1);DCV32(40,260,'����',15);\r\n"); //ˢ��������ͬ��	
		CheckBusy();
		
		UartSend("DC48(200,260,'�й�',6);\r\n");//ˢ��������ͬ��
		CheckBusy();
		
		UartSend("DC72(370,240,'72',1);\r\n"); 	//ˢ��������ͬ��
		CheckBusy();
		delay_ms(800);
		
		UartSend("CLR(0);DIR(1);FSIMG(4247552,0,0,480,320,0);\r\n");//ˢͼƬָ��ͬ��
		CheckBusy();	
		UartSend("DC48(110,10,'2017-06-28 ',15);\r\n");//ˢ��������ͬ��
		CheckBusy();
		UartSend("DC32(90,85,' ���ڶ�',15);\r\n");//ˢ��������ͬ��
		CheckBusy();
		
		UartSend("DC32(220,85,' 20 ��',15);\r\n");//ˢ��������ͬ��
		CheckBusy();
		
		UartSend("PL(0,60,480,60,15);\r\n");//��ֱ������ͬ��
		CheckBusy();
			 
	{
		 
		 k=0;
		 for(k=0;k<=60;k++)
	 {
			sprintf(buf,"SBC(15);DCV32(320,85,'%d',1);\r\n",k);//ˢ32������仯����0-60	 
			k=k+1;
			delay_ms(100);
			UartSend(buf);
			CheckBusy();	
						 
	 }
		 k=0;
		 for(k=0;k<=50;k++)
		 {
			sprintf(buf,"SBC(15);DCV32(70,180,'%d',1);\r\n",k);//ˢ32������仯����0-50	
			k=k+1;
			delay_ms(100);
			UartSend(buf);
			CheckBusy();	
			 
			 sprintf(buf,"SBC(15);DCV32(220,180,'%d',1);\r\n",k);//ˢ32������仯����0-50	
			 k=k+1;
			 delay_ms(100);
			 UartSend(buf);
			 CheckBusy();
			 			 
			 sprintf(buf,"SBC(15);DCV32(360,180,'%d',1);\r\n",k);//ˢ32������仯����0-50	
			 k=k+1;
			 delay_ms(100);
			 UartSend(buf);
			 CheckBusy();						 
			}
		 
		}
	 		delay_ms(3000);
		
		UartSend("CLR(0);DIR(1);FSIMG(4554752,0,0,480,320,0);\r\n");//ˢͼƬָ��ͬ��
		CheckBusy();	
		
	{
			k=40;

		{
			sprintf(buf,"DCV32(180,150,'���ȣ�80 ',1);BL(%2d);\r\n",k);//ˢ32�����壬�����ȱ���kת����"BL(k);\r\n"���ַ�ָ���Χ��0Ϊ���� 255Ϊ�
			UartSend(buf);
			CheckBusy();	
			delay_ms(1000);
		}	
	}	
	
	{
			k=127;
		{
			sprintf(buf,"DCV32(180,150,'���ȣ�50 ',1);BL(%2d);\r\n",k);//ˢ32�����壬�����ȱ���kת����"BL(k);\r\n"���ַ�ָ���Χ��0Ϊ���� 255Ϊ�
			UartSend(buf);
			CheckBusy();	
			delay_ms(1000);
		}	
	}	
		
		{
			k=200;

		 {
			sprintf(buf,"DCV32(180,150,'���ȣ�20 ',1);BL(%2d);\r\n",k);//ˢ32�����壬�����ȱ���kת����"BL(k);\r\n"���ַ�ָ���Χ��0Ϊ���� 255Ϊ�
			UartSend(buf);
			CheckBusy();	
			delay_ms(1000);
		}	
	 }	
			
	{
			k=0;
		{
			sprintf(buf,"DCV32(170,150,'���ȣ�100',1);BL(%2d);\r\n",k);//ˢ32�����壬�����ȱ���kת����"BL(k);\r\n"���ַ�ָ���Χ��0Ϊ���� 255Ϊ�
			UartSend(buf);
			CheckBusy();	
			delay_ms(1000);
		}	
	}	
	  	delay_ms(1600);
		UartSend("DIR(1);CLR(61);DC48(130,140,'��ʾ������',15);\r\n");//ˢ��������ͬ�� 	
		CheckBusy();
		delay_ms(800);


}


u8 RX_BUF[4];
u8 rx_flag_finished=0;
u8 rx_count=0;
void USART1_IRQHandler(void)//����1�жϷ�����
{
	u8 temp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		temp=USART_ReceiveData(USART1);//���մ���1��ȡ����Ϣ

		USART_SendData(USART3,temp);//������1��ȡ����Ϣ���͸�����2	
			
	}
}


void USART3_IRQHandler(void)//����2�жϷ�����
{
	//	u8 temp;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		usart1_temp=USART_ReceiveData(USART3);//���մ���1��ȡ����Ϣ
		USART_SendData(USART1,usart1_temp);//������1��ȡ����Ϣ���͸�����2	
		
		
	}
}

#endif

