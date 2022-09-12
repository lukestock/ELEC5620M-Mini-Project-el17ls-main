/*
 * pongEngine.c
 *
 *  Created on: 16 May 2022
 *      Author: lukes
 */

#include "pongEngine.h"
#include "../DE1SoC_SevenSeg/DE1SoC_SevenSeg.h"

// Define base addresses
extern volatile unsigned int *key_ptr ;  // base address for rising edge of key press = (unsigned int *)0xFF20005C;

// Define variables
// ----------------------------------
// Paddle variables
// ----------------------------------
unsigned int paddle_width = 32;
unsigned int paddle_height = 32;
unsigned int paddle_thickness = 5;
unsigned int paddle_indent = 15;
unsigned int temp_keypressed;
unsigned int flag_paddlestopped = 0;
signed int x_pos_pad1;
signed int y_pos_pad1;
signed int x_pos_pad2;
signed int y_pos_pad2;

// Ball variables
// ----------------------------------
unsigned int ball_width = 18;
unsigned int direction;
signed int velocity_x;
signed int velocity_y;
unsigned int position_x;
unsigned int position_y;
signed int x_pos_ball;
signed int y_pos_ball;

// Scoring variables
// ----------------------------------
unsigned int score_player1 = 0;
unsigned int score_player2 = 0;



// -----------------------------------------------
// -----------------------------------------------
// -------------- Paddle functions ---------------
// -----------------------------------------------
// -----------------------------------------------

// Initialises paddle1 member values to position paddle mid-screen at correct court side
void pongEngine_paddle1Init(struct image* paddle1) {

	paddle1->xleft = LT24_WIDTH/2 - paddle_width/2;
	paddle1->ytop = paddle_indent;
	paddle1->width = paddle_width;
	paddle1->height = paddle_height;
	paddle1->spriteId = paddleSprite;
	paddle1->x_incr = 10;

}

// Initialises paddle2 member values to position paddle mid-screen at correct court side
void pongEngine_paddle2Init(struct image* paddle2) {

	paddle2->xleft = LT24_WIDTH/2 - paddle_width/2;
	paddle2->ytop = LT24_HEIGHT - 2*paddle_indent - paddle_thickness;
	paddle2->width = paddle_width;
	paddle2->height = paddle_height;
	paddle2->spriteId = paddleSprite;
	paddle2->x_incr = 10;

}

// Initialises ball members values to position the ball at centre of the court
void pongEngine_ballInit(struct image* ball, unsigned int velocity){

	ball->xleft = LT24_WIDTH/2 - ball_width/2;
	ball->ytop = LT24_HEIGHT/2 - ball_width/2;
	ball->width = ball_width;
	ball->height = ball_width;
	ball->spriteId = ballSprite;
	ball->x_incr = 0;
	ball->y_incr = 0;

	// Random serve from centre court
	// Randomise serve direction
	direction = rand() % 4;

	if (direction == 0){
		ball->x_incr = velocity;
		ball->y_incr  = velocity;
	} else if (direction == 1) {
		ball->x_incr = -velocity;
		ball->y_incr  = velocity;
	} else if (direction == 2) {
		ball->x_incr = velocity;
		ball->y_incr  = -velocity;
	} else  {
		ball->x_incr = -velocity;
		ball->y_incr  = -velocity;
	}

}
// Updates paddle1 and paddle2 position read from key buttons inputs
void pongEngine_paddleUpdatePos( struct image* paddle1, struct image* paddle2 ){

	// Move paddle1 DOWN
	if (*key_ptr & 0x01){

		paddle1->xleft = paddle1->xleft + paddle1->x_incr;
		resetKeyInputs();

	}
	// Move paddle1 UP
	if (*key_ptr & 0x02){

		paddle1->xleft = paddle1->xleft - paddle1->x_incr;
		resetKeyInputs();

	}
	// Move paddle2 Down
	if (*key_ptr & 0x04){

		paddle2->xleft = paddle2->xleft + paddle2->x_incr;
		resetKeyInputs();

	}
	// Move paddle2 UP
	if (*key_ptr & 0x08){

		paddle2->xleft = paddle2->xleft - paddle2->x_incr;
		resetKeyInputs();

	}

	//contain within screen boundary limits
	if (paddle1->xleft < 0){
		paddle1->xleft = 0;
	}
	if (paddle2->xleft < 0){
		paddle2->xleft = 0;
	}
	if (paddle1->xleft > LT24_WIDTH - paddle_width){
		paddle1->xleft = LT24_WIDTH - paddle_width;
	}
	if (paddle2->xleft > LT24_WIDTH - paddle_width){
		paddle2->xleft = LT24_WIDTH - paddle_width;
	}

}

// Get the paddle1 position in x
signed int pongEngine_paddle1GetPos_X(struct image* paddle1){
	x_pos_pad1 = paddle1->xleft;

	return x_pos_pad1;
}

// Get the paddle1 position in y
signed int pongEngine_paddle1GetPos_Y(struct image* paddle1){
	y_pos_pad1 = paddle1->ytop;

	return y_pos_pad1;
}

// Get the paddle2 position in x
signed int pongEngine_paddle2GetPos_X(struct image* paddle2){
	x_pos_pad2 = paddle2->xleft;

	return x_pos_pad2;
}

