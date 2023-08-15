#ifndef LCD_H_
#define LCD_H_


#define LCD_Y 						2
#define LCD_X 						16
#define LCD_LINE1 					0x00
#define LCD_LINE2 					0x40
#define LCD_CLR						0x01
#define LCD_HOME					0x02
#define LCD_MOVE					0x01
#define LCD_ENTRY					0x04
#define LCD_ENTRYRIGHT				0x02
#define LCD_ENTRYLEFT				0x00
#define LCD_ONOFF					0x08
#define LCD_DISPLAYON				0x04
#define LCD_CURSORON				0x02
#define LCD_CURSOROFF				0x00
#define LCD_BLINKON					0x01
#define LCDC_BLINKOFF				0x00
#define LCD_SHIFT					0x10
#define LCD_SHIFTDISP				0x08
#define LCD_SHIFTRIGHT				0x04
#define LCD_SHIFTLEFT				0x00
#define LCD_FUNC					0x20
#define LCD_FUNC8BIT				0x10
#define LCD_FUNC4BIT				0x00
#define LCD_FUNC2LINE				0x08
#define LCD_FUNC1LINE				0x00
#define LCD_FUNC5x10				0x04
#define LCD_FUNC5x7					0x00
#define LCD_SET_CGRAM				0x40
#define LCD_SET_DDRAM				0x80



#define LCD_E_SET HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET)
#define LCD_E_RESET HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET)
#define LCD_RS_SET HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET)
#define LCD_RS_RESET HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET)
#define LCD_RW_SET HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_SET)
#define LCD_RW_RESET HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET)



void LCD_Home();
void LCD_Delay_us(uint16_t us);
void LCD_Clear(void);
void LCD_Init(void);
void LCD_SetLocation(uint8_t x, uint8_t y);
void LCD_Blink(uint8_t on_off);
void LCD_Cursor(uint8_t on_off);
void LCD_CustomChar(uint8_t number, uint8_t *def_char);
void LCD_Char(char c);
void LCD_String(char* str);
void LCD_Int(int value);
void LCD_Hex(int value, uint8_t upper_case);

void LCD_WriteCommand(uint8_t cmd);
void LCD_WriteData(uint8_t data);
uint8_t LCD_CheckBusyFlag();
void LCD_WriteByte(uint8_t data);
uint8_t LCD_ReadByte(void);


#endif /* LCD_H_ */
