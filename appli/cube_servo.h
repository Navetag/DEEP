/*
 * cube_servo.h
 *
 *  Created on: 10 juin 2023
 *      Author: auger
 */

#ifndef CUBE_SERVO_H_
#define CUBE_SERVO_H_

#include "macro_types.h"

#define CUBE_SERVO_TIME_WAIT_STATE_MS 	10000

#define CUBE_SERVO_PERIOD_TIMER			10

#define CUBE_SERVO_MAX_REVERSE_POS_VAL	0
#define CUBE_SERVO_MAX_CLKWISE_POS_VAL	100
#define CUBE_SERVO_MIDDLE_POS_VAL		50

#define CUBE_SERVO_FLIP_UP_POS_VAL		50
#define CUBE_SERVO_DEFAULT_CAGE_POS_VAL	20
#define CUBE_SERVO_HOLD_CUBE_POS_VAL	0

typedef enum{
    CUBE_SERVO_BOTTOM_90_A,
	CUBE_SERVO_BOTTOM_180,
	CUBE_SERVO_BOTTOM_90,

	CUBE_SERVO_TOP_90_A,
	CUBE_SERVO_TOP_180,
	CUBE_SERVO_TOP_90,

	CUBE_SERVO_FRONT_90_A,
	CUBE_SERVO_FRONT_180,
	CUBE_SERVO_FRONT_90,

	CUBE_SERVO_BACK_90_A,
	CUBE_SERVO_BACK_180,
	CUBE_SERVO_BACK_90,

	CUBE_SERVO_LEFT_90_A,
	CUBE_SERVO_LEFT_180,
	CUBE_SERVO_LEFT_90,

	CUBE_SERVO_RIGHT_90_A,
	CUBE_SERVO_RIGHT_180,
	CUBE_SERVO_RIGHT_90,

	CUBE_SERVO_GAUCHE_90,
	CUBE_SERVO_DROITE_90,
	CUBE_SERVO_FLIP,

	CUBE_SERVO_DEFAULT_POS,
	CUBE_SERVO_DEFAULT_ROT_COMPLEX,
	CUBE_SERVO_DEFAULT_CAGE_COMPLEX
}cube_servo_complex_mvt_e;

typedef enum{
	CUBE_SERVO_INIT,
	CUBE_SERVO_WAIT_SERVO,
	CUBE_SERVO_MAKE,
	CUBE_SERVO_FINISHED
}cube_servo_state_e;

/*
    Machine à état
        - gère une pile FIFO contenant les mvts en attente (tableau de )
        - récupère l'état du cube
*/
void CUBE_SERVO_process(void);

/*
    Ajoute un mouvement dans la pile
*/
void CUBE_SERVO_addMvt(cube_servo_complex_mvt_e mvt);

/*
    Renvoie l'état de la machine servo
*/
cube_servo_state_e CUBE_SERVO_getState(void);
void CUBE_SERVO_test(void);
#endif /* CUBE_SERVO_H_ */
