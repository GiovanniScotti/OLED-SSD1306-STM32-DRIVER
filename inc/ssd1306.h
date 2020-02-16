/**
 * @file   ssd1306.h
 * @author Giovanni Scotti
 * @brief  Header file of SSD1306 driver module for STM32f10x and STM32F4xx.
 * 		   The original version has been improved with various bug fixes and
 * 		   the code has been restructured paying attention to maximize the
 * 		   portability on resource-constrained devices.
 * 		   This SSD1306 LCD driver uses I2C for communication. Library functions
 * 		   allow to draw lines, rectangles and circles. It is also possible
 * 		   to draw text and single characters only.
 *
 * @brief  LIBRARY USAGE:
 * 		   First of all, find the "driver settings" section in the ssd1306.h
 * 		   file and uncomment the correct define to make the library compatible
 * 		   with your target STM32 microcontroller.
 *  	   Before drawing on the screen, remember to call the @ref SSD1306_init
 *         initialization function passing a pointer to a valid i2c peripheral.
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
#else
	#error Invalid STM32 flavour
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

// Size in bytes of the temporary data storage to communicate with the i2c LCD.
#define SSD1306_I2C_DATATMP_SIZE					 256


/**
 * @brief Initialization status enumeration.
 */
typedef enum {
	SSD1306_OK = 0x00,     /*!< LCD command success. */
	I2C_ERROR = 0x01,      /*!< Error while trying to communicate with the LCD. */
	INVALID_PARAMS = 0x02, /*!< Invalid arguments. */
	NO_INIT = 0x03         /*!< Device has not been initialized before use. */
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
	I2C_HandleTypeDef *i2c_ptr;    /*!< Pointer to the i2c HAL data structure. */
	uint16_t          currentX;    /*!< Current X position of the cursor. */
	uint16_t          currentY;    /*!< Current Y position of the cursor. */
	uint8_t           inverted;    /*!< Display color is inverted. */
	uint8_t           initialized; /*!< Display initialization flag. */
} SSD1306_t;


///////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES.
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief     Initializes the SSD1306 I2C LCD.
 * @param[in] i2c_ptr: a pointer to the i2c peripheral used to communicate
 *            with the LCD.
 * @retval    SSD1306 LCD status.
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
 * @brief Updates buffer from internal RAM to LCD.
 * @note  This function must be called each time you do some changes
 *        to the LCD.
 */
void SSD1306_updateScreen(void);


/**
 * @brief Toggles pixels inversion inside the internal RAM.
 * @note  @ref SSD1306_updateScreen() must be called after that in order to
 *        see updates on the LCD screen.
 */
void SSD1306_toggleInvert(void);


// TODO
// scroll the screen for fixed rows
void SSD1306_scrollRight(uint8_t start_row, uint8_t end_row);
void SSD1306_scrollLeft(uint8_t start_row, uint8_t end_row);
void SSD1306_scrollDiagRight(uint8_t start_row, uint8_t end_row);
void SSD1306_scrollDiagLeft(uint8_t start_row, uint8_t end_row);
void SSD1306_stopScroll(void);


/** 
 * @brief     Fills the entire LCD with desired color.
 * @note      @ref SSD1306_updateScreen() must be called after that in order to
 *            see updates on the LCD screen.
 * @param[in] color: color to be used to fill the screen. This parameter is a
 * 			  value of @ref SSD1306_color_t enumeration.
 */
void SSD1306_fill(SSD1306_color_t color);


/**
 * @brief     Draws pixel at desired location.
 * @note      @ref SSD1306_updateScreen() must called after that in order to
 *            see updates on the LCD screen.
 * @param[in] x: X pixel location. This parameter can be a value between 0 and
 * 			  SSD1306_WIDTH-1.
 * @param[in] y: Y pixel location. This parameter can be a value between 0 and
 * 			  SSD1306_HEIGHT-1.
 * @param[in] color: color to be used for filling the screen. This parameter
 * 			  can be a value of @ref SSD1306_color_t enumeration.
 */
void SSD1306_drawPixel(uint16_t x, uint16_t y, SSD1306_color_t color);


/**
 * @brief     Draws bitmap.
 * @param[in] x: X location to start the Drawing
 * @param[in] y: Y location to start the Drawing
 * @param[in] *bitmap : Pointer to the bitmap
 * @param[in] w: width of the image
 * @param[in] h: height of the image
 * @param[in] color: 1-> white/blue, 0-> black
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
 *            @ref SSD1306_color_t enumeration.
 */