// Get the paddle2 position in y
signed int pongEngine_paddle2GetPos_Y(struct image* paddle2){
	y_pos_pad2 = paddle2->ytop;

	return y_pos_pad2;
}


// -----------------------------------------------
// -----------------------------------------------
// -------------- Ball functions -----------------
// -----------------------------------------------
// -----------------------------------------------

// Updates ball position
void pongEngine_ballUpdatePos(struct image* ball){

	ball->xleft = ball->xleft + ball->x_incr;
	ball->ytop = ball->ytop + ball->y_incr;

}

// -----------------------------------------------
// -----------------------------------------------
// -------------- Input functions ----------------
// -----------------------------------------------
// -----------------------------------------------

// Resets key buttons pressed by writing 1 to all bits to clear them
void resetKeyInputs() {
	*key_ptr = 0xF;
}

// -----------------------------------------------
// -----------------------------------------------
// ------------ Collision functions --------------
// -----------------------------------------------
// -----------------------------------------------

// Checks if ball has collided with wall
// If ball has collided with wall it's x direction is inverted, to reflect trajectory,
// and y direction is unchanged to continue moving towards correct goal end
void pongEngine_checkWallCollision(struct image* ball ){

	// checks that ball has hit top of screen and updates movement
	if (ball->xleft <= 1 ) {
		ball->xleft = 1;                                     // Stops ball travelling off screen
		ball->x_incr = (- ball->x_incr);                     // Inverts velocity to bounce ball off of the wall (screen)
	}
	// checks that ball has hit bottom of screen and updates movement
	else if (ball->xleft >= LT24_WIDTH-1 - ball_width ) {
			ball->xleft = (LT24_WIDTH-1) - ball_width;       // Stops ball travelling off screen
			ball->x_incr = (- ball->x_incr);                 // Inverts velocity to bounce ball off of the wall (screen)
		}

}

// Checks if ball has collided with paddle 1 or 2
// If ball has collided with wall it's y direction is inverted, to reflect trajectory,
// to opposite goal end and x direction is unchanged to continue moving in correct direction
void pongEngine_checkPaddleCollision(struct image* ball, struct image* paddle1, struct image* paddle2) {

	// checks that ball has collided with paddle 1 and updates movement
	if ((ball->xleft >= paddle1->xleft - ball_width/2                    )   // below top
		&& (ball->xleft <= paddle1->xleft + paddle_height + ball_width/2 )   // above bottom
		&& (ball->ytop <= paddle1->ytop + paddle_thickness               )   // inside left
		&& (ball->ytop >= paddle1->ytop                                  ))  // inside right
	{
		ball->ytop = paddle1->ytop +  ball_width;                            // Stops ball travelling beyond paddle
		ball->y_incr = (- ball->y_incr);                                     // Inverts velocity to bounce ball off of the paddle
	}
	// checks that ball has collided with paddle 2 and updates movement
	if ((ball->xleft >= paddle2->xleft - ball_width/2                    )   // below top
		&& (ball->xleft <= paddle2->xleft + paddle_height + ball_width/2 )   // above bottom
		&& (ball->ytop < paddle2->ytop - ball_width + 3*paddle_thickness )   // inside left
		&& (ball->ytop > paddle2->ytop - ball_width + 2*paddle_thickness ))  // inside right
	{
		ball->ytop = paddle2->ytop - ball_width + 2*paddle_thickness;        // Stops ball travelling beyond paddle
		ball->y_incr = (- ball->y_incr);                                     // Inverts velocity to bounce ball off of the wall (screen)
	}
}

// Check if player has won a point
void pongEngine_checkPointWon(struct image* ball, unsigned int velocity ) {

	    // checks if player 2 has won a point
		if (ball->ytop <= 1 ) {
			ball->ytop = 1;                                                  // Stops ball going off screen
			ball->x_incr = 0;                                                // Stops ball moving
			ball->y_incr = 0;                                                // Stops ball moving
			pongEngine_playerScored(2);
			pongEngine_ballInit(ball, velocity);
		}
		// checks if player 1 has won a point
		else if (ball->ytop >= LT24_HEIGHT - ball_width ) {
				ball->ytop = LT24_HEIGHT - ball_width;                       // Stops ball going off screen
				ball->x_incr = 0;                                            // Stops ball moving
				ball->y_incr = 0;                                            // Stops ball moving
				pongEngine_playerScored(1);
				pongEngine_ballInit(ball, velocity);
			}

}

// Check if player has scored, if so increase player's score
void pongEngine_playerScored(unsigned int player){

	if (player == 1){
		score_player1++;
	}
	if (player == 2){
		score_player2++;
	}

	pongEngine_UpdateScore(score_player1, score_player2);
}

// Update score to 7-seg display with player score
void pongEngine_UpdateScore(unsigned int score_player1, unsigned int score_player2){

	DE1SoC_SevenSeg_SetDoubleDec(0, score_player1);

	DE1SoC_SevenSeg_SetDoubleDec(4, score_player2);

	DE1SoC_SevenSeg_Write(2, 0x40);
	DE1SoC_SevenSeg_Write(3, 0x40);

}


