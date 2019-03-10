#include <reg52.h> //头文件
#define uchar unsigned char
#define uint unsigned int //宏定义
sfr T2MOD = 0xC9;

uchar data buf[4];			//秒显示的变量
uchar data sec_dx = 20;		//东西数默认
uchar data sec_nb = 30;		//南北默认值
uchar data set_timedx = 20; //设置东西方向的时间
uchar data set_timenb = 30; //设置南北方向的时间

uchar data num_che_dx = 0; //设置变量用于记录东西通行车量的个数
uchar data num_che_nb = 0; //设置变量用于记录南北通行车辆的个数

uchar data xianshi_fx = 0; //显示方式
int n;
uchar data countt0, countt1; //定时器0中断次数
//定义6组开关
sbit k1 = P1 ^ 5;	//夜间模式
sbit k2 = P1 ^ 4;	//禁止
sbit k3 = P3 ^ 4;	//确认
sbit k4 = P1 ^ 6;	//东西通行
sbit k5 = P1 ^ 7;	//南北通行
sbit hw_dx = P3 ^ 5; //使用此引脚接红外探头检测"东西"方向通过的车辆当检测到南北有车辆通行时，该引脚会呈现低电平
sbit hw_nb = P3 ^ 6; //使用此引脚接红外探头检测"南北"方向通过的车辆当检测到南北有车辆通行时，该引脚会呈现低电平
sbit k6 = P3 ^ 7;	//切换方向

sbit Red_nb = P2 ^ 6;	//南北红灯标志
sbit Yellow_nb = P2 ^ 5; //南北黄灯标志
sbit Green_nb = P2 ^ 4;  //南北绿灯标志

sbit Red_dx = P2 ^ 3;	//东西红灯标志
sbit Yellow_dx = P2 ^ 2; //东西黄灯标志
sbit Green_dx = P2 ^ 1;  //东西绿灯标志

sbit Buzz = P3 ^ 3;
bit set = 0;	 //调时方向切换键标志 =1时，南北，=0时，东西
bit dx_nb = 0;   //东西南北控制位
bit shanruo = 0; //闪烁标志位
bit yejian = 0;  //夜间黄灯闪烁标志位

uchar code table[11] = {
	//共阴极字型码
	0x3f, //--0
	0x06, //--1
	0x5b, //--2
	0x4f, //--3
	0x66, //--4
	0x6d, //--5
	0x7d, //--6
	0x07, //--7
	0x7f, //--8
	0x6f, //--9
	0x00  //--NULL
};

//函数的声明部分
void delay(int ms); //延时子程序
void key();			//按键扫描子程序
void key_to1();		//键处理子程序
void key_to2();
void key_to3();
void key_to4();
void key_to5();
void display(); //显示子程序
void logo();	//开机LOGO
void Buzzer();
//主程序
void main()
{
	TMOD = 0X11; //定时器设置
	TH1 = 0X3C;
	TL1 = 0XB0;
	TH0 = 0X3C; //定时器0置初值 0.05S
	TL0 = 0XB0;

	T2MOD = 0x01; //自动重载
	T2CON = 0x30; //T2用做发送接收时钟
	TH2 = 0xFF;  //9600波特率,11.0592Mhz晶振
	TL2 = 0xDC; 
	RCAP2H = 0xFF; 
	RCAP2L = 0xDC; 
	SCON = 0x50; //串口方式1,1个起始位,1个停止位,8位数据，可变波特率
	PCON = 0X00;		 //波特率不加倍
 	TR2 = 1;  //启动T2

	ES = 1;				 //开串口中断
	EA = 1;				 //开总中断
	ET0 = 1;			 //定时器0中断开启
	ET1 = 1;			 //定时器1中断开启
	TR0 = 1;			 //启动定时0
	TR1 = 0;			 //关闭定时1
	EX0 = 1;			 //开外部中断0
	EX1 = 1;			 //开外部中断1
	logo();				 //开机初始化
	P2 = 0Xc3;			 // 开始默认状态，东西绿灯，南北黄灯
	sec_nb = sec_dx + 5; //默认南北通行时间比东西多5秒
	while (1)			 //主循环
	{
		key();	 //调用按键扫描程序
		display(); //调用显示程序
	}
}

void key_to1() //夜间模式设置函数
{
	TR0 = 0; //关定时器
	P2 = 0x00;
	TR1 = 1;
	sec_dx = 00; //四个方向的时间都为00
	sec_nb = 00;
}

