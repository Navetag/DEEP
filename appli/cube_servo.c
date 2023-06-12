/*
 * cube_servo.c
 *
 *  Created on: 10 juin 2023
 *      Author: auger
 */

#include "systick.h"
#include "stm32f1_timer.h"
#include "config.h"
#include "cube_servo.h"

/*
 * =====================================================================================
 * Types privés
 * =====================================================================================
 */

typedef enum{
	CUBE_SERVO_MAX_CLKWISE,
	CUBE_SERVO_MAX_REVERSE,
	CUBE_SERVO_MIDDLE_POS,

	CUBE_SERVO_HOLD_CUBE,
	CUBE_SERVO_FLIP_UP,
	CUBE_SERVO_DEFAULT_CAGE         //Le doigt ne touche pas et le cube peut tourner, position de lecture du cube

}cube_servo_primary_mvt_e;

typedef enum{
	CUBE_SERVO_POS_CENTER,
	CUBE_SERVO_POS_LEFT,
	CUBE_SERVO_POS_RIGHT
}cube_servo_rot_pos_e;

typedef enum{
	CUBE_SERVO_POS_FLIP_UP,
	CUBE_SERVO_POS_DEFAULT,
	CUBE_SERVO_POS_CUBE_LOCK
}cube_servo_cage_pos_e;

/*
 * =====================================================================================
 * Prototypes privés
 * =====================================================================================
 */

void CUBE_SERVO_init(void);

void CUBE_SERVO_waitingTimer(void);

void CUBE_SERVO_handlePrimary(cube_servo_primary_mvt_e mvt);

void CUBE_SERVO_queue(cube_servo_primary_mvt_e mvt);

void CUBE_SERVO_unqueue(void);

void CUBE_SERVO_supportSetPosition(uint16_t position);

void CUBE_SERVO_cageSetPosition(uint16_t position);

/*
 * =====================================================================================
 * Variables privées
 * =====================================================================================
 */

static cube_servo_rot_pos_e cube_rot_position;
static cube_servo_cage_pos_e cube_cage_position;

static cube_servo_primary_mvt_e primary_mvt_list[2048];

static cube_servo_state_e current_state = CUBE_SERVO_INIT;
static cube_servo_state_e previous_state;

int queue_pointer = -1;

static volatile int t;

/*
 * =====================================================================================
 * Fonctions publiques
 * =====================================================================================
 */
void CUBE_SERVO_test(){
	CUBE_SERVO_process();
	CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_ROT_COMPLEX);
	while(1){
		CUBE_SERVO_process();
	}
}
void CUBE_SERVO_process(){
	bool_e entrance = current_state != previous_state;
	previous_state = current_state;

	switch(current_state){
		case CUBE_SERVO_INIT :
			Systick_add_callback_function(&CUBE_SERVO_waitingTimer);
			CUBE_SERVO_init();
			for(int i = 0 ; i<500; i++){
				primary_mvt_list[i] = -1;
			}
			cube_rot_position = CUBE_SERVO_POS_CENTER;
			cube_cage_position = CUBE_SERVO_DEFAULT_CAGE;
			current_state = CUBE_SERVO_MAKE;
			break;
		case CUBE_SERVO_MAKE :
			if(queue_pointer < 0){
				current_state = CUBE_SERVO_FINISHED;
			}
			else{
				CUBE_SERVO_handlePrimary(primary_mvt_list[0]);
				current_state = CUBE_SERVO_WAIT_SERVO;
			}
			break;
		case CUBE_SERVO_WAIT_SERVO :
			if(entrance){
				t = CUBE_SERVO_TIME_WAIT_STATE_MS;
			}
			if(t<=0){
				current_state = CUBE_SERVO_MAKE;
			}
			break;
		case CUBE_SERVO_FINISHED :
			break;
		default :
			break;
	}
}

