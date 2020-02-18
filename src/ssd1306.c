/**
 * @file   ssd1306.c
 * @brief  SSD1306 driver module for STM32f10x and STM32F4xx.
 *
 * 		   The original version has been improved with various bug fixes and
 * 		   code has been restructured paying attention to maximize the
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

#include "ssd1306.h"


/* Write command macro. */
#define SSD1306_WRITECOMMAND(command) SSD1306_I2C_Write(SSD1306_I2C_ADDR, 0x00, (command))

/* Write data macro. */
#define SSD1306_WRITEDATA(data) SSD1306_I2C_Write(SSD1306_I2C_ADDR, 0x40, (data))

/* Absolute value macro. */
#define ABS(x) ((x) > 0 ? (x) : -(x))


///////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES.
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Private SSD1306 data buffer.
 */
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

/**
 * @brief Private structure to store the LCD properties.
 */
static SSD1306_t SSD1306;


///////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS.
///////////////////////////////////////////////////////////////////////////////

SSD1306_status_t SSD1306_init(I2C_HandleTypeDef *i2c_ptr) {
	// Saves the used i2c peripheral and keeps track of it.
	SSD1306.i2c_ptr = i2c_ptr;

	// Resets the LCD structure.
	SSD1306.currentX = 0;
	SSD1306.currentY = 0;
	SSD1306.initialized = 0;

	if (HAL_I2C_IsDeviceReady(SSD1306.i2c_ptr, SSD1306_I2C_ADDR, 10,
		SSD1306_I2C_TIMEOUT) != HAL_OK) {
		return I2C_ERROR;
	}

	HAL_Delay(10);

	/* Initializes the LCD. */
	SSD1306_WRITECOMMAND(0xAE); //display off
	SSD1306_WRITECOMMAND(0x20); //Set Memory Addressing Mode
	SSD1306_WRITECOMMAND(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	SSD1306_WRITECOMMAND(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	SSD1306_WRITECOMMAND(0xC8); //Set COM Output Scan Direction
	SSD1306_WRITECOMMAND(0x00); //--set low column address
	SSD1306_WRITECOMMAND(0x10); //--set high column address
	SSD1306_WRITECOMMAND(0x40); //--set start line address
	SSD1306_WRITECOMMAND(0x81); //--set contrast control register
	SSD1306_WRITECOMMAND(0xFF);
	SSD1306_WRITECOMMAND(0xA1); //--set segment re-map 0 to 127
	SSD1306_WRITECOMMAND(0xA6); //--set normal display
	SSD1306_WRITECOMMAND(0xA8); //--set multiplex ratio(1 to 64)
	SSD1306_WRITECOMMAND(0x3F); //
	SSD1306_WRITECOMMAND(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	SSD1306_WRITECOMMAND(0xD3); //-set display offset
	SSD1306_WRITECOMMAND(0x00); //-not offset
	SSD1306_WRITECOMMAND(0xD5); //--set display clock divide ratio/oscillator frequency
	SSD1306_WRITECOMMAND(0xF0); //--set divide ratio
	SSD1306_WRITECOMMAND(0xD9); //--set pre-charge period
	SSD1306_WRITECOMMAND(0x22); //
	SSD1306_WRITECOMMAND(0xDA); //--set com pins hardware configuration
	SSD1306_WRITECOMMAND(0x12);
	SSD1306_WRITECOMMAND(0xDB); //--set vcomh
	SSD1306_WRITECOMMAND(0x20); //0x20,0.77xVcc
	SSD1306_WRITECOMMAND(0x8D); //--set DC-DC enable
	SSD1306_WRITECOMMAND(0x14); //
	SSD1306_WRITECOMMAND(0xAF); //--turn on SSD1306 panel

	SSD1306_WRITECOMMAND(SSD1306_DEACTIVATE_SCROLL);

	SSD1306_clear();
	SSD1306.initialized = 1;

	return LCD_OK;
}


SSD1306_status_t SSD1306_clear(void) {
	if (!SSD1306.initialized) {
		return NO_INIT;
	}

	SSD1306_fill(SSD1306_COLOR_BLACK);
	SSD1306_updateScreen();

	return LCD_OK;
}


SSD1306_status_t SSD1306_invertDisplay(uint8_t is_inverted) {
	if (!SSD1306.initialized) {
		return NO_INIT;
	}

	if (is_inverted)
		SSD1306_WRITECOMMAND(SSD1306_INVERTDISPLAY);
	else
		SSD1306_WRITECOMMAND(SSD1306_NORMALDISPLAY);

	return LCD_OK;
}


SSD1306_status_t SSD1306_updateScreen(void) {
	if (!SSD1306.initialized) {
		return NO_INIT;
	}

	for (uint8_t m = 0; m < 8; m++) {
		SSD1306_WRITECOMMAND(0xB0 + m);
		SSD1306_WRITECOMMAND(0x00);
		SSD1306_WRITECOMMAND(0x10);

		SSD1306_I2C_WriteMulti(SSD1306_I2C_ADDR, 0x40,
			&SSD1306_Buffer[SSD1306_WIDTH * m], SSD1306_WIDTH);
	}

	return LCD_OK;
}


SSD1306_status_t SSD1306_toggleInvert(void) {
	if (!SSD1306.initialized) {
		return NO_INIT;
	}

	for (uint32_t i = 0; i < sizeof(SSD1306_Buffer); i++) {
		SSD1306_Buffer[i] = ~SSD1306_Buffer[i];
	}

	// Updates internal status.
	SSD1306.inverted = !SSD1306.inverted;

	return LCD_OK;
}


SSD1306_status_t SSD1306_scrollRight(uint8_t start_page, uint8_t end_page) {
	if (start_page >= SSD1306_MAX_PAGE_NUM ||
		end_page >= SSD1306_MAX_PAGE_NUM) {

		return INVALID_PARAMS;
	}

	if (start_page > end_page) {
		return INVALID_PARAMS;
	}

	// Right horizontal scroll.
	SSD1306_WRITECOMMAND(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	SSD1306_WRITECOMMAND(0x00);       // Dummy byte.
	SSD1306_WRITECOMMAND(start_page); // Start page address.

	// Time interval between each scroll step as 5 frames.
	SSD1306_WRITECOMMAND(0X00);

	SSD1306_WRITECOMMAND(end_page);   // End page address.
	SSD1306_WRITECOMMAND(0X00);
	SSD1306_WRITECOMMAND(0xFF);		  // Scroll offset for continuous movement.
	SSD1306_WRITECOMMAND(SSD1306_ACTIVATE_SCROLL); // Start scrolling.

	return LCD_OK;
}


SSD1306_status_t SSD1306_scrollLeft(uint8_t start_page, uint8_t end_page) {
	if (start_page >= SSD1306_MAX_PAGE_NUM ||
		end_page >= SSD1306_MAX_PAGE_NUM) {

		return INVALID_PARAMS;
	}

	if (start_page > end_page) {
		return INVALID_PARAMS;
	}

	// Left horizontal scroll.
	SSD1306_WRITECOMMAND(SSD1306_LEFT_HORIZONTAL_SCROLL);
	SSD1306_WRITECOMMAND(0x00);  	  // Dummy byte.
	SSD1306_WRITECOMMAND(start_page); // Start page address.

	// Time interval between each scroll step as 5 frames.
	SSD1306_WRITECOMMAND(0X00);

	SSD1306_WRITECOMMAND(end_page);  // End page address.
	SSD1306_WRITECOMMAND(0X00);
	SSD1306_WRITECOMMAND(0XFF);
	SSD1306_WRITECOMMAND(SSD1306_ACTIVATE_SCROLL); // Start scrolling.

	return LCD_OK;
}


SSD1306_status_t SSD1306_scrollDiagRight(uint8_t start_page, uint8_t end_page) {
	if (start_page >= SSD1306_MAX_PAGE_NUM ||
		end_page >= SSD1306_MAX_PAGE_NUM) {

		return INVALID_PARAMS;
	}

	if (start_page > end_page) {
		return INVALID_PARAMS;
	}

	SSD1306_WRITECOMMAND(SSD1306_SET_VERTICAL_SCROLL_AREA);
	SSD1306_WRITECOMMAND(0x00); 					// Dummy byte.
	SSD1306_WRITECOMMAND(SSD1306_HEIGHT);

	SSD1306_WRITECOMMAND(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	SSD1306_WRITECOMMAND(0x00);
	SSD1306_WRITECOMMAND(start_page);				// Start page address.
	SSD1306_WRITECOMMAND(0X00);
	SSD1306_WRITECOMMAND(end_page);					// End page address.
	SSD1306_WRITECOMMAND(0x01);
	SSD1306_WRITECOMMAND(SSD1306_ACTIVATE_SCROLL);

	return LCD_OK;
}


SSD1306_status_t SSD1306_scrollDiagLeft(uint8_t start_page, uint8_t end_page) {
	if (start_page >= SSD1306_MAX_PAGE_NUM ||
		end_page >= SSD1306_MAX_PAGE_NUM) {

		return INVALID_PARAMS;
	}

	if (start_page > end_page) {
		return INVALID_PARAMS;
	}

	SSD1306_WRITECOMMAND(SSD1306_SET_VERTICAL_SCROLL_AREA);
	SSD1306_WRITECOMMAND(0x00);						// Dummy byte.
	SSD1306_WRITECOMMAND(SSD1306_HEIGHT);

	SSD1306_WRITECOMMAND(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	SSD1306_WRITECOMMAND(0x00);
	SSD1306_WRITECOMMAND(start_page);				// Start page address.
	SSD1306_WRITECOMMAND(0X00);
    SSD1306_WRITECOMMAND(end_page);					// End page address.
    SSD1306_WRITECOMMAND(0x01);
    SSD1306_WRITECOMMAND(SSD1306_ACTIVATE_SCROLL);

    return LCD_OK;
}


SSD1306_status_t SSD1306_stopScroll(void) {
	SSD1306_WRITECOMMAND(SSD1306_DEACTIVATE_SCROLL);

	return LCD_OK;
}


SSD1306_status_t SSD1306_fill(SSD1306_color_t color) {
	if (!SSD1306.initialized) {
		return NO_INIT;
	}

	switch (color) {
		case SSD1306_COLOR_BLACK:
			memset(SSD1306_Buffer, 0x00, sizeof(SSD1306_Buffer));
			break;
		case SSD1306_COLOR_WHITE:
			memset(SSD1306_Buffer, 0xFF, sizeof(SSD1306_Buffer));
			break;
		default:
			return INVALID_PARAMS;
	}

	return LCD_OK;
}


SSD1306_status_t SSD1306_drawPixel(uint16_t x, uint16_t y, SSD1306_color_t color) {
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
		return INVALID_PARAMS;
	}

	// Checks if pixels are inverted.
	if (SSD1306.inverted) {
		color = (SSD1306_color_t)!color;
	}

	// Sets color. Shifting right by 3 means dividing by 8. Bitwise ANDing by
	// 0x7 means getting the remainder of a division by 8.
	switch (color) {
		case SSD1306_COLOR_WHITE:
			SSD1306_Buffer[x + (y >> 3) * SSD1306_WIDTH] |= 1 << (y & 0x07);
			break;
		case SSD1306_COLOR_BLACK:
			SSD1306_Buffer[x + (y >> 3) * SSD1306_WIDTH] &= ~(1 << (y & 0x07));
			break;
		default:
			return INVALID_PARAMS;
	}

	return LCD_OK;
}


SSD1306_status_t SSD1306_drawBitmap(int16_t x, int16_t y,
	const unsigned char* bitmap, int16_t w, int16_t h, SSD1306_color_t color) {

    int16_t byteWidth = (w + 7) >> 3; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++) {
        for(int16_t i=0; i<w; i++) {
            if(i & 7) {
               byte <<= 1;
            }
            else {
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }
            if(byte & 0x80) SSD1306_drawPixel(x+i, y, color);
        }
    }

    return LCD_OK;
}


SSD1306_status_t SSD1306_gotoXY(uint16_t x, uint16_t y) {
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
		return INVALID_PARAMS;
	}

	// Sets write pointers.
	SSD1306.currentX = x;
	SSD1306.currentY = y;

	return LCD_OK;
}


SSD1306_status_t SSD1306_putc(char ch, FontDef_t* font, SSD1306_color_t color) {
	uint16_t b;
	
	// Check available space on the visible LCD area.
	if (SSD1306_WIDTH <= (SSD1306.currentX + font->fontWidth) ||
		SSD1306_HEIGHT <= (SSD1306.currentY + font->fontHeight)) {

		return INVALID_PARAMS;
	}
	
	for (uint8_t i = 0; i < font->fontHeight; i++) {
		// Since the first available character of the ASCII table is 'space'
		// (32d), subtracts it from the given char to compute the array index.
		b = font->data[(ch - 32) * font->fontHeight + i];

		for (uint8_t j = 0; j < font->fontWidth; j++) {
			if ((b << j) & 0x8000) {
				SSD1306_drawPixel(SSD1306.currentX + j, (SSD1306.currentY + i),
					color);
			} else {
				SSD1306_drawPixel(SSD1306.currentX + j, (SSD1306.currentY + i),
					!color);
			}
		}
	}
	
	// Updates the X pointer.
	SSD1306.currentX += font->fontWidth;
	
	return LCD_OK;
}


SSD1306_status_t SSD1306_puts(char* str, FontDef_t* font, SSD1306_color_t color) {
	while (*str) {
		// Write character by character.
		SSD1306_putc(*str, font, color);
		str++;
	}

	return LCD_OK;
}


SSD1306_status_t SSD1306_drawLine(uint16_t x0, uint16_t y0, uint16_t x1,
	uint16_t y1, SSD1306_color_t color) {

	int16_t dx, dy, sx, sy, err, e2, i, tmp;

	if (x0 >= SSD1306_WIDTH) {
		x0 = SSD1306_WIDTH - 1;
	}

	if (x1 >= SSD1306_WIDTH) {
		x1 = SSD1306_WIDTH - 1;
	}

	if (y0 >= SSD1306_HEIGHT) {
		y0 = SSD1306_HEIGHT - 1;
	}

	if (y1 >= SSD1306_HEIGHT) {
		y1 = SSD1306_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;

	// Shifting by 1 means diving by two.
	err = ((dx > dy) ? dx : -dy) >> 1;

	// Draws vertical line.
	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			SSD1306_drawPixel(x0, i, color);
		}

		/* Return from function */
		return LCD_OK;
	}

	// Draws horizontal line.
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			SSD1306_drawPixel(i, y0, color);
		}

		/* Return from function */
		return LCD_OK;
	}

	while (1) {
		SSD1306_drawPixel(x0, y0, color);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}

	return LCD_OK;
}


