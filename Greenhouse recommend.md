# Greenhouse recommend

#本项目仅完成基础模块，进阶模块正开发当中

## A.配件清单

1.基础配件：STM32F103开发板，DHT11温度湿度模块，光敏电阻配件，0.96寸OLED屏幕

2.进阶模块：JDY31蓝牙模块，电机风扇模块，MQ1135模块

## B.功能简介

1.基础功能：使用DHT11模块检测当前的坏境温度湿度，光敏电阻模块检测当前坏境的光照强度，并且使用OLED屏幕显示出来

2.进阶功能：通过蓝牙模块将数据上传到手机端检测，设置特定的阈值，当坏境温度或者湿度超过设置的阈值时，蜂鸣器报警，并且使用电机模块进行降温操作，同时可以使用MQ135模块检测当前环境的参数。

## C.模块介绍

1.光敏电阻传感器模块

#D0开关led灯（0和1）（TTL开关信号输出）【坏境光线亮度达不到设定阈值时D0输出高电平，超过设定阈值时输出低电平】& A0输入电压（模拟信号输出）【与AD模块相连，通过AD转换可以获得坏境光亮的更精确的数值

2.DHT11数字温度湿度传感器模块

#接口说明：3个引脚（VDD，GND，DATA数据线）![1716107496392](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716107496392.png)

#通讯过程：拉低时高电平，拉高时低电平

![1716107524886](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716107524886.png)

![1716107589288](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716107589288.png)

3.OLED显示屏原理

#接口说明：4个引脚（GND，VDD，SCLK时钟线，SDA数据线）

![1716109951428](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716109951428.png)

#读地址：0111 1001=0x79

#写地址：0111 1000=0x78

#命令0x00

#发送数据0x40

## D. 主要步骤

### 1.开发板操作部分

1）基础操作：

（1）SYS->Debug：Serial Wire![1716105997416](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716105997416.png)

（2）RCC

![1716106326746](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716106326746.png)

（3）clock configuration--72

![1716106367812](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716106367812.png)

（4）project manager：姓名，位置，IDE，生成c和h文件

![1716106419126](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716106419126.png)

![1716106434995](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716106434995.png)

2）光敏电阻配置：打开IN0 连接上了A0引脚

![1716106501918](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716106501918.png)

3）DHT11配置开启clock source 并设置PSC为72-1

![1716107723219](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716107723219.png)

4）OLED配置I^2C

![1716110207438](C:\Users\winter\AppData\Roaming\Typora\typora-user-images\1716110207438.png)

### 2.编程部分

1）main.c

（1）定义ADC的值和开启ADC通道

（2）在完成dht11.c的编写后加入定义两个变量和一个延时（延时里包括了显示温度和湿度数据）

```c
uint16_t adc_val = 0;//define the number of getting from ADC
float adc_volt = 0;

int temp = 0;
int humity = 0;
while (1)
  {
		HAL_ADC_Start(&hadc1);//turn on ADC channel
		adc_val = HAL_ADC_GetValue(&hadc1);//get the data from ADC
		adc_volt = adc_val/4096.0f * 3.3f;//trans from voltage
    
    DHT11_Task();
		temp = DHT11_data.temp;
		humity = DHT11_data.humidity;
		
		OLED_ShowNum(0, 4, temp, 2, 16);//display the temp
		OLED_ShowNum(32, 4, humity, 2, 16);//display the humity
		
		HAL_Delay(2000);
  }
}
```

（2）包含头文件

```c
#include "dht11.h"
#include "oled.h"
```

（3）显示字符

```c
OLED_Init();//initialize
	OLED_Clear();//clear screen
	OLED_ShowString(0, 0, "LIU", 16);// 0 1
	OLED_ShowString(0, 2, "WEI", 16);// 2
```







2）dht11.h（温度湿度部分）

#新建文件夹driver存放驱动文件dht11.c和dht11.h，工程里添加manage project items添加driver文件夹，利用add files to group driver将c文件导入，别忘了要去添加路径

#定义头文件，宏，数据引脚（控制引脚高低电平，读取数据，引脚接口）

#编写DHT11所产生的数据（40bit的数组，温度，湿度）

```c
#ifndef __DHT11_H__
#define __DHT11_H__
#include "main.h"
#define u8 unsigned char //define some macro
#define u16 unsigned short 
#define u32 unsigned int

//define data GPIO
#define DATA_SET() 		HAL_GPIO_WritePin(DATA_GPIO_Port, DATA_Pin, GPIO_PIN_SET)//define high level
#define DATA_RESET()  HAL_GPIO_WritePin(DATA_GPIO_Port, DATA_Pin, GPIO_PIN_RESET)//define low level
#define DATA_READ()		HAL_GPIO_ReadPin(DATA_GPIO_Port, DATA_Pin)//define read function
#define DATA_Pin				GPIO_PIN_7//optimization code
#define DATA_GPIO_Port	GPIOA

typedef struct//structure
{
	u8 Data[5];//data array
	u8 index;
	u8 temp;
	u8 humidity;
	
}DHT11_DATA;

extern DHT11_DATA DHT11_data;

void DHT11_Task(void);

#endif

```

