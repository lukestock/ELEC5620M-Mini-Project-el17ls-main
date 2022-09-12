 /*
 * ScreenBuffer.h
 *
 *  Created on: 15 May 2022
 *      Author: lukes
 */

#ifndef SCREENBUFFER_H_
#define SCREENBUFFER_H_

//Include required header files
#include <stdbool.h> //Boolean variable type "bool" and "true"/"false" constants.

//Size of the LCD
#define LT24_WIDTH  240
#define LT24_HEIGHT 320

// Copies 240x340 background image size to a reference frame buffer, pixel by pixel
void ScreenBuffer_setFrameBuffer (unsigned short* framebuffer, const unsigned short* background);

// Draws the sprite RGB565 image over background frame buffer set in ScreenBuffer_setFrameBuffer()
void ScreenBuffer_drawSprite (unsigned short* framebuffer, const unsigned short* sprite, signed int xleft, signed int ytop, signed int width, signed int height);

#endif /* SCREENBUFFER_H_ */
