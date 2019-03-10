#include <reg52.h> //ͷ�ļ�
#define uchar unsigned char
#define uint unsigned int //�궨��
sfr T2MOD = 0xC9;

uchar data buf[4];			//����ʾ�ı���
uchar data sec_dx = 20;		//������Ĭ��
uchar data sec_nb = 30;		//�ϱ�Ĭ��ֵ
uchar data set_timedx = 20; //���ö��������ʱ��
uchar data set_timenb = 30; //�����ϱ������ʱ��

uchar data num_che_dx = 0; //���ñ������ڼ�¼����ͨ�г����ĸ���
uchar data num_che_nb = 0; //���ñ������ڼ�¼�ϱ�ͨ�г����ĸ���

uchar data xianshi_fx = 0; //��ʾ��ʽ
int n;
uchar data countt0, countt1; //��ʱ��0�жϴ���
//����6�鿪��
sbit k1 = P1 ^ 5;	//ҹ��ģʽ
sbit k2 = P1 ^ 4;	//��ֹ
sbit k3 = P3 ^ 4;	//ȷ��
sbit k4 = P1 ^ 6;	//����ͨ��
sbit k5 = P1 ^ 7;	//�ϱ�ͨ��
sbit hw_dx = P3 ^ 5; //ʹ�ô����ŽӺ���̽ͷ���"����"����ͨ���ĳ�������⵽�ϱ��г���ͨ��ʱ�������Ż���ֵ͵�ƽ
sbit hw_nb = P3 ^ 6; //ʹ�ô����ŽӺ���̽ͷ���"�ϱ�"����ͨ���ĳ�������⵽�ϱ��г���ͨ��ʱ�������Ż���ֵ͵�ƽ
sbit k6 = P3 ^ 7;	//�л�����

sbit Red_nb = P2 ^ 6;	//�ϱ���Ʊ�־
sbit Yellow_nb = P2 ^ 5; //�ϱ��ƵƱ�־
sbit Green_nb = P2 ^ 4;  //�ϱ��̵Ʊ�־

sbit Red_dx = P2 ^ 3;	//������Ʊ�־
sbit Yellow_dx = P2 ^ 2; //�����ƵƱ�־
sbit Green_dx = P2 ^ 1;  //�����̵Ʊ�־

sbit Buzz = P3 ^ 3;
bit set = 0;	 //��ʱ�����л�����־ =1ʱ���ϱ���=0ʱ������
bit dx_nb = 0;   //�����ϱ�����λ
bit shanruo = 0; //��˸��־λ
bit yejian = 0;  //ҹ��Ƶ���˸��־λ