3）dht11.c

#头文件

#定义温度湿度变量

#用extern声明延时函数

#void1：微秒级别的演示函数

#void2：设置DHT11引脚为输入引脚的函数

#u8 1：配置输入函数

#u8 2：读取字节的函数：for循环读取字节不超过8bit，时间不超过100，时间到达自动退出循环，读取数据等于1时使用中间变量，最后进行组合

#u8 3：提供读取函数：发送开始信号--输出模式：主机拉低18ms--主机拉高20ms--输入模式：延时20ms--if函数和while函数来储存变量--校验检测是否成功

#void3：编写一个函数将数据供给主函数使用

```c
#include "dht11.h"

extern TIM_HandleTypeDef htim1;//statement 

DHT11_DATA DHT11_data;

void Delay_us(uint16_t us)//dalay function
{
	uint16_t differ = 0xffff - us - 5;
	__HAL_TIM_SET_COUNTER(&htim1, differ);
	HAL_TIM_Base_Start(&htim1);
	
	while(differ < 0xffff - 5)//define timer
	{
		differ = __HAL_TIM_GET_COUNTER(&htim1);
	}
	HAL_TIM_Base_Stop(&htim1);//turn off timer
}

void DATA_OUTPUT(u8 flg)//define that the dht11 is the output pin
	
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = DATA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;//the pin speed is hith
	
	HAL_GPIO_Init(DATA_GPIO_Port, &GPIO_InitStruct);//initialize pin
	
	if(flg == 0)//when the flg==0,ouput the low level
	{
		DATA_RESET();
	}
	else
	{
		DATA_SET();//when the flg==1,ouput the hige level
	}
}


u8 DATA_INPUT(void)//define the input
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	u8 flg = 0;
	
	GPIO_InitStruct.Pin = DATA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;//pull up a resistor
	
	HAL_GPIO_Init(DATA_GPIO_Port, &GPIO_InitStruct);//initialize

	if(DATA_READ() == GPIO_PIN_RESET)//read the data
	{
		flg = 0;//if the level is low,return 0
	}
	else
	{
		flg = 1;//if the level is high,return 1
	}
	
	return flg;
}

u8 DHT11_Read_Byte(void)//read byte function
{
	u8 ReadDat = 0;
	u8 temp = 0;
	u8 retry = 0;//timeout mechanism
	u8 i = 0;
	
	for(i = 0; i < 8; i++)
	{
		while(DATA_READ() == 0 && retry < 100)
		{
			Delay_us(1);//dalay function
			retry++;//when the time is arrive, exit this loop
		}
		retry = 0;//clear variable
		Delay_us(40);//delay function
		if(DATA_READ() == 1)
		{
			temp = 1;//if read function=1, intermediate variable is 1
		}
		else
		{
			temp = 0;//else the intermediate variable is 0
		}
		while(DATA_READ() == 1 && retry < 100)
		{
			Delay_us(1);
			retry++;			
		}
		retry = 0;
		
		ReadDat<<=1;
		ReadDat|=temp;
	}
	
	return ReadDat;
}

u8 DHT11_Read(void)//read function
{
	u8 retry = 0;//begin signal
	u8 i = 0;
	
	DATA_OUTPUT(0);//mode output
	HAL_Delay(18);//delay  18ms,because the schematic diagram show the device need pull down 18ms
	DATA_SET();
	Delay_us(20);//delay 20ms,because the schematic diagram show the device need pull up 20ms
	
	DATA_INPUT();//mode input
	Delay_us(20);//dalay 20ms
	
	if(DATA_READ() == 0)
	{
		while(DATA_READ() == 0 && retry < 100)//use while loop to determine timeout
		{
			Delay_us(1);
			retry++;
		}
		retry = 0;
		while(DATA_READ() == 1 && retry < 100)
		{
			Delay_us(1);
			retry++;
		}
		retry = 0;//after finish two while lop,that means we receive the response signal

		for(i = 0; i < 5; i++)//get the data
		{
			DHT11_data.Data[i] = DHT11_Read_Byte();			
		}
		Delay_us(50);
	}
	u32 sum = DHT11_data.Data[0] + DHT11_data.Data[1] + DHT11_data.Data[2] + DHT11_data.Data[3];
	//the checksum is equal to the sum of the first four digits
	
	if((sum) == DHT11_data.Data[4])//if checksum==data[4],that means the data is correct
	{
		DHT11_data.humidity = DHT11_data.Data[0];//save the integer part of the data
		DHT11_data.temp = DHT11_data.Data[2];
		return 1;
	}
	else
	{
		return 0;
	}
}

void Test(void)
{
	if(DHT11_Read())
	{
		DHT11_data.index++;
		if(DHT11_data.index >= 128)
		{
			DHT11_data.index = 0;
		}
	}
}


void DHT11_Task(void)
{
	Test();
}

```

