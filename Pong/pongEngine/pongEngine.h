/*
 * pongEngine.h
 *
 *  Created on: 16 May 2022
 *      Author: lukes
 */

#ifndef PONGENGINE_H_
#define PONGENGINE_H_

#include <math.h>
#include <stdlib.h>

#include "../Sprites/Sprites.h"

//Size of the LCD
#define LT24_WIDTH  240
#define LT24_HEIGHT 320

// Define struct
struct image {
	signed int xleft, ytop, width, height, x_incr, y_incr;
	const unsigned short* spriteId;
};

// Initialise game libraries
void init(void);

// Initialise paddle1 values
void pongEngine_paddle1Init(struct image* paddle1 );

// Initialise paddle2 values
void pongEngine_paddle2Init(struct image* paddle2 );

// Initialises ball values
void pongEngine_ballInit(struct image* ball, unsigned int velocity);

// Update paddle 1 and 2 position values
void pongEngine_paddleUpdatePos( struct image* paddle1, struct image* paddle2 );

// Get the paddle1 position in x
signed int pongEngine_paddle1GetPos_X(struct image* paddle1);

// Get the paddle1 position in y
signed int pongEngine_paddle1GetPos_Y(struct image* paddle1);

// Get the paddle2 position in x
signed int pongEngine_paddle2GetPos_X(struct image* paddle2);

// Get the paddle2 position in y
signed int pongEngine_paddle2GetPos_Y(struct image* paddle2);

// Update ball position values
void pongEngine_ballUpdatePos(struct image* ball);

// Resets key buttons pressed by writing 1 to all bits to clear them
void resetKeyInputs(void);

// Checks if ball has collided with wall
void pongEngine_checkWallCollision(struct image* ball);

// Checks if ball has collided with paddle 1 or 2
void pongEngine_checkPaddleCollision(struct image* ball, struct image* paddle1, struct image* paddle2);

// Check if player has won a point
void pongEngine_checkPointWon(struct image* ball, unsigned int velocity);

// Check if player has scored
void pongEngine_playerScored(unsigned int player);

void pongEngine_UpdateScore(unsigned int score_player1, unsigned int score_player2);

#endif /* PONGENGINE_H_ */