void key_to2() //禁止通行模式设置函数
{
	TR0 = 0;   //关定时器
	P2 = 0x00; //灭显示
	Red_dx = 1;
	Red_nb = 1; //全部置红灯
	TR1 = 0;
	sec_dx = 00; //四个方向的时间都为00
	sec_nb = 00;
}

void key_to3() //确认设置函数
{
	TR0 = 1;			 //启动定时器0
	xianshi_fx = 0;		 //设置重新显示
	sec_nb = set_timenb; //从中断回复，仍显示设置过的数值
	sec_dx = set_timedx; //显示设置过的时间
	TR1 = 0;			 //关定时器1
	if (set == 0)		 //时间倒时到0时
	{
		P2 = 0X00;			 //灭显示
		Green_dx = 1;		 //东西绿灯亮
		Red_nb = 1;			 //南北红灯亮
		sec_nb = sec_dx + 5; //回到初值
	}
	else
	{
		P2 = 0x00; //南北绿灯，东西红灯
		Green_nb = 1;
		Red_dx = 1;
		sec_dx = sec_nb + 5;
	}
}

void key_to4() //东西通行设置函数
{
	TR0 = 0;	  //关定时器0
	TR1 = 0;	  //关定时器1
	P2 = 0x00;	//灭显示
	Green_dx = 1; //东西方向置绿灯
	Red_nb = 1;   //南北方向为红灯
	sec_dx = 00;  //四个方向的时间都为00
	sec_nb = 00;
}

void key_to5() //南北通行设置函数
{
	TR0 = 0;	  //关定时器0
	TR1 = 0;	  //关定时器1
	P2 = 0x00;	//灭显示
	Green_nb = 1; //置南北方向为绿灯
	Red_dx = 1;   //东西方向为红灯
	sec_nb = 00;  //四个方向的时间都为00
	sec_dx = 00;
}

//函数的定义部分
void key(void) //按键扫描子程序
{
	if (k1 != 1) //当K1（夜间模式）按下
	{
		display();   //调用显示，用于延时消抖
		if (k1 != 1) //如果确定按下
		{
			key_to1(); //夜间模式设置函数
			do
			{
				display();	 //调用显示，用于延时
			} while (k1 != 1); //等待按键释放
		}
	}
	if (k2 != 1) //当K2（禁止）键按下时
	{
		display();   //调用显示，用于延时消抖
		if (k2 != 1) //如果确定按下
		{
			key_to2(); //停止通行设置函数
			do
			{
				display();	 //调用显示，用于延时
			} while (k2 != 1); //等待按键释放
		}
	}
	if (k3 != 1) //当K3（确认）键按下时
	{
		display();   //调用显示，用于延时消抖
		if (k3 != 1) //如果确定按下
		{
			key_to3();
			do
			{
				display();	 //调用显示，用于延时
			} while (k3 != 1); //等待按键释放
		}
	}

	if (k4 != 1) //当K4（东西通行）键按下时
	{
		display();   //调用显示，用于延时消抖
		if (k4 != 1) //如果确定按下
		{
			key_to4(); //东西通行函数
			do
			{
				display();	 //调用显示，用于延时
			} while (k4 != 1); //等待按键释放
		}
	}

	if (k5 != 1) //当K5（南北通行）键按下时
	{
		display();   //调用显示，用于延时消抖
		if (k5 != 1) //如果确定按下
		{
			key_to4(); //南北通行函数
			do
			{
				display();	 //调用显示，用于延时
			} while (k5 != 1); //等待按键释放
		}
	}

	if (hw_dx != 1) //东西红外探头
	{
		display();		//调用显示，用于延时消抖和按键检测方法基本相同
		if (hw_dx != 1) //如果确定有车辆经过
		{
			if (dx_nb == 0) //东西通行的时间
			{
				num_che_dx++; //用于记录东西绿灯时经过的车辆数
			}
			else
			{
				Buzz = 0; //蜂鸣器开
			}
			while (hw_dx != 1)
				; //等待释放
			{
				display(); //调用显示，用于延时
			}
			Buzz = 1; //蜂鸣器关
		}
	}

	if (hw_nb != 1) //南北红外探头
	{
		display();		//调用显示，用于延时消抖和按键检测方法基本相同
		if (hw_nb != 1) //如果确定有车辆经过
		{
			if (dx_nb == 1) //东西通行的时间
			{
				num_che_nb++; //用于记录南北绿灯时经过的车辆数
			}
			else
			{
				Buzz = 0; //蜂鸣器开
			}
			while (hw_nb != 1)
				; //等待释放
			{
				display(); //调用显示，用于延时
			}
			Buzz = 1; //蜂鸣器关
		}
	}
	if (k6 != 1) //（查看）键按下
	{
		display();   //调用显示，用于延时消抖
		if (k6 != 1) //如果确定按下
		{
			xianshi_fx++;
			if (xianshi_fx >= 3)
				xianshi_fx = 2;
			while (k6 != 1)
				; //等待按键释放
			{
				display(); //调用显示，用于延时
			}
		}
	}
}
void display(void) //显示子程序
{

	if (xianshi_fx == 0) //正常显示
	{
		buf[1] = sec_nb / 10; //第1位 东西显示秒十位
		buf[2] = sec_nb % 10; //第2位 东西显示秒个位
		buf[3] = sec_dx / 10; //第3位 南北显示秒十位
		buf[0] = sec_dx % 10; //第4位 南北显示秒个位
	}
	if (xianshi_fx == 1) //查看通行时间
	{
		buf[1] = set_timenb / 10; //第1位 东西通行秒十位
		buf[2] = set_timenb % 10; //第2位 东西通行秒个位
		buf[3] = set_timedx / 10; //第3位 南北通行秒十位
		buf[0] = set_timedx % 10; //第4位 南北通行秒个位
	}

	if (xianshi_fx == 2) //查看红外计数值
	{
		buf[1] = num_che_nb / 10; //第1位 东西红外计数值十位
		buf[2] = num_che_nb % 10; //第2位 东西红外计数值个位
		buf[3] = num_che_dx / 10; //第3位 南北红外计数值十位
		buf[0] = num_che_dx % 10; //第4位 南北红外计数值个位
	}

	P1 = 0xff;			// 初始灯为灭的
	P0 = 0x00;			////灭显示
	P1 = 0xfe;			//片选LED1
	P0 = table[buf[1]]; //送东西时间十位的数码管编码
	delay(1);			//延时
	P1 = 0xff;			//关显示
	P0 = 0x00;			//灭显示

	P1 = 0xfd;			//片选LED2
	P0 = table[buf[2]]; //送东西时间个位的数码管编码
	delay(1);			//延时
	P1 = 0xff;			//关显示
	P0 = 0x00;			//关显示
	P1 = 0Xfb;			//片选LED3

	P0 = table[buf[3]]; //送南北时间十位的数码管编码
	delay(1);			//延时
	P1 = 0xff;			//关显示
	P0 = 0x00;			//关显示
	P1 = 0Xf7;			//片选LED4
	P0 = table[buf[0]]; //送南北时间个位的数码管编码
	delay(1);			//延时
}

