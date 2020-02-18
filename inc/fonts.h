/**
 * @file   fonts.h
 * @brief  Font header file of SSD1306 driver module for STM32f10x and STM32F4xx.
 *
 * 		   The original version has been improved with various bug fixes and
 * 		   the code has been restructured paying attention to maximize the
 * 		   portability on resource-constrained devices. This SSD1306 LCD driver
 * 		   uses I2C for communication. Library functions allow to draw lines,
 * 		   rectangles and circles. It is also possible to draw text and single
 * 		   characters only.
 *
 *         <b>LIBRARY USAGE:</b>
 *         <ol>
 *         	 <li> Find the "driver settings" section in the ssd1306.h
 * 		          file and uncomment the correct define to make the library
 * 		          compatible with your target STM32 microcontroller. </li>
 * 		     <li> Before drawing on the screen, remember to call the
 * 		          @ref SSD1306_init initialization function passing a pointer
 * 		          to a valid i2c peripheral. </li>
 * 		   </ol>
 *
 * 		   Supported fonts:
 * 		   <ul>
 *  		 <li> 7x10 pixels. </li>
 *  		 <li> 11x18 pixels. </li>
 *  		 <li> 16x26 pixels. </li>
 *  	   </ul>
 *
 * @copyright
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Giovanni Scotti
 *
 * @attention
 * Original author: Tilen Majerle <tilen@majerle.eu>
 * Modification for STM32f10x: Alexander Lutsai <s.lyra@ya.ru>
 */

#ifndef __FONTS_H
#define __FONTS_H

/* C++ detection */
#ifdef __cplusplus
	extern C {
#endif

#include "stm32f1xx_hal.h"
#include "string.h"


/**
 * @brief Structure storing font information.
 */
typedef struct {
	uint8_t fontWidth;    /*!< Font width in pixels. */
	uint8_t fontHeight;   /*!< Font height in pixels. */
	const uint16_t *data; /*!< Pointer to font data array. */
} FontDef_t;

/** 
 * @brief Structure used in conjunction with @ref get_string_size. It stores
 *        information about the length and height of the given string.
 */
typedef struct {
	uint16_t length; /*!< String length in units of pixels */
	uint16_t height; /*!< String height in units of pixels */
} FontStringSize_t;


/**
 * @brief 7x10 pixels font size structure.
 */
extern FontDef_t FontDef_7x10;

/**
 * @brief 11x18 pixels font size structure.
 */
extern FontDef_t FontDef_11x18;

/**
 * @brief 16x26 pixels font size structure.
 */
extern FontDef_t FontDef_16x26;


/**
 * @brief      Calculates the length and height in units of pixels of the given
 *             string depending on the used font.
 *
 * @param[in]  *str: string to be checked for length and height.
 * @param[out] *sizeStruct: pointer to empty @ref FontStringSize_t structure
 *             where string information are stored.
 * @param[in]  *font: pointer to @ref FontDef_t font used for calculations.
 */
void get_string_size(char *str, FontStringSize_t *sizeStruct, FontDef_t *font);

/* C++ detection */
#ifdef __cplusplus
	}
#endif

 
#endif // __FONT_H
