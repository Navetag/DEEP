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
#include "cube_types.h"

typedef enum{
	VINCENT_INIT,
	VINCENT_RESET,
	VINCENT_IDLE,
	VINCENT_SHUFFLE_SELECTED,
	VINCENT_SHUFFLE,
	VINCENT_RESOLVE_SELECTED,
	VINCENT_MODELING,
	VINCENT_RESOLVE
}vincent_state_e;

typedef enum {
	VINCENT_MODELING_FINISHED,
	VINCENT_MODELING_RESOLVED,
	VINCENT_MODELING_PROCESSING
}modeling_result_e;

typedef enum{
	VINCENT_MODELING_PHASE_0,
	VINCENT_MODELING_PHASE_1,
	VINCENT_MODELING_PHASE_2,
	VINCENT_MODELING_PHASE_3,
	VINCENT_MODELING_PHASE_4,
	VINCENT_MODELING_PHASE_5,
	VINCENT_MODELING_PHASE_6,
	VINCENT_MODELING_PHASE_7,
	VINCENT_MODELING_PHASE_8,
	VINCENT_MODELING_PHASE_9,
	VINCENT_MODELING_PHASE_10,
	VINCENT_MODELING_PHASE_11,
	VINCENT_MODELING_PHASE_12,
}modeling_state_e;

void VINCENT_state();

void VINCENT_initCube(cube_s *cube);

modeling_result_e VINCENT_modelingProcess(cube_s *cube, bool_e *new_modelisation);


void VINCENT_process(){
	cube_servo_process();
	screen_process();
	VINCENT_state();

}

void VINCENT_state(){
	static cube_s cube;
	static 	vincent_state_e previous_state 	= VINCENT_INIT,
							state 			= VINCENT_INIT;
	static bool_e new_modelisation = false;

	bool_e entrance = state != previous_state;
	previous_state = state;

	user_action_e user_action = NONE;
//	user_action = screen_getLastUserAction();
	if(state != VINCENT_INIT && user_action == CLOSE){
		state = VINCENT_RESET;
	}

	switch(state){
		case VINCENT_INIT:
			CAM_init();
			VINCENT_initCube(&cube);
			state = VINCENT_RESET;
			break;
		case VINCENT_RESET:
			if(entrance){
				CAM_askFor(CAM_LED_OFF);
				//Servo default
			}
			if(CAM_isReady() && cube_servo_getState() == FINISHED){
				CAM_flush();
				state = VINCENT_RESET;
			}
			break;
		case VINCENT_IDLE:
			if(entrance){
				//SCRREN IDLE
			}
			if(user_action == SHUFFLE){
				state = VINCENT_SHUFFLE_SELECTED;
			}else if(user_action == SOLVE){
				state = VINCENT_RESOLVE_SELECTED;
			}
			break;
		case VINCENT_SHUFFLE_SELECTED:
			if(entrance){
				//Set screen SHUFFLING MENU
			}
			if(user_action == START){
				state = VINCENT_SHUFFLE;
			}
			break;
		case VINCENT_SHUFFLE:
			if(entrance){
				//Screen set SHUFFLING
				cube_servo_complex_mvt_e moves[] = {BOTTOM_180, LEFT_180};
				uint8_t nb_moves = 2;
				for(uint8_t i = 0; i < nb_moves; i++){
					cube_servo_addMvt(moves[i]);
				}
			}
			if(cube_servo_getState() == FINISHED)
				state = VINCENT_RESET;
			break;
		case VINCENT_RESOLVE_SELECTED:
			if(entrance){
				//Screen set SOLVING_MENU
			}
			if(user_action == START){
				state = VINCENT_MODELING;
			}
			break;
		case VINCENT_MODELING:
			if(entrance){
				new_modelisation = true;
				//Screen print cube / SCANNING
			}

			modeling_result_e result = VINCENT_modelingProcess(&cube, &new_modelisation);
			if(result == VINCENT_MODELING_FINISHED){
				state = VINCENT_RESOLVE;
			}else if(result == VINCENT_MODELING_RESOLVED){
				state = VINCENT_RESET;
			}

			break;
		case VINCENT_RESOLVE:
			if(entrance){
				//Screen set SOLVING
				cube_servo_complex_mvt_e moves[] = {BOTTOM_180, LEFT_180};
				uint8_t nb_moves = 2;
				for(uint8_t i = 0; i < nb_moves; i++){
					cube_servo_addMvt(moves[i]);
				}
			}
			if(cube_servo_getState() == FINISHED)
				state = VINCENT_MODELING;
			break;
		default:
			state = VINCENT_RESET;
			break;
	}
}

void VINCENT_initCube(cube_s *cube){
	for(uint8_t i = 0; i < 3; i++){
		for(uint8_t j = 0; j < 3; j++){
			cube->up[i][j] = CUBE_WHITE;
			cube->down[i][j] = CUBE_WHITE;
			cube->front[i][j] = CUBE_WHITE;
			cube->back[i][j] = CUBE_WHITE;
			cube->right[i][j] = CUBE_WHITE;
			cube->left[i][j] = CUBE_WHITE;
		}
	}
}

modeling_result_e VINCENT_modelingProcess(cube_s *cube, bool_e *new_modelisation){
	static 	modeling_state_e 	previous_state 	= VINCENT_MODELING_PHASE_12,
								state 			= VINCENT_MODELING_PHASE_0;

	if(*new_modelisation){
		state = VINCENT_MODELING_PHASE_0;
		(*new_modelisation) = false;
	}

	bool_e entrance = previous_state != state;
	previous_state = state;

	modeling_result_e return_value = VINCENT_MODELING_PROCESSING;

	switch (state) {
		case VINCENT_MODELING_PHASE_0:
			if(entrance){
				VINCENT_initCube(cube);
				CAM_askFor(CAM_LED_PARTIAL_ON);
			}
			if(CAM_isReady()){
				CAM_flush();
				state = VINCENT_MODELING_PHASE_1;
			}
			break;
		default:
			break;
	}

	return return_value;

}
