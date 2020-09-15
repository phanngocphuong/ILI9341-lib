#include "ili9341.h"

void ili9341_PinSetup(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /**
     * init pin RS
     * **/
    HAL_GPIO_WritePin(RS_Port,RS, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = RS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RS_Port, &GPIO_InitStruct);

    /**
     * init pin RES
     * **/
    HAL_GPIO_WritePin(RES_Port,RES, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = RES;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(RES_Port, &GPIO_InitStruct);

    /**
     * init pin CS
     * **/
    HAL_GPIO_WritePin(CS_Port,CS, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(CS_Port, &GPIO_InitStruct);

    /**
     * init pin LED
     * **/
    HAL_GPIO_WritePin(LED_Port,LED, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = LED;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_Port, &GPIO_InitStruct);

}

void ili9341_Init(void) {
    ili9341_PinSetup();
}
