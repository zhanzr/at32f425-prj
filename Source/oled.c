#include "at32f425.h"

#include "oled.h"
#include "custom_at32f425_board.h"
#include "i2c_application.h"

#define I2C_TIMEOUT                      0xFFFFFFF

// SSD1306 IIC MD066
#define MD066_CMD 0  //写命令
#define MD066_DATA 1 //写数据
#define Max_Column 128
#define Max_Row 64

extern const unsigned char F6x8[96][6];
extern const unsigned char F8X16[95];
extern const uint8_t BMP1[];
extern const uint8_t BMP2[];
extern const uint8_t BMP1_MD066[];
extern const uint8_t BMP2_MD066[];
extern const uint8_t BMP3[];
extern const uint8_t BMP4[];

extern volatile uint32_t g_Ticks;
extern i2c_handle_type hi2cx;

void LCD_DLY_ms(uint32_t ms) {
    uint32_t test_tick_0 = g_Ticks;
    while ((test_tick_0 + ms) > g_Ticks) {
      __NOP();
    }
}

#define	HAL_Delay	LCD_DLY_ms

/**
  * @brief  error handler program
  * @param  i2c_status
  * @retval none
  */
void error_handler(uint32_t error_code)
{
  while(1)
  {
    at32_led_toggle(LED2);
    delay_ms(500);
  }
}

uint32_t oled_pow(uint8_t m, uint8_t n) {
  uint32_t result = 1;
  while (n--)
    result *= m;
  return result;
}

/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte_Buf(uint8_t slave_addr, uint8_t *buf, uint16_t size) {
		  i2c_status_type i2c_status;

	    /* write data to memory device */  
    if((i2c_status = i2c_master_transmit(&hi2cx, slave_addr, buf, size, I2C_TIMEOUT)) != I2C_OK)
    {
      error_handler(i2c_status);
    }
}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command) {
  uint8_t buf[2] = {0x00, IIC_Command};
  Write_IIC_Byte_Buf(0x78, buf, 2);
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data) {
  uint8_t buf[2] = {0x40, IIC_Data};
  Write_IIC_Byte_Buf(0x78, buf, 2);
}

void MD066_WR_Byte(unsigned dat, unsigned cmd) {
  if (cmd) {

    Write_IIC_Data(dat);

  } else {
    Write_IIC_Command(dat);
  }
}

/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data) {
  unsigned char m, n;
  for (m = 0; m < 8; m++) {
    MD066_WR_Byte(0xb0 + m, 0); // page0-page1
    MD066_WR_Byte(0x00, 0);     // low column start address
    MD066_WR_Byte(0x10, 0);     // high column start address
    for (n = 0; n < 128; n++) {
      MD066_WR_Byte(fill_Data, 1);
    }
  }
}

void MD066_Set_Pos(unsigned char x, unsigned char y) {
  MD066_WR_Byte(0xb2 + y, MD066_CMD);
  MD066_WR_Byte((((x + 0x20) & 0xf0) >> 4) | 0x10, MD066_CMD);
  MD066_WR_Byte((x & 0x0f) | 0x01, MD066_CMD);
}

void MD066_Display_On(void) {
  MD066_WR_Byte(0X8D, MD066_CMD); // SET DCDC命令
  MD066_WR_Byte(0X14, MD066_CMD); // DCDC ON
  MD066_WR_Byte(0XAF, MD066_CMD); // DISPLAY ON
}

void MD066_Display_Off(void) {
  MD066_WR_Byte(0X8D, MD066_CMD); // SET DCDC命令
  MD066_WR_Byte(0X10, MD066_CMD); // DCDC OFF
  MD066_WR_Byte(0XAE, MD066_CMD); // DISPLAY OFF
}

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void MD066_Clear(void) {
  uint8_t i, n;
  for (i = 0; i < 8; i++) // 8
  {
    MD066_WR_Byte(0xb2 + i, MD066_CMD); //设置页地址（0~7）
    MD066_WR_Byte(0x00, MD066_CMD);     //设置显示位置—列低地址
    MD066_WR_Byte(0x12, MD066_CMD);     //设置显示位置—列高地址
    for (n = 0; n < 64; n++)
      MD066_WR_Byte(0, MD066_DATA);
  } //更新显示
}

//在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反白显示;1,正常显示
// size:选择字体 16/12
void MD066_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size) {
  unsigned char c = 0, i = 0;
  c = chr - ' '; //得到偏移后的值
  if (x > Max_Column - 1) {
    x = 0;
    y = y + 2;
  }
  if (Char_Size == 16) {
    MD066_Set_Pos(x, y);
    for (i = 0; i < 8; i++)
      MD066_WR_Byte(F8X16[c * 16 + i], MD066_DATA);
    MD066_Set_Pos(x, y + 1);
    for (i = 0; i < 8; i++)
      MD066_WR_Byte(F8X16[c * 16 + i + 8], MD066_DATA);
  } else {
    MD066_Set_Pos(x, y);
    for (i = 0; i < 6; i++)
      MD066_WR_Byte(F6x8[c][i], MD066_DATA);
  }
}

