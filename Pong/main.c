/*
 * main.c
 *
 *  Created on: 14 May 2022
 *      Author: lukes
 */
#include "DE1SoC_LT24/DE1SoC_LT24.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_usleep/HPS_usleep.h"
#include "ScreenBuffer/ScreenBuffer.h"
#include "Sprites/Sprites.h"
#include "pongEngine/pongEngine.h"
#include "Servo_CrowdEyes/Servo_CrowdEyes.h"

#include "HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "DE1SoC_SevenSeg/DE1SoC_SevenSeg.h"

#include <stdlib.h>
#include <stdio.h>

// Define global variables
// ---------------------------------
unsigned short framebuffer[76800];
unsigned int velocity =3;
unsigned int x;
unsigned int startup_flag;
unsigned int displaytime_flag;
unsigned int timer_loop;
struct image paddle1;
struct image paddle2;
struct image ball;

// Define prototypes
// ---------------------------------
void init(void);
void gameplay(void);
void introscreen(void);

// Define base addresses
volatile unsigned int *switch_ptr = (unsigned int *)0xFF200040;  // base address for switches


// --------------------------------------------------------------------------------------------------------
// Define bass addresses
volatile unsigned int *key_ptr = (unsigned int *)0xFF20005C;  // base address for rising edge of key press


 void initialise_privateTimer( void );

 //Define new data type for function which takes an int parameter and returns updated int values.
 typedef unsigned int (*TaskFunction)( unsigned int);

 // Define task scheduler function to update hundredths
 unsigned int hundredths ( unsigned int );

 // Define task scheduler function to update seconds
 unsigned int seconds ( unsigned int );

 // Define task scheduler function to update minutes
 unsigned int minutes ( unsigned int );

 // Define task scheduler function to update hours
 unsigned int hours ( unsigned int );

 // Define function to display time on 7-segment display
 void SevenSegDisplay_Time( unsigned int timeValues[] );

 // Define reset timer values to zero and update lastTimerValue with current time values.
 void ResetTimer( unsigned int timeValues[], unsigned int lastTimerValue[], unsigned int );

 unsigned int last_x = 0;
 unsigned int i;
 const unsigned int taskCount = 4;                                                                // Define number of task counts (hundredths, seconds, minutes, hours)
 unsigned int timeValues[taskCount] = {0};                                                        // Declare stop watch time values to be passed to display
 unsigned int lastTimerValue[taskCount] = {0};                                                    // Last timer values are initialised to zero (hundredths, seconds, minutes, hours)
 const unsigned int period = 1000000;                                                             // Period for 1 sec
 const unsigned int incrementedPeriod[taskCount] = {period/100, period, period*60, period*3600};  // Increment timer periods (hundredths, seconds, minutes, hours)
 TaskFunction taskFunctions[taskCount] = {&hundredths, &seconds, &minutes, &hours};               // Define task Functions to call time increments (hundredths, seconds, minutes, hours)

 // --------------------------------------------------------------------------------------------------------


// ---------------------------------
// ---------------------------------
// Main loop
// ---------------------------------
// ---------------------------------
 main(void) {

	// Initialise game libraries
	init();

    //Wait a moment
    usleep(500000);
    HPS_ResetWatchdog();

    //Main Run Loop
    while (1) {

    	// switch 1 used to start game
    	x = *switch_ptr & 0x3;

    	// Check if it is time to run each task
    	for (i = 0; i < taskCount; i++) {
    	    if ((lastTimerValue[i] - Timer_readCurrentTimer()) >=  incrementedPeriod[i]) {
    	    	timeValues[i] = taskFunctions[i](timeValues[i]);
    	    	lastTimerValue[i] = lastTimerValue[i] - incrementedPeriod[i];
    	    }
    	 }

    	 // Upon start up timer is not running until a switch is switched to "1/ON" to start stop watch
    	 if (startup_flag == 1) {

    		 ResetTimer(timeValues, lastTimerValue, taskCount);  // Reset timer values and push current timer into last increment time
    		 Timer_setControlvals(224, 0, 1, 0);                 // Timer is paused at startup, begins once key 0 is pressed
    		 startup_flag = 0;                                   // Clears startup_flag to avoid returning to function
    		 last_x = 0;
    	 }
    	// Show intro screen
    	if(x == 0 ){

    		introscreen();
    		SevenSegDisplay_Time(timeValues);                // Display paused time on 7-segment displays
    		Timer_setControlvals(224, 0, 1, 0);              // Stop timer

    		HPS_ResetWatchdog();
    	}
    	// Start game
    	if( x == 1 ){

    		gameplay();

    		Timer_setControlvals(224, 0, 1, 1);  // start timer but don't print to screen

    		HPS_ResetWatchdog();
    	}
    	// Make sure we clear the private timer interrupt flag if it is set
    	if (Timer_readInterruptFlag() & 0x1) {
    	    // If the timer interrupt flag is set, clear the flag
    	    Timer_resetInterruptFlag();
    	}

    	HPS_ResetWatchdog();
    }
}

 void exitOnFail(signed int status, signed int successStatus){
    if (status != successStatus) {
        exit((int)status); //Add breakpoint here to catch failure
    }
 }

