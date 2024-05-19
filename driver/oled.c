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
