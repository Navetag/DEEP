/*
 * vincentColin.c
 *
 *  Created on: 8 juin 2023
 *      Author: caill
 */

#include <vincent_colin.h>
#include "cube_servo.h"
#include "screen.h"
#include "cam.h"

typedef enum{
	VINCENT_INIT,
	VINCENT_IDLE,
	VINCENT_SHUFFLE_SELECTED,
	VINCENT_SHUFFLE,
	VINCENT_RESOLVE_SELECTED,
	VINCENT_MODELING,
	VINCENT_RESOLVE
}vincent_state_e;

void VINCENT_state();

void VINCENT_process(){
//	cube_servo_process();
//	screen_process();
	VINCENT_state();

}

void VINCENT_state(){
	static 	vincent_state_e previous_state 	= VINCENT_INIT,
							state 			= VINCENT_INIT;

	bool_e entrance = state != previous_state;
	previous_state = state;

	user_action_e user_action = NONE;
//	user_action = screen_getLastUserAction();

	switch(state){
	case VINCENT_INIT:
		CAM_init();
		state = VINCENT_IDLE;
		break;
	case VINCENT_IDLE:
		if(entrance){
			//LED OFF, SERVO DEFAULT, SCRREN IDLE
		}

		if(user_action == SHUFFLE){
			state = VINCENT_SHUFFLE_SELECTED;
		}else if(user_action == SOLVE){
			state = VINCENT_RESOLVE_SELECTED;
		}
		break;
	case VINCENT_SHUFFLE_SELECTED:
		if(entrance){
			//Set screen SHUFFLING MENU, SERVO DEFAULT
		}
		if(user_action == START){
			state = VINCENT_SHUFFLE;
		}
		break;
	case VINCENT_SHUFFLE:
		if(entrance){
			cube_servo_complex_mvt_e moves[] = {BOTTOM_180, BOTTOM_90};
			uint8_t nb_moves = 2;
			for(uint8_t i = 0; i < nb_moves;_i++){
				//Add MOVE (moves[i])
			}

//			if(servo == FINISHED)
				state = IDLE;
		}
		break;
	case VINCENT_RESOLVE_SELECTED:
		break;
	case VINCENT_MODELING:
		break;
	case VINCENT_RESOLVE:
		break;
	default:
		state = IDLE;
		break;
	}
}
