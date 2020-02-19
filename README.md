[![Codacy Badge](https://api.codacy.com/project/badge/Grade/dee6f005b2004c4092abbeed8aae6317)](https://www.codacy.com/manual/hardware994/OLED-SSD1306-STM32-DRIVER?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=GiovanniScotti/OLED-SSD1306-STM32-DRIVER&amp;utm_campaign=Badge_Grade)

# OLED SSD1306 driver for STM32 microcontrollers
## Introduction
This is a highly portable driver written in C for the SSD1306 LCD.
Target microcontrollers are the STM32f10x and STM32F4xx families.
The driver uses I2C for communication and library functions allow to draw lines,
rectangles and circles. It is also possible to draw text and single characters only.
Moreover, advanced scrolling functions are included.

## Library usage
To successfully run the driver, copy the .h and .c files in the appropriate
directory of your project and proceed with the following steps:
*  find the "driver settings" section in the ssd1306.h file and uncomment the correct define
   to make the library compatible with your target STM32 microcontroller.
```C
#define STM32_FLAVOUR STM32F1XX
//#define STM32_FLAVOUR STM32F4XX
```
*  include the ssd1306.h header file into your project.
```C
#include "ssd1306.h"
```
*  before drawing on the screen, remember to call the SSD1306\_init initialization function
   passing a pointer to a valid i2c peripheral. The function prototype follows for your
   convenience.
```C
SSD1306_status_t SSD1306_init(I2C_HandleTypeDef *i2c_ptr);
```

## Credits
The original version of this driver has been implemented by Tilen Majerle and extended by
Alexander Lutsai.
