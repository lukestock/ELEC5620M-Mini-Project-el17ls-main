/*
 *    Servo_CrowdEyes.c
 *    -----------------------
 *    Created on: 18 May 2022
 *    Author: Luke Stock
 *
 *    Description:
 *    -----------------------
 *    Library to initialise and interact with the servo motor.
 *    Servo turns with ball motion in y direction to ressembled crowd
 *    watching motion of ball.
  */

#include "Servo_CrowdEyes.h"

// Define Servo variables
// ------------------------------
signed int servo_maxsteps =  256;
signed int last_motion = 0;
signed int posn = 0;

 //Initialise Servo Driver
void Servo_CrowdEyes_Init (void){

    exitOnFail(
            Servo_initialise(0xFF2000C0),   //Initialise Servo Controller
            SERVO_SUCCESS);                 //Exit if not successful
    exitOnFail(
            Servo_enable(0, true),          //Enable Servo 0.
            SERVO_SUCCESS);                 //Exit if not successful
    exitOnFail(
            Servo_pulseWidthRange(0, true), //Double pulse width.
            SERVO_SUCCESS);                 //Exit if not successful


    Servo_pulseWidth(0,0);

}

// Update servo motor position
// ball starts at 340/2; moves positively from y = 170 : 320; moves negatively from y : 170 :0 (range = 170)
// Servo starts at 256/2; moves postively from 127 : 256; moves negatively from 127:0 (range 120/170)
void Servo_CrowdEyes_Update (struct image* ball, struct image* paddle1, struct image* paddle2, unsigned int velocity ) {
	// Read y direction position of ball
	signed int current_motion = ball->ytop;  // Determine how to actuate servo

	// Calculate number of ball increments in y direction between paddles (93 when velocity = 3)
	signed int incr = ceil((LT24_HEIGHT - 40)/ (2*velocity));

	// if ball moves right
	if (current_motion == last_motion - 3 ) {
		posn = posn - 1;
		Servo_pulseWidth(0,posn);
	}
	// if ball moves left
	else if (current_motion == last_motion + 3 ){
		posn = posn + 1;
		Servo_pulseWidth(0,posn);
	}
	// if ball hit paddle 1
	else if (current_motion == paddle1->ytop +  18 + ball->y_incr ){
		posn = posn + 1;
		Servo_pulseWidth(0,posn);
	}
	// if ball bit paddle 2
	else if (current_motion == paddle2->ytop - 18 + 2*30 + ball->y_incr ){
		posn = posn -1 ;
		Servo_pulseWidth(0,posn);
	}
	else {
		Servo_pulseWidth(0,0);
		posn = 0;
	}

	last_motion = ball->ytop;
}