void time0(void) interrupt 1 using 1 //定时中断子程序
{
	TH0 = 0X3C;		   //重赋初值
	TL0 = 0XB0;		   //12m晶振50ms//重赋初值
	TR0 = 1;		   //重新启动定时器
	countt0++;		   //软件计数加1
	if (countt0 == 10) //加到10也就是半秒
	{
		if ((sec_nb <= 5) && (dx_nb == 0) && (shanruo == 1)) //东西黄灯闪
		{
			Green_dx = 0;
			Yellow_dx = 0;
			Buzz = 0; //蜂鸣器关
		}
		if ((sec_dx <= 5) && (dx_nb == 1) && (shanruo == 1)) //南北黄灯闪
		{
			Green_nb = 0;
			Yellow_nb = 0;
			Buzz = 0; //蜂鸣器关
		}
	}

	if (countt0 == 20) // 定时器中断次数=20时（即1秒时）
	{
		countt0 = 0; //清零计数器
		sec_dx--;	//东西时间减1
		sec_nb--;	//南北时间减1

		if ((sec_nb <= 5) && (dx_nb == 0) && (shanruo == 1)) //东西黄灯闪
		{
			Green_dx = 0;
			Yellow_dx = 1;
			Buzz = 1; //蜂鸣器关
		}
		if ((sec_dx <= 5) && (dx_nb == 1) && (shanruo == 1)) //南北黄灯闪
		{
			Green_nb = 0;
			Yellow_nb = 1;
			Buzz = 1; //蜂鸣器关
		}
		if (sec_dx == 0 && sec_nb == 5) //当东西倒计时到0时，重置5秒，用于黄灯闪烁时间
		{
			sec_dx = 5;
			shanruo = 1;
		}
		if (sec_nb == 0 && sec_dx == 5) //当南北倒计时到0时，重置5秒，用于黄灯闪烁时间
		{
			sec_nb = 5;
			shanruo = 1;
		}
		if (dx_nb == 0 && sec_nb == 0) //当黄灯闪烁时间倒计时到0时，
		{
			Buzz = 1;  //蜂鸣器开
			P2 = 0x00; //重置东西南背方向的红绿灯
			Green_nb = 1;
			Red_dx = 1;
			dx_nb = !dx_nb;
			shanruo = 0;
			if (num_che_nb > set_timenb / 2) //如果此时南北通行的车辆数大于预设通行量
				set_timenb = set_timenb + 5;
			if (num_che_nb == 0) //如果南北方向无车辆通行,每次递减5秒
				set_timenb = set_timenb - 5;
			if (set_timenb <= 15)
				set_timenb = 15;
			sec_nb = set_timenb;	 //重赋南北方向的起始值
			sec_dx = set_timenb + 5; //重赋东西方向的起始值
			num_che_nb = 0;			 //清零
		}

		if (dx_nb == 1 && sec_dx == 0) //当黄灯闪烁时间到
		{
			P2 = 0X00;						 //重置东西南北的红绿灯状态
			Green_dx = 1;					 //东西绿灯亮
			Red_nb = 1;						 //南北红灯亮
			dx_nb = !dx_nb;					 //取反
			shanruo = 0;					 //闪烁
			if (num_che_dx > set_timedx / 2) //如果此时南北通行的车辆数大于预设通行量
				set_timedx = set_timedx + 5;
			if (num_che_dx == 0) //如果东西方向无车辆通行,每次递减5秒
				set_timedx = set_timedx - 5;
			if (set_timedx <= 15)
				set_timedx = 15;
			sec_dx = set_timedx;	 //重赋东西方向的起始值
			sec_nb = set_timedx + 5; //重赋南北方向的起始值
			num_che_dx = 0;			 //清零
		}
	}
}
void time1(void) interrupt 3 //定时中断子程序
{
	TH1 = 0X3C;		   //重赋初值
	TL1 = 0XB0;		   //12m晶振50ms//重赋初值
	countt1++;		   //软件计数加1
	if (countt1 == 10) // 定时器中断次数=10时（即0.5秒）
	{
		Yellow_nb = 0; //南北黄灯灭
		Yellow_dx = 0; //东西黄灯灭
	}
	if (countt1 == 20) // 定时器中断次数=20时（即1秒时）
	{
		countt1 = 0;   //清零计数器
		Yellow_nb = 1; //南北黄灯亮
		Yellow_dx = 1; //东西黄灯亮
	}
}

