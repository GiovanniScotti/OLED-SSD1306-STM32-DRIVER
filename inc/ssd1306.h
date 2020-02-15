/**
 * @file   ssd1306.h
 * @author Giovanni Scotti
 * @brief  Header file of SSD1306 driver module for STM32f10x and STM32F4xx.
 * 		   The original version has been improved with various bug fixes and
 * 		   restructured code paying attention to resource-constrained devices.
 *
 * @attention
 * Original author: Tilen Majerle <tilen@majerle.eu>
 * Modification for STM32f10x: Alexander Lutsai <s.lyra@ya.ru>
 *
 * ----------------------------------------------------------------------
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 */

#ifndef __SSD1306_H
#define __SSD1306_H

/* C++ detection */
#ifdef __cplusplus
	extern C {
#endif

/**
 * This SSD1306 LCD uses I2C for communication.
 * Library functions allow you to draw lines, rectangles and circles.
 * It is also possible to draw text and characters.
 *
 * LIBRARY USAGE:
 *  1. Call the SSD1306_Init initialization function passing a pointer to a
 *     valid i2c peripheral.
 *  2. TODO
 */

///////////////////////////////////////////////////////////////////////////////
// DRIVER SETTINGS.
///////////////////////////////////////////////////////////////////////////////

#define STM32_FLAVOUR STM32F1XX
//#define STM32_FLAVOUR STM32F4XX

#define SSD1306_I2C_ADDR    0x78
//#define SSD1306_I2C_ADDR  0x7A

#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 64

///////////////////////////////////////////////////////////////////////////////


#if STM32_FLAVOUR == STM32F1XX
	#include "stm32f1xx_hal.h"
#elif STM32_FLAVOUR == STM32F4XX
	#include "stm32f4xx_hal.h"
#endif

#include "fonts.h"
#include "stdlib.h"
#include "string.h"


#define SSD1306_I2C_TIMEOUT	                         2000

// Command defines.
#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define SSD1306_DEACTIVATE_SCROLL                    0x2E // Stop scroll.
#define SSD1306_ACTIVATE_SCROLL                      0x2F // Start scroll.
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 // Set scroll range.

#define SSD1306_NORMALDISPLAY 						 0xA6
#define SSD1306_INVERTDISPLAY 						 0xA7


/**
 * @brief Initialization status enumeration.
 */
typedef enum {
	I2C_ERROR = 0x00, /*!< Error while trying to communicate with the LCD. */
	I2C_OK = 0x01     /*!< LCD initialization done. */
} SSD1306_status_t;


/**
 * @brief SSD1306 color enumeration.
 */
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel. */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on the LCD. */
} SSD1306_color_t;


/**
 * @brief Structure to store information about the LCD status.
 */
typedef struct {
	uint16_t currentX;
	uint16_t currentY;
	uint8_t  inverted;
	uint8_t  initialized;
} SSD1306_t;


///////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES.
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief     Initializes the SSD1306 I2C LCD.
 * @param[in] A pointer to the i2c peripheral used to communicate with the LCD.
 * @retval    Initialization status:
 *             - I2C_ERROR: LCD was not detected on I2C port.
 *             - I2C_OK: LCD is working and ready to use.
 */
SSD1306_status_t SSD1306_init(I2C_HandleTypeDef *i2c_ptr);


/**
 * @brief Clears the display.
 */
void SSD1306_clear(void);


/**
 * @brief     Inverts the display.
 * @param[in] is_inverted: when equal to 0 the display is in normal state,
 *            otherwise it is inverted.
 */
void SSD1306_invertDisplay(uint8_t is_inverted);


/** 
 * @brief  Updates buffer from internal RAM to LCD.
 * @note   This function must be called each time you do some changes
 *         to the LCD.
 */
void SSD1306_updateScreen(void);


/**
 * @brief  Toggles pixels inversion inside the internal RAM.
 * @note   @ref SSD1306_updateScreen() must be called after that in order to
 *         see updates on the LCD screen.
 */
void SSD1306_toggleInvert(void);


// scroll the screen for fixed rows
void SSD1306_scrollRight(uint8_t start_row, uint8_t end_row);
void SSD1306_scrollLeft(uint8_t start_row, uint8_t end_row);
void SSD1306_scrollDiagRight(uint8_t start_row, uint8_t end_row);
void SSD1306_scrollDiagLeft(uint8_t start_row, uint8_t end_row);
void SSD1306_stopScroll(void);


