/*
 * Servo_CrowdEyes.h
 *
 *  Created on: 18 May 2022
 *      Author: lukes
 */

#ifndef SERVO_CROWDEYES_H_
#define SERVO_CROWDEYES_H_

#include "DE1SoC_Servo/DE1SoC_Servo.h"
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "../pongEngine/pongEngine.h"

#include <stdlib.h>
#include <math.h>


//Initialise Servo Driver
void Servo_CrowdEyes_Init (void);

// Update servo motor position
void Servo_CrowdEyes_Update (struct image* ball, struct image* paddle1, struct image* paddle2, unsigned int velocity);

#endif /* SERVO_CROWDEYES_H_ */
