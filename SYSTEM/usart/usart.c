#include "sys.h"
#include "usart.h"
#include "stdio.h"
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

#define RX_DIS USART3//改变此位宏定义，便可改变几号串口输出
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

u8 usart1_temp;
u8* usart1_ptemp=&usart1_temp;
extern struct HC06_DOWN_PACK Hc06_d_pack;
extern u8 CAR_TEMP;
void my_usart1_init(u32 bound)//串口初始化函数
{
	GPIO_InitTypeDef GPIO_InitStruct;//GPIO初始化结构体
	USART_InitTypeDef USART_InitStruct;//串口初始化结构体
	NVIC_InitTypeDef NVIC_InitStruct;//串口中断使能机构体
	//第一步
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//GPIO时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//串口时钟使能
	
	//第二步 
	USART_DeInit(USART1);//串口复位
	
	//第三步 GPIO端口模式设置
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;//GPIOPA9口为TX口
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);//根据数据手册查阅到PA9为TX口，应设置为复用推挽输出模式
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;//GPIOPA10口为RX口
	//GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);//根据数据手册查阅到PA10为RX口，应设置为浮空输入
	

	
	//第四步  开启中断并初始NVIC(如果需要开启中断才需要这个步骤）
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;//开启USART1中断
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//开启串口中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;//设置抢占优先级为3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;//设置子优先级为3
	NVIC_Init(&NVIC_InitStruct);//中断优先级初始化
	
		//第五步 
	USART_InitStruct.USART_BaudRate=bound;//波特率设置为4800
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//设置为无硬件流
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//设置为收发模式
	USART_InitStruct.USART_Parity=USART_Parity_No;//奇偶校验位设置为无
	USART_InitStruct.USART_StopBits=USART_StopBits_1;//停止位设置为1位
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//数据字长设置为8位
	USART_Init(USART1, &USART_InitStruct);//串口参数初始化
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启串口1中断
	
	//第六步 
	USART_Cmd(USART1,ENABLE);//使能串口
	
	//第七步 编写中断服务函数
}


void my_usart_dispor_init(u32 bound)//串口3管脚 USART3 TX->PB10   RX->PB11
{
	GPIO_InitTypeDef GPIO_InitStruct;//定义GPIO初始化结构体
	NVIC_InitTypeDef NVIC_InitStruct;//定义一个中断初始化结构体
	USART_InitTypeDef USART_InitStruct;//定义一个USART初始化结构体
	//第一步  串口时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//串口3时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//第二步 串口复位
	USART_DeInit(USART3);
	
	//第三步 GPIO模式设置
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽功能
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);//PB10-TX设置为复用推挽输出模式
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStruct);//PB11-RX设置为浮空输入模式
	
	//第四步 开启中断并设置NVIC
	NVIC_InitStruct.NVIC_IRQChannel= USART3_IRQn;//开启USART串口3的中断
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//开启
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;//设置抢占优先级为3级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=4;//设置子优先级为4级
	NVIC_Init(&NVIC_InitStruct);//中断优先级初始化
	
	//第五步 串口初始化
	USART_InitStruct.USART_BaudRate=bound;//设置波特率
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无硬件流
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//设置为收发模式
	USART_InitStruct.USART_Parity=USART_Parity_No;//设置为无奇偶位
	USART_InitStruct.USART_StopBits=USART_StopBits_1;//设置1位停止位
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;//八位数据格式
	USART_Init(USART3,&USART_InitStruct);
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//开启串口2中断
	
	USART_Cmd(USART3,ENABLE);//使能串口中断
}


char rch[5][3];     //xxx:xxx,xxx,xxx,xxx
u8 i=0;	
u8 p=0;//
u8 cmd=0;		//==1
u8 cmdok=0;	//==1 
u8 ok=0x00;



