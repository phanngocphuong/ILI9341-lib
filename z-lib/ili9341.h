#ifndef _ILI_9341_
#define _ILI_9341_

    #include "main.h"

    #define BLACK       0x0000      
    #define NAVY        0x000F      
    #define DARKGREEN   0x03E0      
    #define DARKCYAN    0x03EF      
    #define MAROON      0x7800      
    #define PURPLE      0x780F      
    #define OLIVE       0x7BE0      
    #define LIGHTGREY   0xC618      
    #define DARKGREY    0x7BEF      
    #define BLUE        0x001F      
    #define GREEN       0x07E0      
    #define CYAN        0x07FF      
    #define RED         0xF800     
    #define MAGENTA     0xF81F      
    #define YELLOW      0xFFE0      
    #define WHITE       0xFFFF      
    #define ORANGE      0xFD20      
    #define GREENYELLOW 0xAFE5     
    #define PINK        0xF81F

    #define SCREEN_VERTICAL_1		0
    #define SCREEN_HORIZONTAL_1		1
    #define SCREEN_VERTICAL_2		2
    #define SCREEN_HORIZONTAL_2		3

    #define RS GPIO_PIN_2
    #define RS_Port GPIOB

    #define RES GPIO_PIN_10
    #define RES_Port GPIOB

    #define CS GPIO_PIN_11
    #define CS_Port GPIOB

    #define LED GPIO_PIN_4
    #define LED_Port GPIOA

    uint16_t LCD_WIDTH ;
    uint16_t LCD_HEIGHT;

    void ili9341_PinSetup(void);
    void ili9341_Init(void);

#endif