/** 
 * @brief     Fills the entire LCD with desired color.
 * @note      @ref SSD1306_updateScreen() must be called after that in order to
 *            see updates to the LCD screen.
 * @param[in] color: color to be used to fill the screen. This parameter is a
 * 			  value of @ref SSD1306_color_t enumeration.
 */
void SSD1306_fill(SSD1306_color_t color);


/**
 * @brief     Draws pixel at desired location.
 * @note      @ref SSD1306_updateScreen() must called after that in order to
 *            see updates to the LCD screen.
 * @param[in] x: X pixel location. This parameter can be a value between 0 and
 * 			  SSD1306_WIDTH-1.
 * @param[in] y: Y pixel location. This parameter can be a value between 0 and
 * 			  SSD1306_HEIGHT-1.
 * @param[in] color: color to be used for filling the screen. This parameter
 * 			  can be a value of @ref SSD1306_color_t enumeration.
 */
void SSD1306_drawPixel(uint16_t x, uint16_t y, SSD1306_color_t color);


/**
 * @brief    Draws bitmap.
 * @param[i]  X:  X location to start the Drawing
 * @param[i]  Y:  Y location to start the Drawing
 * @param[i]  *bitmap : Pointer to the bitmap
 * @param[i]  W : width of the image
 * @param[i]  H : Height of the image
 * @param[i]  color : 1-> white/blue, 0-> black
 */
void SSD1306_drawBitmap(int16_t x, int16_t y, const unsigned char* bitmap,
	int16_t w, int16_t h, SSD1306_color_t color);


/**
 * @brief     Sets cursor pointer to desired location.
 * @param[in] x: X location. This parameter can be a value between
 *            0 and SSD1306_WIDTH-1.
 * @param[in] y: Y location. This parameter can be a value between
 *            0 and SSD1306_HEIGHT-1.
 */
void SSD1306_gotoXY(uint16_t x, uint16_t y);


/**
 * @brief     Puts character into internal RAM.
 * @note      @ref SSD1306_updateScreen() must be called after that in order
 *            to see updates on the LCD screen.
 * @param[in] ch: the character to be written.
 * @param[in] *font: pointer to @ref FontDef_t structure with the used font.
 * @param[in] color: color used for drawing. This parameter can be a value of
 *            @ref SSD1306_COLOR_t enumeration.
 * @retval    The written character.
 */
char SSD1306_putc(char ch, FontDef_t* font, SSD1306_color_t color);


/**
 * @brief     Puts string into internal RAM.
 * @note      @ref SSD1306_updateScreen() must be called after that in order to
 * 			  see updates on the LCD screen.
 * @param[in] *str: string to be written.
 * @param[in] *font: pointer to @ref FontDef_t structure with the used font.
 * @param[in] color: color used for drawing. This parameter can be a value of
 *            @ref SSD1306_color_t enumeration.
 */
void SSD1306_puts(char* str, FontDef_t* font, SSD1306_color_t color);




/**
 * @brief  Draws line on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x0: Line X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y0: Line Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x1: Line X end point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: Line Y end point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
//void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c);

/**
 * @brief  Draws rectangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
//void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/**
 * @brief  Draws filled rectangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
//void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/**
 * @brief  Draws triangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x1: First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x2: Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y2: Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x3: Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y3: Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
//void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);

/**
 * @brief  Draws circle to STM buffer
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
//void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

/**
 * @brief  Draws filled circle to STM buffer
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
//void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);


/**
 * @brief     Writes a single byte to the i2c slave.
 * @param[in] addr: 7 bit slave address, left aligned, bits 7:1 are used.
 * @param[in] reg: register to write to.
 * @param[in] data: pointer to the array of bytes to be written.
 */
void SSD1306_I2C_Write(uint8_t addr, uint8_t reg, uint8_t data);


/**
 * @brief     Writes a single byte to the i2c slave.
 * @param[in] addr: 7 bit slave address, left aligned, bits 7:1 are used.
 * @param[in] reg: register to write to.
 * @param[in] data: pointer to the array of bytes to be written.
 * @param[in] counter: how many bits will be written.
 */
void SSD1306_I2C_WriteMulti(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t count);


/* C++ detection */
#ifdef __cplusplus
	}
#endif

#endif // __SSD1306_H
