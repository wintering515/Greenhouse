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