void CUBE_SERVO_addMvt(cube_servo_complex_mvt_e mvt){
	switch(mvt){
		case CUBE_SERVO_BOTTOM_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_BOTTOM_180:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_BOTTOM_90:
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_TOP_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_TOP_180:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_TOP_90:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_FRONT_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_FRONT_180:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_FRONT_90:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_BACK_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_BACK_180:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_BACK_90:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_LEFT_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_LEFT_180:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_LEFT_90:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_RIGHT_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_RIGHT_180:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_RIGHT_90:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_GAUCHE_90:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			break;
		case CUBE_SERVO_DROITE_90:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			break;
		case CUBE_SERVO_FLIP:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			break;
		case CUBE_SERVO_DEFAULT_POS:
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_DEFAULT_CAGE_COMPLEX:
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_DEFAULT_ROT_COMPLEX:
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		default:
			break;
	}
	if(current_state == CUBE_SERVO_FINISHED) current_state = CUBE_SERVO_MAKE;
}

cube_servo_state_e CUBE_SERVO_getState(void){
	return current_state;
}

/*
 * =====================================================================================
 * Fonctions privées
 * =====================================================================================
 */

void CUBE_SERVO_init(){
	//initialisation et lancement du timer1 à une période de 10 ms
	TIMER_run_us(TIMER1_ID, CUBE_SERVO_PERIOD_TIMER*1000, FALSE); //10000us = 10ms

	//activation du signal PWM sur le canal 1 du timer 1 (broche PA8)
	TIMER_enable_PWM(TIMER1_ID, TIM_CHANNEL_1, 150, FALSE, FALSE);
	//rapport cyclique reglé pour une position servo de 50%
	CUBE_SERVO_supportSetPosition(CUBE_SERVO_MIDDLE_POS_VAL);

	//activation du signal PWM sur le canal 1 du timer 1 (broche PA9)
	TIMER_enable_PWM(TIMER1_ID, TIM_CHANNEL_2, 150, FALSE, FALSE);
	//rapport cyclique reglé pour une position servo de 50%
	CUBE_SERVO_cageSetPosition(CUBE_SERVO_DEFAULT_CAGE_POS_VAL);
}

void CUBE_SERVO_waitingTimer(){
	t = t-1;
}

void CUBE_SERVO_handlePrimary(cube_servo_primary_mvt_e mvt){
	switch(mvt){
		case CUBE_SERVO_MAX_CLKWISE :
			CUBE_SERVO_supportSetPosition(CUBE_SERVO_MAX_CLKWISE_POS_VAL);
			break;
		case CUBE_SERVO_MAX_REVERSE :
			CUBE_SERVO_supportSetPosition(CUBE_SERVO_MAX_REVERSE_POS_VAL);
			break;
		case CUBE_SERVO_MIDDLE_POS:
			CUBE_SERVO_supportSetPosition(CUBE_SERVO_MIDDLE_POS_VAL);
			break;

		case CUBE_SERVO_HOLD_CUBE :
			CUBE_SERVO_cageSetPosition(CUBE_SERVO_HOLD_CUBE_POS_VAL);
			break;
		case CUBE_SERVO_FLIP_UP :
			CUBE_SERVO_cageSetPosition(CUBE_SERVO_FLIP_UP_POS_VAL);
			break;

		case CUBE_SERVO_DEFAULT_CAGE :
			CUBE_SERVO_cageSetPosition(CUBE_SERVO_DEFAULT_CAGE_POS_VAL);
			break;
	}
	CUBE_SERVO_unqueue();
}

void CUBE_SERVO_queue(cube_servo_primary_mvt_e mvt){
	queue_pointer ++;
	primary_mvt_list[queue_pointer] = mvt;
}

void CUBE_SERVO_unqueue(){
	for(int i = 0; i <= queue_pointer; i++){
		primary_mvt_list[i] = primary_mvt_list[i+1];
	}
	queue_pointer--;
}

void CUBE_SERVO_supportSetPosition(uint16_t position){
	if(position > 100)
		position = 100; //écretage si l'utilisateur demande plus de 100%

	TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_1, position+100);
}

void CUBE_SERVO_cageSetPosition(uint16_t position){
	if(position > 100)
		position = 100; //écretage si l'utilisateur demande plus de 100%

	TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_2, position+100);
}
