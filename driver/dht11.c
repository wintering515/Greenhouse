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