SSD1306_status_t SSD1306_drawRectangle(uint16_t x, uint16_t y, uint16_t w,
	uint16_t h, SSD1306_color_t color) {

	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
		return INVALID_PARAMS;
	}

	/* Check width and height */
	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}
	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}

	// Draws 4 lines.
	SSD1306_drawLine(x, y, x + w, y, color);         /* Top line */
	SSD1306_drawLine(x, y + h, x + w, y + h, color); /* Bottom line */
	SSD1306_drawLine(x, y, x, y + h, color);         /* Left line */
	SSD1306_drawLine(x + w, y, x + w, y + h, color); /* Right line */

	return LCD_OK;
}


SSD1306_status_t SSD1306_drawFilledRectangle(uint16_t x, uint16_t y, uint16_t w,
	uint16_t h, SSD1306_color_t color) {

	/* Check input parameters */
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
		return INVALID_PARAMS;
	}

	if ((x + w) >= SSD1306_WIDTH) {
		w = SSD1306_WIDTH - x;
	}

	if ((y + h) >= SSD1306_HEIGHT) {
		h = SSD1306_HEIGHT - y;
	}

	// Draws rectangle lines.
	for (uint8_t i = 0; i <= h; i++) {
		SSD1306_drawLine(x, y + i, x + w, y + i, color);
	}

	return LCD_OK;
}