// Initialise game libraries
 void init() {

	 initialise_privateTimer();

	// initialise start up flag
	startup_flag = 1;

	// Initialise game libraries
 	pongEngine_paddle1Init(&paddle1);
 	pongEngine_paddle2Init(&paddle2);
 	pongEngine_ballInit(&ball, velocity);
 	pongEngine_UpdateScore(0,0);

 	// Initialise servo
 	Servo_CrowdEyes_Init ();

 	// Initialise 7 segment-display
	DE1SoC_SevenSeg_SetDoubleDec(0, 0);
	DE1SoC_SevenSeg_SetDoubleDec(4, 0);
	DE1SoC_SevenSeg_Write(2, 0x40);
	DE1SoC_SevenSeg_Write(3, 0x40);

 	//Initialise the LCD Display.
 	exitOnFail(
 	        LT24_initialise(0xFF200060,0xFF200080), //Initialise LCD
 	        LT24_SUCCESS);                          //Exit if not successful

 	HPS_ResetWatchdog();
 }

 // Intro splash screen
 void introscreen(){

		 ScreenBuffer_setFrameBuffer(framebuffer, intro_background);
		 LT24_copyFrameBuffer(framebuffer, 0, 0, 240, 320);
		 HPS_ResetWatchdog();

 }

 // Gameplay functions
 void gameplay(){

 	pongEngine_paddleUpdatePos(&paddle1, &paddle2);
 	pongEngine_ballUpdatePos(&ball);
 	Servo_CrowdEyes_Update (&ball, &paddle1, &paddle2, velocity);

 	pongEngine_checkWallCollision(&ball);
 	pongEngine_checkPaddleCollision(&ball, &paddle1, &paddle2);
 	pongEngine_checkPointWon(&ball, velocity);

 	ScreenBuffer_setFrameBuffer(framebuffer, background);
 	ScreenBuffer_drawSprite(framebuffer, paddle2.spriteId, paddle2.xleft, paddle2.ytop, paddle2.width, paddle2.height);
   	ScreenBuffer_drawSprite(framebuffer, paddle1.spriteId, paddle1.xleft, paddle1.ytop, paddle1.width, paddle1.height);
   	ScreenBuffer_drawSprite(framebuffer, ball.spriteId, ball.xleft, ball.ytop, ball.width, ball.height);

    LT24_copyFrameBuffer(framebuffer, 0, 0, 240, 320);

    // Testing
   	// printf("player 1 score: %i ; player 2 score: %i %i \n",score_player1,score_player2, paddle2.xleft);

    HPS_ResetWatchdog();

 }


 // Timer Functions

 void initialise_privateTimer( void ) {

     // Configure the ARM Private Timer using functions
     Timer_initialise(0xFFFEC600);      // Timer base address
     Timer_setLoadval(0xFFFFFFFF);      // Set to maximum value
     Timer_setControlvals(224, 0, 1, 1);  // (prescaler, I, E, A): Set clock frequency - prescaler (prescaler = 225-1, as clock rate is 225MHz), disable ISR - 0,  enable automatic overflow - A, enable timer - E

 }

 // If called by task scheduler, function increments the hundredths value by 1 and resets value to zero beyond 100 calls
 unsigned int hundredths (unsigned int hundredths) {

 	if (hundredths < 99) {
 		hundredths = hundredths + 1;
 	}
 	else {
 		hundredths = 0;
 	}

 	return hundredths;
 }

 // If called by task scheduler, function increments the seconds value by 1 and resets value to zero beyond 60 calls
 unsigned int seconds (unsigned int seconds) {

 	if (seconds < 59) {
 		seconds = seconds + 1;
 	}
 	else {
 		seconds = 0;
 	}

 	return seconds;
 }

 // If called by task scheduler, function increments the minutes value by 1 and resets value to zero beyond 60 calls
 unsigned int minutes (unsigned int minutes) {

 	if (minutes < 59) {
 		minutes = minutes + 1;
 	}
 	else {
 		minutes = 0;
 	}

 	return minutes;
 }

 // If called by task scheduler, function increments the hours value by 1 and resets value to zero beyond 24 calls
 unsigned int hours (unsigned int hours) {

 	if (hours < 23) {
 		hours = hours + 1;
 	}
 	else {
 		hours = 0;
 	}

 	return hours;

 }

 // Displays the time on the 7-segment displays
 void SevenSegDisplay_Time(unsigned int timeValues[]) {

 	unsigned int hours = timeValues[3];

 	// If under an hour display MM:SS:FF
 	if (hours <= 0) {

 		DE1SoC_SevenSeg_SetDoubleDec(0, timeValues[0]);  // hundredths in lowest pair of 7-segment displays
 		DE1SoC_SevenSeg_SetDoubleDec(2, timeValues[1]);  // seconds in middle pair of 7-segment displays
 		DE1SoC_SevenSeg_SetDoubleDec(4, timeValues[2]);  // minutes in highest pair of 7-segment displays

 	}
 	// If above an hour display HH:MM:SS
 	else {

 		DE1SoC_SevenSeg_SetDoubleDec(0, timeValues[1]);  // seconds in lowest pair of 7-segment displays
 		DE1SoC_SevenSeg_SetDoubleDec(2, timeValues[2]);  // minutes in middle pair of 7-segment displays
 		DE1SoC_SevenSeg_SetDoubleDec(4, timeValues[3]);  // hours in highest pair of 7-segment displays

 	}

 }

 // Reset Timer Values. Set all values for hundredths, seconds, minutes, hours to zero
 // Set the lastTimerValue for all the tasks to the current time value
 void ResetTimer( unsigned int timeValues[], unsigned int lastTimerValue[], unsigned int taskCount ) {

 	unsigned int counter;

 	for (counter = 0; counter < taskCount; counter++){
 		timeValues[counter] = 0;                             // All time values are eual to zero
 		lastTimerValue[counter] = Timer_readCurrentTimer();  // All tasks begin from current time value
 	}
 }