/////////////////////////////////////////////////////////////////////////////////////
//获取串口字符函数
//1.接线方式 TX-->TX RX->RX 
//2.串口初始化 模块的默认设置为 115200 8位数据位  1开始位 1停止位 无奇偶校验
//3.接收到的字符返回给上一层函数
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
//获取就绪状态函数
//1.接线方式 TX-->TX RX->RX 
//2.串口初始化 模块的默认设置为 115200 8位数据位  1开始位 1停止位 无奇偶校验
//3.串口中断方式接收"OK\r\n"
//4.置位ok全局变量，直到ok变量设置为0x0f，正常获取"OK\r\n"，函数正常退出，否则进入死循环
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
//解析模块发送回来的字符函数
//1.接线方式 TX-->TX RX->RX 
//2.串口初始化 模块的默认设置为 115200 8位数据位  1开始位 1停止位 无奇偶校验
//3.置位ok全局变量和抽取VAR变量的ASCII 字符
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
//VAR变量二次解析函数
//1.接线方式 TX-->TX RX->RX 
//2.串口初始化 模块的默认设置为 115200 8位数据位  1开始位 1停止位 无奇偶校验
//3.VAR变量的ASCII 字符转换为val值，并返回给上一层函数
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
				/*第一个字节*/
				if ((rch[1][2]>=0x30) && (rch[1][2]<=0x39))//100以上的数  
				{		
					val[0]=(rch[1][0]-0x30)*100+(rch[1][1]-0x30)*10+(rch[1][2]-0x30);
					
				}
				else if((rch[1][1]>=0x30) && (rch[1][1]<=0x39)) //10以上的数
				{
					val[0]=(rch[1][0]-0x30)*10+(rch[1][1]-0x30);
				}		
				else if((rch[1][0]>=0x30) && (rch[1][0]<=0x39))
				{
					val[0]=(rch[1][0]-0x30);						
				}
				/*第二个字节*/
					if ((rch[2][2]>=0x30) && (rch[2][2]<=0x39))//100以上的数
				{		
					val[1]=(rch[2][0]-0x30)*100+(rch[2][1]-0x30)*10+(rch[2][2]-0x30);				
				}
				else if((rch[2][1]>=0x30) && (rch[2][1]<=0x39)) //10以上的数
				{
					val[1]=(rch[2][0]-0x30)*10+(rch[2][1]-0x30);
				}		
				else if((rch[2][0]>=0x30) && (rch[2][0]<=0x39))
				{
					val[1]=(rch[2][0]-0x30);						
				}
				
			/*第三个字节*/
					if ((rch[3][2]>=0x30) && (rch[3][2]<=0x39))//100以上的数
				{		
					val[2]=(rch[3][0]-0x30)*100+(rch[3][1]-0x30)*10+(rch[3][2]-0x30);
					
				}
			
				else if((rch[3][1]>=0x30) && (rch[3][1]<=0x39)) //10以上的数
				{
					val[2]=(rch[3][0]-0x30)*10+(rch[3][1]-0x30);
				}		
				else if((rch[3][0]>=0x30) && (rch[3][0]<=0x39))
				{
					val[2]=(rch[3][0]-0x30);						
				}
				/*第四个字节*/
					if ((rch[4][2]>=0x30) && (rch[4][2]<=0x39))//100以上的数
				{		
					val[3]=(rch[4][0]-0x30)*100+(rch[4][1]-0x30)*10+(rch[4][2]-0x30);
					
				}
			
				else if((rch[4][1]>=0x30) && (rch[4][1]<=0x39)) //10以上的数
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
//串口发送数据函数
//1.接线方式 TX-->TX RX->RX 
//2.串口初始化 模块的默认设置为 115200 8位数据位  1开始位 1停止位 无奇偶校验
//3.连续发送字符串，直到字符串结束
//4. databuf 为要发送字符串的地址
//////////////////////////////////////////////////////////////////////////////////////  

