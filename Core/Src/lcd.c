#include "main.h"
#include "stm32f3xx_hal.h"
#include "gpio.h"
#include "tim.h"
#include "lcd.h"



// Configure data pins based on the provided data byte
static inline void LCD_SetDataPins(uint8_t data) {
    GPIO_TypeDef* ports[] = {D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port};
    uint16_t pins[] = {D4_Pin, D5_Pin, D6_Pin, D7_Pin};

    for (int i = 0; i < sizeof(ports) / sizeof(ports[0]); i++) {
        if (data & (1 << i)) {
            HAL_GPIO_WritePin(ports[i], pins[i], GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(ports[i], pins[i], GPIO_PIN_RESET);
        }
    }
}


// Read the status of data pins and return the resulting byte
static inline uint8_t LCD_ReadDataPins() {
    uint8_t result = 0;

    GPIO_TypeDef* ports[] = {D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port};
    uint16_t pins[] = {D4_Pin, D5_Pin, D6_Pin, D7_Pin};

    for (int i = 0; i < sizeof(ports) / sizeof(ports[0]); i++) {
        if (HAL_GPIO_ReadPin(ports[i], pins[i]) == GPIO_PIN_SET) {
            result |= (1 << i);
        }
    }

    return result;
}


// Set display in input mode
static void LCD_SetDataInputMode() {
	 GPIO_InitTypeDef GPIO_InitStruct;
	 GPIO_InitStruct.Pin = D4_Pin;
	 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 HAL_GPIO_Init(D4_GPIO_Port, &GPIO_InitStruct);
	 
	 GPIO_InitStruct.Pin = D5_Pin;
	 HAL_GPIO_Init(D5_GPIO_Port, &GPIO_InitStruct);
	 
	 GPIO_InitStruct.Pin = D6_Pin;
	 HAL_GPIO_Init(D6_GPIO_Port, &GPIO_InitStruct);
	 	 
	 GPIO_InitStruct.Pin = D7_Pin;
	 HAL_GPIO_Init(D7_GPIO_Port, &GPIO_InitStruct);
}


// Set display in output mode
static void LCD_SetDataOutputMode() {
	  GPIO_InitTypeDef GPIO_InitStruct;
	  GPIO_InitStruct.Pin = D4_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(D4_GPIO_Port, &GPIO_InitStruct);
	  
	  GPIO_InitStruct.Pin = D5_Pin;
	  HAL_GPIO_Init(D5_GPIO_Port, &GPIO_InitStruct);
	  
	  GPIO_InitStruct.Pin = D6_Pin;
	  HAL_GPIO_Init(D6_GPIO_Port, &GPIO_InitStruct);
	  	 
	  GPIO_InitStruct.Pin = D7_Pin;
	  HAL_GPIO_Init(D7_GPIO_Port, &GPIO_InitStruct);
}


// Read first 4 bites from LCD, then another 4 bites
uint8_t LCD_ReadByte(void) {
	uint8_t result = 0;
	LCD_SetDataInputMode();

	LCD_RW_SET;
	LCD_E_SET;
	result = (LCD_ReadDataPins() << 4);
	LCD_E_RESET;

	LCD_E_SET;
	result |= LCD_ReadDataPins();
	LCD_E_RESET;
	return result;
}


// Write first 4 bites to LCD, then another 4 bites, wait until display is ready
void LCD_WriteByte(uint8_t data) {
	LCD_SetDataOutputMode();

	LCD_RW_RESET;
	LCD_E_SET;
	LCD_SetDataPins(data >> 4);
	LCD_E_RESET;

	LCD_E_SET;
	LCD_SetDataPins(data);
	LCD_E_RESET;

	while((LCD_CheckBusyFlag() & (1<<7)));
}


// Check display busy flag (RS)
uint8_t LCD_CheckBusyFlag() {
	LCD_RS_RESET;
	return LCD_ReadByte();
}


// Write data into display
void LCD_WriteData(uint8_t data) {
	LCD_RS_SET;
	LCD_WriteByte(data);
}


// Write command into display
void LCD_WriteCommand(uint8_t cmd) {
	LCD_RS_RESET;
	LCD_WriteByte(cmd);
}


//	Print character on display
void LCD_Char(char c) {
	// If the character is in the range of special characters (0x80 - 0x87),
	// keep only the last 3 bits; otherwise, keep the character unchanged
	LCD_WriteData(((c >= 0x80) && (c <= 0x87)) ? (c & 0x07) : c);
}


//	Print string on display
void LCD_String(char* str) {
	char c;
	while((c = *(str++)))
		LCD_Char(c);
}

// Print integer on display
void LCD_Int(int value) {
	char buf[LCD_X+1];
	sprintf(buf, "%d", value);
	LCD_String(buf);
}

// Print hexadecimal on display
void LCD_Hex(int value, uint8_t upper_case) {
	char buf[LCD_X+1];
	if(upper_case) {
		sprintf(buf, "%X", value);
	} else {
		sprintf(buf, "%x", value);
	}
	LCD_String(buf);
}

// Print custom char on display
void LCD_CustomChar(uint8_t number, uint8_t *def_char) {
	uint8_t i, c;
	LCD_WriteCommand(64+((number&0x7)*8));
	for(i = 0; i < 8; i++) {
		c = *(def_char++);
		LCD_WriteData(c);
	}
}


// Set cursor to home position
void LCD_Home(){
	LCD_WriteCommand(LCD_CLR|LCD_HOME);
}


// Control cursor visibility
void LCD_Cursor(uint8_t on_off)
{
	if(on_off == 0)
		LCD_WriteCommand(LCD_ONOFF|LCD_DISPLAYON);
	else
		LCD_WriteCommand(LCD_ONOFF|LCD_DISPLAYON|LCD_CURSORON);
}

//
// Control cursor blinking
void LCD_Blink(uint8_t on_off) {
	if(on_off == 0)
		LCD_WriteCommand(LCD_ONOFF|LCD_DISPLAYON);
	else
		LCD_WriteCommand(LCD_ONOFF|LCD_DISPLAYON|LCD_CURSORON|LCD_BLINKON);
}

//
// Set cursor on display: x-row, y-column
void LCD_SetLocation(uint8_t x, uint8_t y) {
    uint8_t line_offset[] = {LCD_LINE1, LCD_LINE2};

    if (y < sizeof(line_offset) / sizeof(line_offset[0])) {
        uint8_t target_line = line_offset[y];
        LCD_WriteCommand(0x80 + target_line + x);
    }
}


// Clear LCD display
void LCD_Clear(void) {
	LCD_WriteCommand(LCD_CLR);
}


// Delay in us for display
void LCD_Delay_us(uint16_t us){
	htim3.Instance->CNT = 0;
	while(htim3.Instance->CNT <= us);
}


//	Initialize display
void LCD_Init(void) {

	// reset data pins
	LCD_RS_RESET;
	LCD_E_RESET;
	LCD_RW_RESET;

	//set display into output mode and wait
	LCD_SetDataOutputMode();
	HAL_Delay(15);

	// send initialization data in 8-bit mode
	LCD_SetDataPins(0x03);
	Delay_us(4100);

	LCD_SetDataPins(0x03);
	LCD_Delay_us(100);

	LCD_SetDataPins(0x03);
	LCD_Delay_us(100);

	// set display into 4-bit mode
	LCD_SetDataPins(0x02);
	LCD_Delay_us(100);

	// Set display parameters
	LCD_WriteCommand(LCD_FUNC|LCD_FUNC4BIT|LCD_FUNC2LINE|LCD_FUNC5x7);
	LCD_WriteCommand(LCD_ONOFF|LCD_CURSOROFF);
	LCD_WriteCommand(LCD_ONOFF|LCD_DISPLAYON);
	LCD_WriteCommand(LCD_ENTRY|LCD_ENTRYRIGHT);

	// Clear display
	LCD_Clear();
}
