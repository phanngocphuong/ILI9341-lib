#include "ili9341.h"
#include "spi.h"
#include "font.h"

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

void ILI9341_SPI_Send(unsigned char SPI_Data) {

    HAL_SPI_Transmit(&hspi1, &SPI_Data, 1, 1);
}

void ILI9341_Write_Command(uint8_t Command) {

    HAL_GPIO_WritePin(RS_Port, RS, GPIO_PIN_RESET);	
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);
    ILI9341_SPI_Send(Command);
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_SET);		
}

void ILI9341_Write_Data(uint8_t Data) {
    HAL_GPIO_WritePin(RS_Port, RS, GPIO_PIN_SET);	
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);
    ILI9341_SPI_Send(Data);	
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_SET);
}

void ILI9341_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2) {
    ILI9341_Write_Command(0x2A);
    ILI9341_Write_Data(X1>>8);
    ILI9341_Write_Data(X1);
    ILI9341_Write_Data(X2>>8);
    ILI9341_Write_Data(X2);

    ILI9341_Write_Command(0x2B);
    ILI9341_Write_Data(Y1>>8);
    ILI9341_Write_Data(Y1);
    ILI9341_Write_Data(Y2>>8);
    ILI9341_Write_Data(Y2);

    ILI9341_Write_Command(0x2C);
}

void ILI9341_Set_Rotation(uint8_t Rotation) {
	
    uint8_t screen_rotation = Rotation;

    ILI9341_Write_Command(0x36);
    HAL_Delay(1);
        
    switch(screen_rotation) 
        {
            case SCREEN_VERTICAL_1:
                ILI9341_Write_Data(0x40|0x08);
                LCD_WIDTH = 240;
                LCD_HEIGHT = 320;
                break;
            case SCREEN_HORIZONTAL_1:
                ILI9341_Write_Data(0x20|0x08);
                LCD_WIDTH  = 320;
                LCD_HEIGHT = 240;
                break;
            case SCREEN_VERTICAL_2:
                ILI9341_Write_Data(0x80|0x08);
                LCD_WIDTH  = 240;
                LCD_HEIGHT = 320;
                break;
            case SCREEN_HORIZONTAL_2:
                ILI9341_Write_Data(0x40|0x80|0x20|0x08);
                LCD_WIDTH  = 320;
                LCD_HEIGHT = 240;
                break;
            default:
                //EXIT IF SCREEN ROTATION NOT VALID!
                break;
        }
}

#define BURST_MAX_SIZE 500
void ILI9341_Draw_Colour_Burst(uint16_t Colour, uint32_t Size) {
    //SENDS COLOUR
    uint32_t Buffer_Size = 0;
    if((Size*2) < BURST_MAX_SIZE)
    {
        Buffer_Size = Size;
    }
    else
    {
        Buffer_Size = BURST_MAX_SIZE;
    }
        
    HAL_GPIO_WritePin(RS_Port, RS, GPIO_PIN_SET);	
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);

    unsigned char chifted = 	Colour>>8;;
    unsigned char burst_buffer[Buffer_Size];
    for(uint32_t j = 0; j < Buffer_Size; j+=2)
        {
            burst_buffer[j] = 	chifted;
            burst_buffer[j+1] = Colour;
        }

    uint32_t Sending_Size = Size*2;
    uint32_t Sending_in_Block = Sending_Size/Buffer_Size;
    uint32_t Remainder_from_block = Sending_Size%Buffer_Size;

    if(Sending_in_Block != 0)
    {
        for(uint32_t j = 0; j < (Sending_in_Block); j++)
            {
            HAL_SPI_Transmit(&hspi1, (unsigned char *)burst_buffer, Buffer_Size, 10);	
            }
    }

    //REMAINDER!
    HAL_SPI_Transmit(&hspi1, (unsigned char *)burst_buffer, Remainder_from_block, 10);	
        
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_SET);
}

void ILI9341_Fill_Screen(uint16_t Colour)
{
    ILI9341_Set_Address(0,0,LCD_WIDTH,LCD_HEIGHT);	
    ILI9341_Draw_Colour_Burst(Colour, LCD_WIDTH*LCD_HEIGHT);	
}

void ILI9341_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour) {
    if((X >=LCD_WIDTH) || (Y >=LCD_HEIGHT)) return;
    if((X+Width-1)>=LCD_WIDTH)
        {
            Width=LCD_WIDTH-X;
        }
    if((Y+Height-1)>=LCD_HEIGHT)
        {
            Height=LCD_HEIGHT-Y;
        }
    ILI9341_Set_Address(X, Y, X+Width-1, Y+Height-1);
    ILI9341_Draw_Colour_Burst(Colour, Height*Width);
}