SSD1306_status_t SSD1306_drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2,
	uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_color_t color) {

	SSD1306_drawLine(x1, y1, x2, y2, color);
	SSD1306_drawLine(x2, y2, x3, y3, color);
	SSD1306_drawLine(x3, y3, x1, y1, color);

	return LCD_OK;
}


SSD1306_status_t SSD1306_drawFilledTriangle(uint16_t x1, uint16_t y1,
	uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_color_t color) {

	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		SSD1306_drawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}

	return LCD_OK;
}


SSD1306_status_t SSD1306_drawCircle(int16_t x0, int16_t y0, int16_t r,
	SSD1306_color_t color) {

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SSD1306_drawPixel(x0, y0 + r, color);
    SSD1306_drawPixel(x0, y0 - r, color);
    SSD1306_drawPixel(x0 + r, y0, color);
    SSD1306_drawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_drawPixel(x0 + x, y0 + y, color);
        SSD1306_drawPixel(x0 - x, y0 + y, color);
        SSD1306_drawPixel(x0 + x, y0 - y, color);
        SSD1306_drawPixel(x0 - x, y0 - y, color);

        SSD1306_drawPixel(x0 + y, y0 + x, color);
        SSD1306_drawPixel(x0 - y, y0 + x, color);
        SSD1306_drawPixel(x0 + y, y0 - x, color);
        SSD1306_drawPixel(x0 - y, y0 - x, color);
    }

    return LCD_OK;
}


