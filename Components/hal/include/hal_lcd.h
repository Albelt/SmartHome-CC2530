/*********************************************************
 * HAL_LCD.H
 * 中景园1.8寸TFT屏驱动，主控芯片为ST7735S，分辨率为128*160，采用4线SPI总线协议(软件模拟SPI时序)
 * 加上VCC和GND一共6根线，直接按顺序连接到板载LCD接口上即可
 * 作者：物联网1701文诗波
 * 邮箱：albeltbo@foxmail.com
 * 时间：2020.07.18
******************************************************/

/*  占用4个GPIO口
LCD_DC                 P0_0  // 数据/命令选择
LCD_SDA                P1_3  // 双向数据
LCD_SCL                P1_2  // 时钟
LCD_RST                P1_7  // 控制lcd复位   
*/


#ifndef HAL_LCD_H
#define HAL_LCD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "hal_board.h"


/* These are used to specify which line the text will be printed */
#define HAL_LCD_LINE_1      0x00
#define HAL_LCD_LINE_2      0x01
/*
   This to support LCD with extended number of lines (more than 2).
   Don't use these if LCD doesn't support more than 2 lines
*/
#define HAL_LCD_LINE_3      0x02
#define HAL_LCD_LINE_4      0x03
#define HAL_LCD_LINE_5      0x04
#define HAL_LCD_LINE_6      0x05
#define HAL_LCD_LINE_7      0x06
#define HAL_LCD_LINE_8      0x07

/* Max number of chars on a single LCD line */
#define HAL_LCD_MAX_CHARS   16
#define HAL_LCD_MAX_BUFF    25

extern void HalLcdInit(void);

/*
 * Write a string to the LCD
 */
extern void HalLcdWriteString ( char *str, uint8 option);

/*
 * Write a value to the LCD
 */
extern void HalLcdWriteValue ( uint32 value, const uint8 radix, uint8 option);

/*
 * Write a value to the LCD
 */
extern void HalLcdWriteScreen( char *line1, char *line2 );

/*
 * Write a string followed by a value to the LCD
 */
extern void HalLcdWriteStringValue( char *title, uint16 value, uint8 format, uint8 line );

/*
 * Write a string followed by 2 values to the LCD
 */
extern void HalLcdWriteStringValueValue( char *title, uint16 value1, uint8 format1, uint16 value2, uint8 format2, uint8 line );

/*
 * Write a percentage bar to the LCD
 */
extern void HalLcdDisplayPercentBar( char *title, uint8 value );

//------------------------------------------------------------------------------
//LCD 颜色
#define  WHITE          0xFFFF
#define  BLACK          0x0000
#define  GREY           0xF7DE
#define  GREY2          0xF79E
#define  DARK_GREY      0x6B4D
#define  DARK_GREY2     0x52AA
#define  LIGHT_GREY     0xE71C
#define  BLUE           0x001F
#define  BLUE2          0x051F
#define  RED            0xF800
#define  MAGENTA        0xF81F
#define  GREEN          0x07E0
#define  CYAN           0x7FFF
#define  YELLOW         0xFFE0


//显示界面颜色
#define WINDOW_BK_COLOR 0xDFFF //窗口背景色
#define WINDOW_COLOR    0x11FA //窗口前景色
#define TITLE_BK_COLOR  0x11FA //标题栏背景色
#define TITLE_COLOR     0xDFFF //标题栏前景色
#define STATUS_BK_COLOR 0x0014 //状态栏背景色
#define STATUS_COLOR    0xDFFF //状态栏前景色

extern unsigned int  Color ;   // 前景颜色
extern unsigned int  Color_BK; // 背景颜色

extern void HalLcdWriteEnString(uint8 x, uint8 y, char *title);
extern void LCD_write_EN_string(unsigned char X,unsigned char Y,uint8 *s);
extern void LCD_write_CN_string(unsigned char X,unsigned char Y,uint8 *s);
extern void DrawRectFill(unsigned int Xpos  , unsigned int Ypos, unsigned int Width, 
                  unsigned int Height, unsigned Color);

#if defined (COORDINATOR)
   extern void ShowLogo();
#endif


#ifdef __cplusplus
}
#endif

#endif
