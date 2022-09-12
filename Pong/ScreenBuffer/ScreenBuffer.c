/*
 *    ScreenBuffer.c
 *    -----------------------
 *    Created on: 15 May 2022
 *    Author: Luke Stock
 *
 *    Description:
 *    -----------------------
 *    A screen buffer library to draw frame images that will be drawn on the 240x320 pixel LCS screen.
 *	  Allows sprites to be drawn over a background image, by coping a background image frame and replacing
 *	  background image pixels with sprite image pixels.
 */


#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "ScreenBuffer.h"

// Copies 240x340 background image size to a reference frame buffer, pixel by pixel
// Adapted Bitmap representation from: https://www.davidepesce.com/2018/10/15/diyconsole-part-5-sprites/
void ScreenBuffer_setFrameBuffer (unsigned short* framebuffer, const unsigned short* background) {

	unsigned int bitmapindex;

	for (bitmapindex = 0; bitmapindex < 76800; bitmapindex++){

		framebuffer[bitmapindex] = background [bitmapindex];

	}
}

// Draws the sprite RGB565 image over background frame buffer set in ScreenBuffer_setFrameBuffer()
// Adapted Bitmap representation from: https://www.davidepesce.com/2018/10/15/diyconsole-part-5-sprites/
void ScreenBuffer_drawSprite (unsigned short* framebuffer, const unsigned short* sprite, signed int xleft, signed int ytop, signed int width, signed int height) {
	signed int x1, y1;
	signed int x, y, spriteBitmapIndex, backgroundBitmapIndex;
	bool within_range;

	// Calculate maximum x and y points
	x1 = xleft + (width - 1);
	y1 = ytop +  (height - 1);

	// Check sprite dimensions are within screen 240x340 boundary limits
	// If beyond boundary limit then condense into screen boundary
	if ( xleft < 0 )         xleft = 0;
	if ( ytop < 0  )         ytop = 0;
	if ( x1 > LT24_WIDTH-1)  x1 = LT24_WIDTH-1;
	if ( y1 > LT24_HEIGHT-1) y1 = LT24_HEIGHT-1;

	// Print sprite bitmap to screen buffer row by row, column by column
	for(y = y1; y != ytop; y--) {
		for(x = x1; x != xleft; x--) {
			spriteBitmapIndex = (y - ytop)* width + (x - xleft);                      // Current sprite array pixel index
			backgroundBitmapIndex = (y*240) + x;                                      // Current background array pixel index

			// Check if pixel is within sprite boundary range
			if  ((x < xleft + width) && (y < ytop + height)) {
				within_range = true;
			}
			else {
				within_range = false;
			}

			// Draw sprite pixel on top of background frame buffer
			if (sprite[spriteBitmapIndex] != 0x0000 && (within_range == true) ) {     // Check pixel is not transparent (=0x0000) and pixel is within x and y range limit to ensure pixel is printed
					framebuffer[backgroundBitmapIndex] = sprite[spriteBitmapIndex];   // Print sprite to frame buffer background image
			}
		}
	}
}



