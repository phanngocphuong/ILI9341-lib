#ifndef _ILI_9341_
#define _ILI_9341_

    #include "main.h"

    #define RS GPIO_PIN_2
    #define RS_Port GPIOB

    #define RES GPIO_PIN_10
    #define RES_Port GPIOB

    #define CS GPIO_PIN_11
    #define CS_Port GPIOB

    #define LED GPIO_PIN_4
    #define LED_Port GPIOA

    void ili9341_PinSetup(void);
    void ili9341_Init(void);

#endif