void ILI9341_Draw_Pixel(uint16_t X,uint16_t Y,uint16_t Colour) {
    if((X >=LCD_WIDTH) || (Y >=LCD_HEIGHT)) return;	//OUT OF BOUNDS!
        
    //ADDRESS
    HAL_GPIO_WritePin(RS_Port, RS, GPIO_PIN_RESET);	
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);
    ILI9341_SPI_Send(0x2A);
    HAL_GPIO_WritePin(RS_Port, RS, GPIO_PIN_SET);	
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_SET);		

    //XDATA
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);	
    unsigned char Temp_Buffer[4] = {X>>8,X, (X+1)>>8, (X+1)};
    HAL_SPI_Transmit(&hspi1, Temp_Buffer, 4, 1);
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_SET);

    //ADDRESS
    HAL_GPIO_WritePin(RS_Port, RS, GPIO_PIN_RESET);	
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);	
    ILI9341_SPI_Send(0x2B);
    HAL_GPIO_WritePin(RS_Port, RS, GPIO_PIN_SET);			
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_SET);			

    //YDATA
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);
    unsigned char Temp_Buffer1[4] = {Y>>8,Y, (Y+1)>>8, (Y+1)};
    HAL_SPI_Transmit(&hspi1, Temp_Buffer1, 4, 1);
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_SET);

    //ADDRESS	
    HAL_GPIO_WritePin(RS_Port, RS, GPIO_PIN_RESET);	
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);	
    ILI9341_SPI_Send(0x2C);
    HAL_GPIO_WritePin(RS_Port, RS, GPIO_PIN_SET);			
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_SET);			

    //COLOUR	
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);
    unsigned char Temp_Buffer2[2] = {Colour>>8, Colour};
    HAL_SPI_Transmit(&hspi1, Temp_Buffer2, 2, 1);
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_SET);
	
}

void ILI9341_Draw_Char(char Character, uint8_t X, uint8_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour) 
{
	uint8_t 	function_char;
    uint8_t 	i,j;
		
	function_char = Character;
		
    if (function_char < ' ') {
        Character = 0;
    } 
    else {
        function_char -= 32;
	}
   	
    char temp[CHAR_WIDTH];
    for(uint8_t k = 0; k<CHAR_WIDTH; k++)
    {
    temp[k] = font[function_char][k];
    }
		
    // Draw pixels
    ILI9341_Draw_Rectangle(X, Y, CHAR_WIDTH*Size, CHAR_HEIGHT*Size, Background_Colour);

    for (j=0; j<CHAR_WIDTH; j++) {
        for (i=0; i<CHAR_HEIGHT; i++) {
            if (temp[j] & (1<<i)) {			
                if(Size == 1) {
                    ILI9341_Draw_Pixel(X+j, Y+i, Colour);
				}
                else {
                    ILI9341_Draw_Rectangle(X+(j*Size), Y+(i*Size), Size, Size, Colour);
                }
            }						
        }
    }
}

void ILI9341_Draw_Text(const char* Text, uint8_t X, uint8_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
    while (*Text) {
        ILI9341_Draw_Char(*Text++, X, Y, Colour, Size, Background_Colour);
        X += CHAR_WIDTH*Size;
    }
}