4）oled.c（OLED屏幕部分）

#添加OLED驱动（仅会用不用自己写）

#头文件

#根据命令通过二进制转换床十六进制

#unit8_t：写地址

#void1：向设备写控制命令

#void2：向设备写数据

#void3：初始化led屏幕

#void4：清屏

#void5：开启OLED显示

#void6：关闭OLED显示

。。。

#void9：显示2个数字

#void10：显示字符

#void11：显示一个字符串

#void12：显示汉字

```c
#include "oled.h"
#include "i2c.h"
#include "oledfont.h"          //head project
#include "oledBMP.h"

uint8_t CMD_Data[] = {
0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 0xA6, 0xA8, 0x3F,

0xC8, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1, 0xDA, 0x12,

0xD8, 0x30, 0x8D, 0x14, 0xAF };      //initialize command,following to the command table in the guide



void WriteCmd(void)// coding the command to the device,0x78 means count accounding to the schematic diagram
//0x00 means command
//0x40 means data 
{
	uint8_t i = 0;
	for (i = 0; i < 27; i++)
	{
		HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, CMD_Data + i, 1, 0x100);
	}
}

void OLED_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0x100);
}

void OLED_WR_DATA(uint8_t data)// coding the data to the device
{
	HAL_I2C_Mem_Write(&hi2c1, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
}

void OLED_Init(void)// initialize oled screen
{
	HAL_Delay(200);//delay 200ms

	WriteCmd();//rewrite the command
}

void OLED_Clear(void)//clear screen
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_CMD(0xb0 + i);
		OLED_WR_CMD(0x00);
		OLED_WR_CMD(0x10);
		for (n = 0; n < 128; n++)
			OLED_WR_DATA(0);
	}
}
   
void OLED_Display_On(void)//turn OLED reveal
{
	OLED_WR_CMD(0X8D);  //SET DCDC command
	OLED_WR_CMD(0X14);  //DCDC ON
	OLED_WR_CMD(0XAF);  //DISPLAY ON
}
    
void OLED_Display_Off(void)//turn off OLED reveal
{
	OLED_WR_CMD(0X8D);  //SET DCDC command 
	OLED_WR_CMD(0X10);  //DCDC OFF
	OLED_WR_CMD(0XAE);  //DISPLAY OFF
}
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
	OLED_WR_CMD(0xb0 + y);
	OLED_WR_CMD(((x & 0xf0) >> 4) | 0x10);
	OLED_WR_CMD(x & 0x0f);
}

void OLED_On(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_CMD(0xb0 + i);    //define address(0~7)
		OLED_WR_CMD(0x00);      //define the display location -low address(bit)
		OLED_WR_CMD(0x10);      //define the display location--high address(bit)
		for (n = 0; n < 128; n++)
			OLED_WR_DATA(1);
	} //update reveal
}
unsigned int oled_pow(uint8_t m, uint8_t n)
{
	unsigned int result = 1;
	while (n--)result *= m;
	return result;
}
//display 2 number
//x,y :origin coordinate
//len :numberic bit
//size:font size
//mode:mode0,mode1,overlay mode
//num:number(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x, uint8_t y, unsigned int num, uint8_t len, uint8_t size2)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size2 / 2)*t, y, ' ', size2);
				continue;
			}
			else enshow = 1;

		}
		OLED_ShowChar(x + (size2 / 2)*t, y, temp + '0', size2);
	}
}
//displays a character,including partial characters, at a specified
//x:0~127
//y:0~63
//mode:0,white;1,normal
//size:choose font 16/12 
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
	unsigned char c = 0, i = 0;
	c = chr - ' ';//the offset value is obtained		
	if (x > 128 - 1) { x = 0; y = y + 2; }
	if (Char_Size == 16)
	{
		OLED_Set_Pos(x, y);
		for (i = 0; i < 8; i++)
			OLED_WR_DATA(F8X16[c * 16 + i]);
		OLED_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++)
			OLED_WR_DATA(F8X16[c * 16 + i + 8]);
	}
	else {
		OLED_Set_Pos(x, y);
		for (i = 0; i < 6; i++)
			OLED_WR_DATA(F6x8[c][i]);

	}
}


void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size)//display string
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], Char_Size);
		x += 8;
		if (x > 120) { x = 0; y += 2; }
		j++;
	}
}
//display chinese characters
//hzk-Array obtained using modulo software
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
	uint8_t t, adder = 0;
	OLED_Set_Pos(x, y);
	for (t = 0; t < 16; t++)
	{
		OLED_WR_DATA(Hzk[2 * no][t]);
		adder += 1;
	}
	OLED_Set_Pos(x, y + 1);
	for (t = 0; t < 16; t++)
	{
		OLED_WR_DATA(Hzk[2 * no + 1][t]);
		adder += 1;
	}
}

```