int UartSend(char* databuf) //给串口发送信息
{ 
	//u8 i=0;
	while (1)
	{ 
		if ((*databuf)!=0)//直到数据都发送完成 
	{ 
		USART_SendData(USART3, *databuf); //发送一个字节数据
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
			sprintf(buf,"DIR(1);FSIMG(%2d,0,0,320,240,0);\r\n",2097152+320*480*2*i);//在（0,0）位置连续调取8张整屏图片，第一张从地址2097152开始，图片大小480x320，模式0表示正常显示
			UartSend(buf);//发送字符指令串
			CheckBusy();	//等待模块执行完，也就是检测模块发送回来的"OK\r\n"
			delay_ms(50);
		}
	 
		UartSend("DIR(1);CLR(0);DC48(80,10,'深圳尚视界科技',4);\r\n");// 横屏显示，背景颜色为黑色，用黄色以(80,10)为起点,在屏幕上显示透明的'深圳尚视界科技'这四个字,字体大小为48,4为黄色的色表编号
		CheckBusy();

		UartSend("PL(0,60,480,60,15);\r\n");//画直线命令将屏幕上的(0,60)位置的点和（480,60）位置的点用白色(编号15)的直线连起来
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(0,60,480,60,15);\r\n");//画直线命令同上
		CheckBusy();
		delay_ms(800);
	
		UartSend("PL(0,140,480,140,15);\r\n");//画直线命令同上
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(0,230,480,230,15);\r\n");//画直线命令同上
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(0,319,480,320,15);\r\n");//画直线命令同上
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(0,60,0,319,15);\r\n");//画直线命令同上
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(160,60,160,319,15);\r\n");//画直线命令同上
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(340,60,340,319,15);\r\n");//画直线命令同上
		CheckBusy();
		delay_ms(800);
		
		UartSend("PL(479,60,479,319,15);\r\n");//画直线命令同上
		CheckBusy();
		delay_ms(800);
		
		UartSend("CIR(70,100,30,1);\r\n"); //(70,100)为圆心 30为半径用红色画一个线圆 ,1为红色的色表编号
		CheckBusy();
		UartSend("CIRF(80,100,30,2);\r\n"); //(80,100)为圆心 30为半径用绿色画一个填充圆, ,2为绿色的色表编号
		CheckBusy();
		delay_ms(800);
		
		UartSend("BOXF(190,75,310,130,3);\r\n");//对角线(190,75)和(310,130)画填充框 ,3为蓝色的色表编号
		CheckBusy();
		
		UartSend("BOX(200,75,320,130,4);\r\n");//对角线(200,75)和(320,130)画线框 ,4为黄色的色表编号
		CheckBusy();
		
		UartSend("PS(410,85,2);\r\n");//在屏幕的(410,85) 位置用绿色描绘一个点,2为绿色的色表编号
		CheckBusy();	
		
		UartSend("PL(370,105,460,105,1);\r\n");//画直线命令同上
		CheckBusy();
		
		
		UartSend("DC16(50,180,'ABC',3);\r\n");//刷字体命令同上
		CheckBusy();
		
		UartSend("SBC(15);DCV16(80,180,'ABC',0);\r\n"); //用黑色以(80,180)为起点,在屏幕显示带白色背景色的'ABC'这四个字母,字体大小为16,0为黑色的色表编号	
		CheckBusy();
		
		UartSend("DC24(200,170,'123',2);\r\n");//刷字体命令同上
		CheckBusy();
		UartSend("SBC(4);DCV24(240,170,'123',0);\r\n");//刷字体命令同上
		CheckBusy();
		
		UartSend("DC32(380,170,'深圳',2);\r\n");//刷字体命令同上
		CheckBusy();
		
		UartSend("SBC(1);DCV32(40,260,'深圳',15);\r\n"); //刷字体命令同上	
		CheckBusy();
		
		UartSend("DC48(200,260,'中国',6);\r\n");//刷字体命令同上
		CheckBusy();
		
		UartSend("DC72(370,240,'72',1);\r\n"); 	//刷字体命令同上
		CheckBusy();
		delay_ms(800);
		
		UartSend("CLR(0);DIR(1);FSIMG(4247552,0,0,480,320,0);\r\n");//刷图片指令同上
		CheckBusy();	
		UartSend("DC48(110,10,'2017-06-28 ',15);\r\n");//刷字体命令同上
		CheckBusy();
		UartSend("DC32(90,85,' 星期二',15);\r\n");//刷字体命令同上
		CheckBusy();
		
		UartSend("DC32(220,85,' 20 ：',15);\r\n");//刷字体命令同上
		CheckBusy();
		
		UartSend("PL(0,60,480,60,15);\r\n");//画直线命令同上
		CheckBusy();
			 
	{
		 
		 k=0;
		 for(k=0;k<=60;k++)
	 {
			sprintf(buf,"SBC(15);DCV32(320,85,'%d',1);\r\n",k);//刷32号字体变化数字0-60	 
			k=k+1;
			delay_ms(100);
			UartSend(buf);
			CheckBusy();	
						 
	 }
		 k=0;
		 for(k=0;k<=50;k++)
		 {
			sprintf(buf,"SBC(15);DCV32(70,180,'%d',1);\r\n",k);//刷32号字体变化数字0-50	
			k=k+1;
			delay_ms(100);
			UartSend(buf);
			CheckBusy();	
			 
			 sprintf(buf,"SBC(15);DCV32(220,180,'%d',1);\r\n",k);//刷32号字体变化数字0-50	
			 k=k+1;
			 delay_ms(100);
			 UartSend(buf);
			 CheckBusy();
			 			 
			 sprintf(buf,"SBC(15);DCV32(360,180,'%d',1);\r\n",k);//刷32号字体变化数字0-50	
			 k=k+1;
			 delay_ms(100);
			 UartSend(buf);
			 CheckBusy();						 
			}
		 
		}
	 		delay_ms(3000);
		
		UartSend("CLR(0);DIR(1);FSIMG(4554752,0,0,480,320,0);\r\n");//刷图片指令同上
		CheckBusy();	
		
	{
			k=40;

		{
			sprintf(buf,"DCV32(180,150,'亮度：80 ',1);BL(%2d);\r\n",k);//刷32号字体，将亮度变量k转换成"BL(k);\r\n"的字符指令，范围：0为最亮 255为最暗
			UartSend(buf);
			CheckBusy();	
			delay_ms(1000);
		}	
	}	
	
	{
			k=127;
		{
			sprintf(buf,"DCV32(180,150,'亮度：50 ',1);BL(%2d);\r\n",k);//刷32号字体，将亮度变量k转换成"BL(k);\r\n"的字符指令，范围：0为最亮 255为最暗
			UartSend(buf);
			CheckBusy();	
			delay_ms(1000);
		}	
	}	
		
		{
			k=200;

		 {
			sprintf(buf,"DCV32(180,150,'亮度：20 ',1);BL(%2d);\r\n",k);//刷32号字体，将亮度变量k转换成"BL(k);\r\n"的字符指令，范围：0为最亮 255为最暗
			UartSend(buf);
			CheckBusy();	
			delay_ms(1000);
		}	
	 }	
			
	{
			k=0;
		{
			sprintf(buf,"DCV32(170,150,'亮度：100',1);BL(%2d);\r\n",k);//刷32号字体，将亮度变量k转换成"BL(k);\r\n"的字符指令，范围：0为最亮 255为最暗
			UartSend(buf);
			CheckBusy();	
			delay_ms(1000);
		}	
	}	
	  	delay_ms(1600);
		UartSend("DIR(1);CLR(61);DC48(130,140,'演示结束！',15);\r\n");//刷字体命令同上 	
		CheckBusy();
		delay_ms(800);


}


u8 RX_BUF[4];
u8 rx_flag_finished=0;
u8 rx_count=0;
void USART1_IRQHandler(void)//串口1中断服务函数
{
	u8 temp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		temp=USART_ReceiveData(USART1);//接收串口1读取的信息

		USART_SendData(USART3,temp);//将串口1读取的信息发送给串口2	
			
	}
}


void USART3_IRQHandler(void)//串口2中断服务函数
{
	//	u8 temp;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		usart1_temp=USART_ReceiveData(USART3);//接收串口1读取的信息
		USART_SendData(USART1,usart1_temp);//将串口1读取的信息发送给串口2	
		
		
	}
}

#endif