uchar code table[11] = {
	//������������
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

//��������������
void delay(int ms); //��ʱ�ӳ���
void key();			//����ɨ���ӳ���
void key_to1();		//�������ӳ���
void key_to2();
void key_to3();
void key_to4();
void key_to5();
void display(); //��ʾ�ӳ���
void logo();	//����LOGO
void Buzzer();
//������
void main()
{
	TMOD = 0X11; //��ʱ������
	TH1 = 0X3C;
	TL1 = 0XB0;
	TH0 = 0X3C; //��ʱ��0�ó�ֵ 0.05S
	TL0 = 0XB0;

	T2MOD = 0x01; //�Զ�����
	T2CON = 0x30; //T2�������ͽ���ʱ��
	TH2 = 0xFF;  //9600������,11.0592Mhz����
	TL2 = 0xDC; 
	RCAP2H = 0xFF; 
	RCAP2L = 0xDC; 
	SCON = 0x50; //���ڷ�ʽ1,1����ʼλ,1��ֹͣλ,8λ���ݣ��ɱ䲨����
	PCON = 0X00;		 //�����ʲ��ӱ�
 	TR2 = 1;  //����T2

	ES = 1;				 //�������ж�
	EA = 1;				 //�����ж�
	ET0 = 1;			 //��ʱ��0�жϿ���
	ET1 = 1;			 //��ʱ��1�жϿ���
	TR0 = 1;			 //������ʱ0
	TR1 = 0;			 //�رն�ʱ1
	EX0 = 1;			 //���ⲿ�ж�0
	EX1 = 1;			 //���ⲿ�ж�1
	logo();				 //������ʼ��
	P2 = 0Xc3;			 // ��ʼĬ��״̬�������̵ƣ��ϱ��Ƶ�
	sec_nb = sec_dx + 5; //Ĭ���ϱ�ͨ��ʱ��ȶ�����5��
	while (1)			 //��ѭ��
	{
		key();	 //���ð���ɨ�����
		display(); //������ʾ����
	}
}

void key_to1() //ҹ��ģʽ���ú���
{
	TR0 = 0; //�ض�ʱ��
	P2 = 0x00;
	TR1 = 1;
	sec_dx = 00; //�ĸ������ʱ�䶼Ϊ00
	sec_nb = 00;
}

void key_to2() //��ֹͨ��ģʽ���ú���
{
	TR0 = 0;   //�ض�ʱ��
	P2 = 0x00; //����ʾ
	Red_dx = 1;
	Red_nb = 1; //ȫ���ú��
	TR1 = 0;
	sec_dx = 00; //�ĸ������ʱ�䶼Ϊ00
	sec_nb = 00;
}

void key_to3() //ȷ�����ú���
{
	TR0 = 1;			 //������ʱ��0
	xianshi_fx = 0;		 //����������ʾ
	sec_nb = set_timenb; //���жϻظ�������ʾ���ù�����ֵ
	sec_dx = set_timedx; //��ʾ���ù���ʱ��
	TR1 = 0;			 //�ض�ʱ��1
	if (set == 0)		 //ʱ�䵹ʱ��0ʱ
	{
		P2 = 0X00;			 //����ʾ
		Green_dx = 1;		 //�����̵���
		Red_nb = 1;			 //�ϱ������
		sec_nb = sec_dx + 5; //�ص���ֵ
	}
	else
	{
		P2 = 0x00; //�ϱ��̵ƣ��������
		Green_nb = 1;
		Red_dx = 1;
		sec_dx = sec_nb + 5;
	}
}

void key_to4() //����ͨ�����ú���
{
	TR0 = 0;	  //�ض�ʱ��0
	TR1 = 0;	  //�ض�ʱ��1
	P2 = 0x00;	//����ʾ
	Green_dx = 1; //�����������̵�
	Red_nb = 1;   //�ϱ�����Ϊ���
	sec_dx = 00;  //�ĸ������ʱ�䶼Ϊ00
	sec_nb = 00;
}

void key_to5() //�ϱ�ͨ�����ú���
{
	TR0 = 0;	  //�ض�ʱ��0
	TR1 = 0;	  //�ض�ʱ��1
	P2 = 0x00;	//����ʾ
	Green_nb = 1; //���ϱ�����Ϊ�̵�
	Red_dx = 1;   //��������Ϊ���
	sec_nb = 00;  //�ĸ������ʱ�䶼Ϊ00
	sec_dx = 00;
}

//�����Ķ��岿��
void key(void) //����ɨ���ӳ���
{
	if (k1 != 1) //��K1��ҹ��ģʽ������
	{
		display();   //������ʾ��������ʱ����
		if (k1 != 1) //���ȷ������
		{
			key_to1(); //ҹ��ģʽ���ú���
			do
			{
				display();	 //������ʾ��������ʱ
			} while (k1 != 1); //�ȴ������ͷ�
		}
	}
	if (k2 != 1) //��K2����ֹ��������ʱ
	{
		display();   //������ʾ��������ʱ����
		if (k2 != 1) //���ȷ������
		{
			key_to2(); //ֹͣͨ�����ú���
			do
			{
				display();	 //������ʾ��������ʱ
			} while (k2 != 1); //�ȴ������ͷ�
		}
	}
	if (k3 != 1) //��K3��ȷ�ϣ�������ʱ
	{
		display();   //������ʾ��������ʱ����
		if (k3 != 1) //���ȷ������
		{
			key_to3();
			do
			{
				display();	 //������ʾ��������ʱ
			} while (k3 != 1); //�ȴ������ͷ�
		}
	}

	if (k4 != 1) //��K4������ͨ�У�������ʱ
	{
		display();   //������ʾ��������ʱ����
		if (k4 != 1) //���ȷ������
		{
			key_to4(); //����ͨ�к���
			do
			{
				display();	 //������ʾ��������ʱ
			} while (k4 != 1); //�ȴ������ͷ�
		}
	}

	if (k5 != 1) //��K5���ϱ�ͨ�У�������ʱ
	{
		display();   //������ʾ��������ʱ����
		if (k5 != 1) //���ȷ������
		{
			key_to4(); //�ϱ�ͨ�к���
			do
			{
				display();	 //������ʾ��������ʱ
			} while (k5 != 1); //�ȴ������ͷ�
		}
	}

	if (hw_dx != 1) //��������̽ͷ
	{
		display();		//������ʾ��������ʱ�����Ͱ�����ⷽ��������ͬ
		if (hw_dx != 1) //���ȷ���г�������
		{
			if (dx_nb == 0) //����ͨ�е�ʱ��
			{
				num_che_dx++; //���ڼ�¼�����̵�ʱ�����ĳ�����
			}
			else
			{
				Buzz = 0; //��������
			}
			while (hw_dx != 1)
				; //�ȴ��ͷ�
			{
				display(); //������ʾ��������ʱ
			}
			Buzz = 1; //��������
		}
	}

	if (hw_nb != 1) //�ϱ�����̽ͷ
	{
		display();		//������ʾ��������ʱ�����Ͱ�����ⷽ��������ͬ
		if (hw_nb != 1) //���ȷ���г�������
		{
			if (dx_nb == 1) //����ͨ�е�ʱ��
			{
				num_che_nb++; //���ڼ�¼�ϱ��̵�ʱ�����ĳ�����
			}
			else
			{
				Buzz = 0; //��������
			}
			while (hw_nb != 1)
				; //�ȴ��ͷ�
			{
				display(); //������ʾ��������ʱ
			}
			Buzz = 1; //��������
		}
	}
	if (k6 != 1) //���鿴��������
	{
		display();   //������ʾ��������ʱ����
		if (k6 != 1) //���ȷ������
		{
			xianshi_fx++;
			if (xianshi_fx >= 3)
				xianshi_fx = 2;
			while (k6 != 1)
				; //�ȴ������ͷ�
			{
				display(); //������ʾ��������ʱ
			}
		}
	}
}
void display(void) //��ʾ�ӳ���
{

	if (xianshi_fx == 0) //������ʾ
	{
		buf[1] = sec_nb / 10; //��1λ ������ʾ��ʮλ
		buf[2] = sec_nb % 10; //��2λ ������ʾ���λ
		buf[3] = sec_dx / 10; //��3λ �ϱ���ʾ��ʮλ
		buf[0] = sec_dx % 10; //��4λ �ϱ���ʾ���λ
	}
	if (xianshi_fx == 1) //�鿴ͨ��ʱ��
	{
		buf[1] = set_timenb / 10; //��1λ ����ͨ����ʮλ
		buf[2] = set_timenb % 10; //��2λ ����ͨ�����λ
		buf[3] = set_timedx / 10; //��3λ �ϱ�ͨ����ʮλ
		buf[0] = set_timedx % 10; //��4λ �ϱ�ͨ�����λ
	}

	if (xianshi_fx == 2) //�鿴�������ֵ
	{
		buf[1] = num_che_nb / 10; //��1λ �����������ֵʮλ
		buf[2] = num_che_nb % 10; //��2λ �����������ֵ��λ
		buf[3] = num_che_dx / 10; //��3λ �ϱ��������ֵʮλ
		buf[0] = num_che_dx % 10; //��4λ �ϱ��������ֵ��λ
	}

	P1 = 0xff;			// ��ʼ��Ϊ���
	P0 = 0x00;			////����ʾ
	P1 = 0xfe;			//ƬѡLED1
	P0 = table[buf[1]]; //�Ͷ���ʱ��ʮλ������ܱ���
	delay(1);			//��ʱ
	P1 = 0xff;			//����ʾ
	P0 = 0x00;			//����ʾ

	P1 = 0xfd;			//ƬѡLED2
	P0 = table[buf[2]]; //�Ͷ���ʱ���λ������ܱ���
	delay(1);			//��ʱ
	P1 = 0xff;			//����ʾ
	P0 = 0x00;			//����ʾ
	P1 = 0Xfb;			//ƬѡLED3

	P0 = table[buf[3]]; //���ϱ�ʱ��ʮλ������ܱ���
	delay(1);			//��ʱ
	P1 = 0xff;			//����ʾ
	P0 = 0x00;			//����ʾ
	P1 = 0Xf7;			//ƬѡLED4
	P0 = table[buf[0]]; //���ϱ�ʱ���λ������ܱ���
	delay(1);			//��ʱ
}

void time0(void) interrupt 1 using 1 //��ʱ�ж��ӳ���
{
	TH0 = 0X3C;		   //�ظ���ֵ
	TL0 = 0XB0;		   //12m����50ms//�ظ���ֵ
	TR0 = 1;		   //����������ʱ��
	countt0++;		   //���������1
	if (countt0 == 10) //�ӵ�10Ҳ���ǰ���
	{
		if ((sec_nb <= 5) && (dx_nb == 0) && (shanruo == 1)) //�����Ƶ���
		{
			Green_dx = 0;
			Yellow_dx = 0;
			Buzz = 0; //��������
		}
		if ((sec_dx <= 5) && (dx_nb == 1) && (shanruo == 1)) //�ϱ��Ƶ���
		{
			Green_nb = 0;
			Yellow_nb = 0;
			Buzz = 0; //��������
		}
	}

	if (countt0 == 20) // ��ʱ���жϴ���=20ʱ����1��ʱ��
	{
		countt0 = 0; //���������
		sec_dx--;	//����ʱ���1
		sec_nb--;	//�ϱ�ʱ���1

		if ((sec_nb <= 5) && (dx_nb == 0) && (shanruo == 1)) //�����Ƶ���
		{
			Green_dx = 0;
			Yellow_dx = 1;
			Buzz = 1; //��������
		}
		if ((sec_dx <= 5) && (dx_nb == 1) && (shanruo == 1)) //�ϱ��Ƶ���
		{
			Green_nb = 0;
			Yellow_nb = 1;
			Buzz = 1; //��������
		}
		if (sec_dx == 0 && sec_nb == 5) //����������ʱ��0ʱ������5�룬���ڻƵ���˸ʱ��
		{
			sec_dx = 5;
			shanruo = 1;
		}
		if (sec_nb == 0 && sec_dx == 5) //���ϱ�����ʱ��0ʱ������5�룬���ڻƵ���˸ʱ��
		{
			sec_nb = 5;
			shanruo = 1;
		}
		if (dx_nb == 0 && sec_nb == 0) //���Ƶ���˸ʱ�䵹��ʱ��0ʱ��
		{
			Buzz = 1;  //��������
			P2 = 0x00; //���ö����ϱ�����ĺ��̵�
			Green_nb = 1;
			Red_dx = 1;
			dx_nb = !dx_nb;
			shanruo = 0;
			if (num_che_nb > set_timenb / 2) //�����ʱ�ϱ�ͨ�еĳ���������Ԥ��ͨ����
				set_timenb = set_timenb + 5;
			if (num_che_nb == 0) //����ϱ������޳���ͨ��,ÿ�εݼ�5��
				set_timenb = set_timenb - 5;
			if (set_timenb <= 15)
				set_timenb = 15;
			sec_nb = set_timenb;	 //�ظ��ϱ��������ʼֵ
			sec_dx = set_timenb + 5; //�ظ������������ʼֵ
			num_che_nb = 0;			 //����
		}

		if (dx_nb == 1 && sec_dx == 0) //���Ƶ���˸ʱ�䵽
		{
			P2 = 0X00;						 //���ö����ϱ��ĺ��̵�״̬
			Green_dx = 1;					 //�����̵���
			Red_nb = 1;						 //�ϱ������
			dx_nb = !dx_nb;					 //ȡ��
			shanruo = 0;					 //��˸
			if (num_che_dx > set_timedx / 2) //�����ʱ�ϱ�ͨ�еĳ���������Ԥ��ͨ����
				set_timedx = set_timedx + 5;
			if (num_che_dx == 0) //������������޳���ͨ��,ÿ�εݼ�5��
				set_timedx = set_timedx - 5;
			if (set_timedx <= 15)
				set_timedx = 15;
			sec_dx = set_timedx;	 //�ظ������������ʼֵ
			sec_nb = set_timedx + 5; //�ظ��ϱ��������ʼֵ
			num_che_dx = 0;			 //����
		}
	}
}
void time1(void) interrupt 3 //��ʱ�ж��ӳ���
{
	TH1 = 0X3C;		   //�ظ���ֵ
	TL1 = 0XB0;		   //12m����50ms//�ظ���ֵ
	countt1++;		   //���������1
	if (countt1 == 10) // ��ʱ���жϴ���=10ʱ����0.5�룩
	{
		Yellow_nb = 0; //�ϱ��Ƶ���
		Yellow_dx = 0; //�����Ƶ���
	}
	if (countt1 == 20) // ��ʱ���жϴ���=20ʱ����1��ʱ��
	{
		countt1 = 0;   //���������
		Yellow_nb = 1; //�ϱ��Ƶ���
		Yellow_dx = 1; //�����Ƶ���
	}
}

//�ⲿ�ж�0
void int0(void) interrupt 0 using 1 
{
}

//�ⲿ�ж�1
void int1(void) interrupt 2 using 1 
{
}

void Com_Int(void) interrupt 4 		//�����ж��Ӻ���
{
	uchar i;
	uchar receive_data, *ruturn_data[8];

	EA = 0;

	if (RI == 1) 			//��Ӳ�����յ�һ������ʱ��RI����λ
	{
		RI = 0;
		receive_data = SBUF;		 //���յ�������

		if (receive_data == '1')		//ҹ��ģʽ
		{
			key_to1();
			display();
			ruturn_data[8] = "ok-1";
		}
		else if (receive_data == '2')		//ֹͣģʽ
		{
			key_to2();
			display();
			ruturn_data[8] = "ok-2";
		}
		else if (receive_data == '3')		//ȷ�ϣ����֣�ģʽ
		{
			key_to3();
			display();
			ruturn_data[8] = "ok-3";
		}
		else if (receive_data == '4')		//����ͨ��ģʽ
		{
			key_to4();
			display();
			ruturn_data[8] = "ok-4";
		}
		else if (receive_data == '5')		//�ϱ�ͨ��ģʽ
		{
			key_to5();
			display();
			ruturn_data[8] = "ok-5";
		}
		else
		{
			ruturn_data[8] = "error";		//��������
		}
	}
	for (i = 0; i < 8; i++)
	{
		SBUF = *ruturn_data[i]; //��Ҫ���͵����ݷ��뵽���ͼĴ���
		while (!TI)
			;   //�ȴ������������
		TI = 0; //���������ɱ�־λ
	}
	EA = 1;
}
void logo() //������Logo  "- - - -"
{
	for (n = 0; n < 50; n++) //ѭ����ʾ----50��
	{
		P0 = 0x40; //���Ρ�-��
		P1 = 0xfe; //��һλ��ʾ
		delay(1);  //��ʱ
		P1 = 0xfd; //�ڶ�λ��ʾ
		delay(1);  //��ʱ
		P1 = 0Xfb; //����λ��ʾ
		delay(1);  //��ʱ
		P1 = 0Xf7; //����λ��ʾ
		delay(1);  //��ʱ
		P1 = 0xff; //����ʾ
	}
}

void delay(int ms) //��ʱ�ӳ���
{
	uint j, k;
	for (j = 0; j < ms; j++) //��ʱms
		for (k = 0; k < 124; k++)
			; //��Լ1�������ʱ
}