//显示2个数字
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// mode:模式	0,填充模式;1,叠加模式
// num:数值(0~4294967295);
void MD066_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
                   uint8_t size2) {
  uint8_t t, temp;
  uint8_t enshow = 0;
  for (t = 0; t < len; t++) {
    temp = (num / oled_pow(10, len - t - 1)) % 10;
    if (enshow == 0 && t < (len - 1)) {
      if (temp == 0) {
        MD066_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
        continue;
      } else
        enshow = 1;
    }
    MD066_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
  }
}
//显示一个字符号串
void MD066_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t Char_Size) {
  unsigned char j = 0;
  while (chr[j] != '\0') {
    MD066_ShowChar(x, y, chr[j], Char_Size);
    x += 8;
    if (x > 120) {
      x = 0;
      y += 2;
    }
    j++;
  }
}

void MD066_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1,
                   unsigned char y1, unsigned char BMP[]) {
  unsigned int j = 0;
  unsigned char x, y;

  if (y1 % 8 == 0)
    y = y1 / 8;
  else
    y = y1 / 8 + 1;
  for (y = y0; y < y1; y++) {
    MD066_Set_Pos(x0, y);
    for (x = x0; x < x1; x++) {
      MD066_WR_Byte(BMP[j++], MD066_DATA);
    }
  }
}

void MD066_Init(void) {
  MD066_WR_Byte(0xAE, MD066_CMD); //--turn off oled panel 0.0

  MD066_WR_Byte(0x00, MD066_CMD); //---set low column address 00
  MD066_WR_Byte(0x12, MD066_CMD); //---set high column address 12
  MD066_WR_Byte(0x40, MD066_CMD); //--set start line address  Set Mapping RAM
                                  // Display Start Line (0x00~0x3F)
  MD066_WR_Byte(0x81, MD066_CMD); //--set contrast control register
  MD066_WR_Byte(0xCF, MD066_CMD); // Set SEG Output Current Brightness
  MD066_WR_Byte(
      0xA1, MD066_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常 A1
  MD066_WR_Byte(
      0xC8, MD066_CMD); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常

  MD066_WR_Byte(0xA6, MD066_CMD); //--set normal display A6

  MD066_WR_Byte(0xA8, MD066_CMD); //--set multiplex ratio(1 to 64) 0.0

  MD066_WR_Byte(0x3f, MD066_CMD); //--1/64 duty ***** 0.0

  MD066_WR_Byte(
      0xD3,
      MD066_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  MD066_WR_Byte(0x00, MD066_CMD); //-not offset

  MD066_WR_Byte(
      0xd5,
      MD066_CMD); //--set display clock divide ratio/oscillator frequency 0.0

  MD066_WR_Byte(
      0x80, MD066_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec 0.0

  MD066_WR_Byte(0xD9, MD066_CMD); //--set pre-charge period
  MD066_WR_Byte(
      0xF1, MD066_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  MD066_WR_Byte(0xDA, MD066_CMD); //--set com pins hardware configuration
  MD066_WR_Byte(0x12, MD066_CMD);

  MD066_WR_Byte(0xDB, MD066_CMD); //--set vcomh
  MD066_WR_Byte(0x40, MD066_CMD); // Set VCOM Deselect Level

  MD066_WR_Byte(0x20,
                MD066_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02) 0.0
  MD066_WR_Byte(0x02, MD066_CMD); // 0.0

  MD066_WR_Byte(0x8D, MD066_CMD); //--set Charge Pump enable/disable
  MD066_WR_Byte(0x14, MD066_CMD); //--set(0x10) disable

  MD066_WR_Byte(0xA4, MD066_CMD); // Disable Entire Display On (0xa4/0xa5)
  MD066_WR_Byte(0xA6, MD066_CMD); // Disable Inverse Display On (0xa6/a7)
  MD066_WR_Byte(0xAF, MD066_CMD); //--turn on oled panel

  MD066_WR_Byte(0xAF, MD066_CMD); /*display ON*/
  MD066_Clear();
  MD066_Set_Pos(0, 0);
}

void MD066_Demo(void) {
  uint8_t t = ' ';

  MD066_Clear();

  MD066_ShowString(0, 0, "ABCDEFGH", 8);
  HAL_Delay(500);
  MD066_ShowString(0, 1, "abcdefgh", 8);
  HAL_Delay(500);
  MD066_ShowString(0, 2, "01234567", 8);
  HAL_Delay(500);
  MD066_ShowString(0, 3, "~!@#$%^&", 8);
  HAL_Delay(500);
  MD066_ShowString(0, 4, "ABCDEFGH", 8);
  HAL_Delay(500);
  MD066_ShowString(0, 5, "01234567", 8);
  HAL_Delay(500);

  MD066_ShowString(0, 2, "  0.66' ", 16);
  MD066_ShowString(0, 4, " O L E D", 16);
  MD066_ShowChar(48, 6, t, 16);
  HAL_Delay(1000);

  MD066_DrawBMP(0, 0, 64, 6, (uint8_t *)BMP1_MD066);
  HAL_Delay(1000);

  MD066_DrawBMP(0, 0, 64, 6, (uint8_t *)BMP2_MD066);
  HAL_Delay(1000);

  MD066_DrawBMP(0, 0, 64, 6, (uint8_t *)BMP3);
  HAL_Delay(1000);
}
