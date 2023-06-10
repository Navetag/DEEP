/*
 * vincentColin.c
 *
 *  Created on: 8 juin 2023
 *      Author: caill
 */

#include "vincent_colin.h"
#include "cam.h"
#include "cube_types.h"
#include "cube_servo.h"
#include "screen.h"

/*
 * =====================================================================================
 * Types privés
 * =====================================================================================
 */

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
	VINCENT_MODELING_PROCESSING,
	VINCENT_MODELING_NEW_FACE
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
	VINCENT_MODELING_PHASE_13,
	VINCENT_MODELING_PHASE_14,
}modeling_state_e;

/*
 * =====================================================================================
 * Prototypes privés
 * =====================================================================================
 */

void VINCENT_state();

void VINCENT_initCube(cube_s *cube);

modeling_result_e VINCENT_modelingProcess(cube_s *cube, bool_e *new_modelisation);

bool_e VINCENT_cubeResolved(cube_s *cube);

void VINCENT_copyFace(face_t dest_face, face_t src_face);

/*
 * =====================================================================================
 * Fonctions publiques
 * =====================================================================================
 */

void VINCENT_process(){
	CUBE_SERVO_process();
	SCREEN_process();
	VINCENT_state();
}

/*
 * =====================================================================================
 * Fonctions privées
 * =====================================================================================
 */