void SSD1306_putc(char ch, FontDef_t* font, SSD1306_color_t color);


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
 * @brief     Draws a segment on the LCD given the coordinates of its two
 *            end points.
 * @note      @ref SSD1306_updateScreen() must be called after that in order to
 *            see updates on the LCD screen.
 * @param[in] x0: line X start point. Valid input is 0 to SSD1306_WIDTH-1.
 * @param[in] y0: line Y start point. Valid input is 0 to SSD1306_HEIGHT-1.
 * @param[in] x1: line X end point. Valid input is 0 to SSD1306_WIDTH-1.
 * @param[in] y1: line Y end point. Valid input is 0 to SSD1306_HEIGHT-1.
 * @param[in] color: color to be used. This parameter can be a value of
 *            @ref SSD1306_color_t enumeration.
 */
void SSD1306_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
	SSD1306_color_t color);


/**
 * @brief     Draws rectangle on the LCD.
 * @note      @ref SSD1306_updateScreen() must be called after that in order to
 *            see updates on the LCD screen.
 * @param[in] x: top left X start point. Valid input is 0 to SSD1306_WIDTH-1.
 * @param[in] y: top left Y start point. Valid input is 0 to SSD1306_HEIGHT-1.
 * @param[in] w: Rectangle width in units of pixels.
 * @param[in] h: Rectangle height in units of pixels.
 * @param[in] color: color to be used. This parameter can be a value of
 *            @ref SSD1306_color_t enumeration.
 */
void SSD1306_drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
	SSD1306_color_t color);


/**
 * @brief     Draws filled rectangle on LCD.
 * @note      @ref SSD1306_updateScreen() must be called after that in order to
 *            see updates on the LCD screen.
 * @param[in] x: top left X start point. Valid input is 0 to SSD1306_WIDTH-1.
 * @param[in] y: top left Y start point. Valid input is 0 to SSD1306_HEIGHT-1.
 * @param[in] w: rectangle width in units of pixels.
 * @param[in] h: rectangle height in units of pixels.
 * @param[in] color: color to be used. This parameter can be a value of
 *            @ref SSD1306_color_t enumeration.
 */
void SSD1306_drawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
	SSD1306_color_t color);


/**
 * @brief     Draws triangle on LCD.
 * @note      @ref SSD1306_updateScreen() must be called after that in order to
 *            see updates on the LCD screen.
 * @param[in] x1: first vertex X location. Valid input is 0 to SSD1306_WIDTH-1.
 * @param[in] y1: first vertex Y location. Valid input is 0 to SSD1306_HEIGHT-1.
 * @param[in] x2: second vertex X location. Valid input is 0 to SSD1306_WIDTH-1.
 * @param[in] y2: second vertex Y location. Valid input is 0 to SSD1306_HEIGHT-1.
 * @param[in] x3: third vertex X location. Valid input is 0 to SSD1306_WIDTH-1.
 * @param[in] y3: third vertex Y location. Valid input is 0 to SSD1306_HEIGHT-1.
 * @param[in] color: color to be used. This parameter can be a value of
 * 			  @ref SSD1306_color_t enumeration.
 */
void SSD1306_drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
	uint16_t x3, uint16_t y3, SSD1306_color_t color);


/**
 * @brief     Draws circle on LCD.
 * @note      @ref SSD1306_updateScreen() must be called after that in order to
 *            see updates on the LCD screen.
 * @param[in] x0: X location of the center of the circle. Valid input is
 *            0 to SSD1306_WIDTH-1.
 * @param[in] y0: Y location of the center of the circle. Valid input is
 *            0 to SSD1306_HEIGHT-1.
 * @param[in] r: circle radius in pixels.
 * @param[in] color: color to be used. This parameter can be a value of
 *            @ref SSD1306_color_t enumeration.
 */
void SSD1306_drawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_color_t color);


/**
 * @brief     Draws filled circle on LCD.
 * @note      @ref SSD1306_updateScreen() must be called after that in order to
 *            see updates on the LCD screen.
 * @param[in] x0: X location of the center of the circle. Valid input is
 *            0 to SSD1306_WIDTH-1.
 * @param[in] y0: Y location of the center of the circle. Valid input is
 *            0 to SSD1306_HEIGHT-1.
 * @param[in] r: circle radius in pixels.
 * @param[in] color: color to be used. This parameter can be a value of
 *            @ref SSD1306_color_t enumeration.
 */
void SSD1306_drawFilledCircle(int16_t x0, int16_t y0, int16_t r,
	SSD1306_color_t color);


/**
 * @brief Turns the LCD on.
 */
void SSD1306_lcdOn(void);


/**
 * @brief Turns the LCD off.
 */
void SSD1306_lcdOff(void);


///////////////////////////////////////////////////////////////////////////////
// I2C COMMUNICATION FUNCTIONS.
///////////////////////////////////////////////////////////////////////////////

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
 * @param[in] count: how many bits will be written.
 */
void SSD1306_I2C_WriteMulti(uint8_t addr, uint8_t reg, uint8_t *data, uint16_t count);


/* C++ detection */
#ifdef __cplusplus
	}
#endif

#endif // __SSD1306_H