//外部中断0
void int0(void) interrupt 0 using 1 
{
}

//外部中断1
void int1(void) interrupt 2 using 1 
{
}

void Com_Int(void) interrupt 4 		//串口中断子函数
{
	uchar i;
	uchar receive_data, *ruturn_data[8];

	EA = 0;

	if (RI == 1) 			//当硬件接收到一个数据时，RI会置位
	{
		RI = 0;
		receive_data = SBUF;		 //接收到的数据

		if (receive_data == '1')		//夜间模式
		{
			key_to1();
			display();
			ruturn_data[8] = "ok-1";
		}
		else if (receive_data == '2')		//停止模式
		{
			key_to2();
			display();
			ruturn_data[8] = "ok-2";
		}
		else if (receive_data == '3')		//确认（重现）模式
		{
			key_to3();
			display();
			ruturn_data[8] = "ok-3";
		}
		else if (receive_data == '4')		//东西通行模式
		{
			key_to4();
			display();
			ruturn_data[8] = "ok-4";
		}
		else if (receive_data == '5')		//南北通行模式
		{
			key_to5();
			display();
			ruturn_data[8] = "ok-5";
		}
		else
		{
			ruturn_data[8] = "error";		//错误命令
		}
	}
	for (i = 0; i < 8; i++)
	{
		SBUF = *ruturn_data[i]; //将要发送的数据放入到发送寄存器
		while (!TI)
			;   //等待发送数据完成
		TI = 0; //清除发送完成标志位
	}
	EA = 1;
}
void logo() //开机的Logo  "- - - -"
{
	for (n = 0; n < 50; n++) //循环显示----50次
	{
		P0 = 0x40; //送形“-”
		P1 = 0xfe; //第一位显示
		delay(1);  //延时
		P1 = 0xfd; //第二位显示
		delay(1);  //延时
		P1 = 0Xfb; //第三位显示
		delay(1);  //延时
		P1 = 0Xf7; //第四位显示
		delay(1);  //延时
		P1 = 0xff; //灭显示
	}
}

void delay(int ms) //延时子程序
{
	uint j, k;
	for (j = 0; j < ms; j++) //延时ms
		for (k = 0; k < 124; k++)
			; //大约1毫秒的延时
}