void VINCENT_state(){
	static cube_s cube;
	static 	vincent_state_e previous_state 	= VINCENT_INIT,
							state 			= VINCENT_INIT;
	static bool_e new_modelisation = false;

	bool_e entrance = state != previous_state;
	previous_state = state;

	user_action_e user_action = SCREEN_getLastUserAction();
	if(user_action == SCREEN_ACTION_CLOSE && state != VINCENT_INIT){
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
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
			}
			else if(CAM_isReady() && CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				CAM_flush();
				state = VINCENT_IDLE;
			}
			break;
		case VINCENT_IDLE:
			if(entrance){
				SCREEN_display(SCREEN_MENU, NULL);
			}
			if(user_action == SCREEN_ACTION_SHUFFLE){
				state = VINCENT_SHUFFLE_SELECTED;
			}else if(user_action == SCREEN_ACTION_SOLVE){
				state = VINCENT_RESOLVE_SELECTED;
			}
			break;
		case VINCENT_SHUFFLE_SELECTED:
			if(entrance){
				SCREEN_display(SCREEN_SHUFFLING_MENU, NULL);
			}
			if(user_action == SCREEN_ACTION_START){
				state = VINCENT_SHUFFLE;
			}
			break;
		case VINCENT_SHUFFLE:
			if(entrance){
				SCREEN_display(SCREEN_SHUFFLING, NULL);
				cube_servo_complex_mvt_e moves[] = {CUBE_SERVO_BOTTOM_180, CUBE_SERVO_LEFT_180};
				uint8_t nb_moves = 2;
				for(uint8_t i = 0; i < nb_moves; i++){
					CUBE_SERVO_addMvt(moves[i]);
				}
			}
			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED)
				state = VINCENT_RESET;
			break;
		case VINCENT_RESOLVE_SELECTED:
			if(entrance){
				SCREEN_display(SCREEN_RESOLVING_MENU, NULL);
			}
			if(user_action == SCREEN_ACTION_START){
				state = VINCENT_MODELING;
			}
			break;
		case VINCENT_MODELING:
			if(entrance){
				new_modelisation = true;
				VINCENT_initCube(&cube);
				SCREEN_display(SCREEN_MODELING, &cube);
			}
			modeling_result_e result = VINCENT_modelingProcess(&cube, &new_modelisation);
			if(result == VINCENT_MODELING_NEW_FACE){
				SCREEN_display(SCREEN_MODELING, &cube);
			}else if(result == VINCENT_MODELING_FINISHED){
				if(VINCENT_cubeResolved(&cube))
					state = VINCENT_RESET;
				else
					state = VINCENT_RESOLVE;
			}
			break;
		case VINCENT_RESOLVE:
			if(entrance){
				SCREEN_display(SCREEN_RESOLVING, NULL);
				cube_servo_complex_mvt_e moves[] = {CUBE_SERVO_BOTTOM_180, CUBE_SERVO_LEFT_180};
				uint8_t nb_moves = 2;
				for(uint8_t i = 0; i < nb_moves; i++){
					CUBE_SERVO_addMvt(moves[i]);
				}
			}
			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED)
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
	/*
	 * 					=========================== Attention, "CUBE_SERVO_DEFAULT_POS" change aussi la rotation !!!! ===========================
	 */
	static 	modeling_state_e 	previous_state 	= VINCENT_MODELING_PHASE_0,
								state 			= VINCENT_MODELING_PHASE_0;

	static face_t face;

	if(*new_modelisation){
		state = VINCENT_MODELING_PHASE_0;
		previous_state = VINCENT_MODELING_PHASE_0;
		(*new_modelisation) = false;
	}

	bool_e entrance = previous_state != state;
	previous_state = state;

	modeling_result_e return_value = VINCENT_MODELING_PROCESSING;

	switch (state) {
		case VINCENT_MODELING_PHASE_0: //Init
			state = VINCENT_MODELING_PHASE_1;
			break;
		case VINCENT_MODELING_PHASE_1: //Default pos + LED ON
			if(entrance){
				CAM_askFor(CAM_LED_PARTIAL_ON);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
			}
			else if(CAM_isReady() && CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				CAM_flush();
				state = VINCENT_MODELING_PHASE_2;
			}
			break;
		case VINCENT_MODELING_PHASE_2: //Read face UP
			if(entrance){
				CAM_askFor(CAM_FACE);
			}
			else if(CAM_isReady()){
				CAM_getFace(face);
				VINCENT_copyFace(cube->up, face);
				CAM_flush(); //Mesure de précaution
				state = VINCENT_MODELING_PHASE_3;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_3: //Flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
			}
			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				state = VINCENT_MODELING_PHASE_4;
			}
			break;
		case VINCENT_MODELING_PHASE_4: //Read face BACK
			if(entrance){
				CAM_askFor(CAM_FACE);
			}
			else if(CAM_isReady()){
				CAM_getFace(face);
				VINCENT_copyFace(cube->back, face);
				CAM_flush(); //Mesure de précaution
				state = VINCENT_MODELING_PHASE_5;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_5: //Flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
			}
			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				state = VINCENT_MODELING_PHASE_6;
			}
			break;
		case VINCENT_MODELING_PHASE_6: //Read face DOWN
			if(entrance){
				CAM_askFor(CAM_FACE);
			}
			else if(CAM_isReady()){
				CAM_getFace(face);
				VINCENT_copyFace(cube->down, face);
				CAM_flush(); //Mesure de précaution
				state = VINCENT_MODELING_PHASE_7;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_7: //Flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
			}
			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				state = VINCENT_MODELING_PHASE_8;
			}
			break;
		case VINCENT_MODELING_PHASE_8: //Read face FRONT
			if(entrance){
				CAM_askFor(CAM_FACE);
			}
			else if(CAM_isReady()){
				CAM_getFace(face);
				VINCENT_copyFace(cube->front, face);
				CAM_flush(); //Mesure de précaution
				state = VINCENT_MODELING_PHASE_9;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_9: //Right, flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_DROITE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
			}
			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				state = VINCENT_MODELING_PHASE_10;
			}
			break;
		case VINCENT_MODELING_PHASE_10: //Read face LEFT
			if(entrance){
				CAM_askFor(CAM_FACE);
			}
			else if(CAM_isReady()){
				CAM_getFace(face);
				VINCENT_copyFace(cube->left, face);
				CAM_flush(); //Mesure de précaution
				state = VINCENT_MODELING_PHASE_11;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_11: //left left flip, flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_GAUCHE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_GAUCHE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
			}
			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				state = VINCENT_MODELING_PHASE_12;
			}
			break;
		case VINCENT_MODELING_PHASE_12: //Read face RIGHT
			if(entrance){
				CAM_askFor(CAM_FACE);
			}
			else if(CAM_isReady()){
				CAM_getFace(face);
				VINCENT_copyFace(cube->right, face);
				CAM_flush(); //Mesure de précaution
				state = VINCENT_MODELING_PHASE_13;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_13: //Right, right, flip, left, flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_DROITE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_DROITE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_GAUCHE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
				CAM_askFor(CAM_LED_OFF);
			}
			else if(CAM_isReady() && CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				CAM_flush();
				state = VINCENT_MODELING_PHASE_14;
			}
			break;
		case VINCENT_MODELING_PHASE_14: //Finished
			return_value = VINCENT_MODELING_FINISHED;
			break;
		default:
			break;
	}

	return return_value;

}

bool_e VINCENT_cubeResolved(cube_s *cube){
	if(!cube) return FALSE;
	for(uint8_t i = 0; i < 3; i++){
		for (uint8_t j = 0; j < 3; j++){
			if(cube->up[i][j] != cube->up[0][0]) return false;
			if(cube->down[i][j] != cube->down[0][0]) return false;
			if(cube->front[i][j] != cube->front[0][0]) return false;
			if(cube->back[i][j] != cube->back[0][0]) return false;
			if(cube->left[i][j] != cube->left[0][0]) return false;
			if(cube->right[i][j] != cube->right[0][0]) return false;
		}
	}
	return true;
}

void VINCENT_copyFace(face_t dest_face, face_t src_face){
	for(uint8_t y = 0; y < 3; y++){
		for(uint8_t x = 0; x < 3; x++){
			dest_face[y][x] = src_face[y][x];
		}
	}
}