void ili9341_Init(void) {
    
    ili9341_PinSetup();
    HAL_GPIO_WritePin(LED_Port,LED, GPIO_PIN_SET);

    HAL_GPIO_WritePin(RES_Port, RES, GPIO_PIN_SET); // enable LCD Display
    SPI1_Init();
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);

    /**
     * hardware reset
     * **/
    HAL_GPIO_WritePin(RES_Port, RES, GPIO_PIN_RESET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(CS_Port, CS, GPIO_PIN_RESET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(RES_Port, RES, GPIO_PIN_SET);	

    //SOFTWARE RESET
    ILI9341_Write_Command(0x01);
    HAL_Delay(1000);
        
    //POWER CONTROL A
    ILI9341_Write_Command(0xCB);
    ILI9341_Write_Data(0x39);
    ILI9341_Write_Data(0x2C);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(0x34);
    ILI9341_Write_Data(0x02);

    //POWER CONTROL B
    ILI9341_Write_Command(0xCF);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(0xC1);
    ILI9341_Write_Data(0x30);

    //DRIVER TIMING CONTROL A
    ILI9341_Write_Command(0xE8);
    ILI9341_Write_Data(0x85);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(0x78);

    //DRIVER TIMING CONTROL B
    ILI9341_Write_Command(0xEA);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(0x00);

    //POWER ON SEQUENCE CONTROL
    ILI9341_Write_Command(0xED);
    ILI9341_Write_Data(0x64);
    ILI9341_Write_Data(0x03);
    ILI9341_Write_Data(0x12);
    ILI9341_Write_Data(0x81);

    //PUMP RATIO CONTROL
    ILI9341_Write_Command(0xF7);
    ILI9341_Write_Data(0x20);

    //POWER CONTROL,VRH[5:0]
    ILI9341_Write_Command(0xC0);
    ILI9341_Write_Data(0x23);

    //POWER CONTROL,SAP[2:0];BT[3:0]
    ILI9341_Write_Command(0xC1);
    ILI9341_Write_Data(0x10);

    //VCM CONTROL
    ILI9341_Write_Command(0xC5);
    ILI9341_Write_Data(0x3E);
    ILI9341_Write_Data(0x28);

    //VCM CONTROL 2
    ILI9341_Write_Command(0xC7);
    ILI9341_Write_Data(0x86);

    //MEMORY ACCESS CONTROL
    ILI9341_Write_Command(0x36);
    ILI9341_Write_Data(0x48);

    //PIXEL FORMAT
    ILI9341_Write_Command(0x3A);
    ILI9341_Write_Data(0x55);

    //FRAME RATIO CONTROL, STANDARD RGB COLOR
    ILI9341_Write_Command(0xB1);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(0x18);

    //DISPLAY FUNCTION CONTROL
    ILI9341_Write_Command(0xB6);
    ILI9341_Write_Data(0x08);
    ILI9341_Write_Data(0x82);
    ILI9341_Write_Data(0x27);

    //3GAMMA FUNCTION DISABLE
    ILI9341_Write_Command(0xF2);
    ILI9341_Write_Data(0x00);

    //GAMMA CURVE SELECTED
    ILI9341_Write_Command(0x26);
    ILI9341_Write_Data(0x01);

    //POSITIVE GAMMA CORRECTION
    ILI9341_Write_Command(0xE0);
    ILI9341_Write_Data(0x0F);
    ILI9341_Write_Data(0x31);
    ILI9341_Write_Data(0x2B);
    ILI9341_Write_Data(0x0C);
    ILI9341_Write_Data(0x0E);
    ILI9341_Write_Data(0x08);
    ILI9341_Write_Data(0x4E);
    ILI9341_Write_Data(0xF1);
    ILI9341_Write_Data(0x37);
    ILI9341_Write_Data(0x07);
    ILI9341_Write_Data(0x10);
    ILI9341_Write_Data(0x03);
    ILI9341_Write_Data(0x0E);
    ILI9341_Write_Data(0x09);
    ILI9341_Write_Data(0x00);

    //NEGATIVE GAMMA CORRECTION
    ILI9341_Write_Command(0xE1);
    ILI9341_Write_Data(0x00);
    ILI9341_Write_Data(0x0E);
    ILI9341_Write_Data(0x14);
    ILI9341_Write_Data(0x03);
    ILI9341_Write_Data(0x11);
    ILI9341_Write_Data(0x07);
    ILI9341_Write_Data(0x31);
    ILI9341_Write_Data(0xC1);
    ILI9341_Write_Data(0x48);
    ILI9341_Write_Data(0x08);
    ILI9341_Write_Data(0x0F);
    ILI9341_Write_Data(0x0C);
    ILI9341_Write_Data(0x31);
    ILI9341_Write_Data(0x36);
    ILI9341_Write_Data(0x0F);

    //EXIT SLEEP
    ILI9341_Write_Command(0x11);
    HAL_Delay(120);

    //TURN ON DISPLAY
    ILI9341_Write_Command(0x29);

    ILI9341_Set_Rotation(1);
    
    ILI9341_Fill_Screen(WHITE);

    ILI9341_Draw_Rectangle(10,10,50,50,BLACK);
    ILI9341_Draw_Rectangle(70,70,50,50,BLUE);
    ILI9341_Draw_Rectangle(00,70,50,50,GREEN);
    ILI9341_Draw_Rectangle(90,5,50,50,RED);
    ILI9341_Draw_Rectangle(200,100,50,50,YELLOW);
    ILI9341_Draw_Rectangle(130,120,50,50,PURPLE);
    ILI9341_Draw_Rectangle(310,10,10,10,PINK);

    ILI9341_Draw_Pixel(1,1,BLACK);
    ILI9341_Draw_Pixel(2,1,BLACK);
    ILI9341_Draw_Pixel(1,2,BLACK);
    ILI9341_Draw_Pixel(2,2,BLACK);

    ILI9341_Draw_Char('P',200,10,BLACK,3,WHITE);
    ILI9341_Draw_Text("phuong",160,30,BLACK,3,WHITE);
}