SSD1306_status_t SSD1306_drawFilledCircle(int16_t x0, int16_t y0, int16_t r,
	SSD1306_color_t color) {

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    SSD1306_drawPixel(x0, y0 + r, color);
    SSD1306_drawPixel(x0, y0 - r, color);
    SSD1306_drawPixel(x0 + r, y0, color);
    SSD1306_drawPixel(x0 - r, y0, color);
    SSD1306_drawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_drawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        SSD1306_drawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

        SSD1306_drawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
        SSD1306_drawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }

    return LCD_OK;
}


SSD1306_status_t SSD1306_lcdOn(void) {
	SSD1306_WRITECOMMAND(0x8D);  
	SSD1306_WRITECOMMAND(0x14);  
	SSD1306_WRITECOMMAND(0xAF);

	return LCD_OK;
}


SSD1306_status_t SSD1306_lcdOff(void) {
	SSD1306_WRITECOMMAND(0x8D);  
	SSD1306_WRITECOMMAND(0x10);
	SSD1306_WRITECOMMAND(0xAE);

	return LCD_OK;
}


///////////////////////////////////////////////////////////////////////////////
// I2C COMMUNICATION FUNCTIONS.
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Temporary buffer to store data to be transmitted to the LCD.
 */
static uint8_t data_tmp[SSD1306_I2C_DATATMP_SIZE];


void SSD1306_I2C_WriteMulti(uint8_t addr, uint8_t reg, uint8_t* data,
	uint16_t count) {

	// Avoids buffer overflow.
	if (count > SSD1306_I2C_DATATMP_SIZE) {
		return;
	}

	data_tmp[0] = reg;

	memcpy(data_tmp + 1, data, count);

	HAL_I2C_Master_Transmit(SSD1306.i2c_ptr, addr, data_tmp, count + 1,
		SSD1306_I2C_TIMEOUT);

	return;
}


void SSD1306_I2C_Write(uint8_t addr, uint8_t reg, uint8_t data) {
	data_tmp[0] = reg;
	data_tmp[1] = data;

	HAL_I2C_Master_Transmit(SSD1306.i2c_ptr, addr, data_tmp, 2,
		SSD1306_I2C_TIMEOUT);

	return;
}
