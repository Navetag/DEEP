/**
 * @file vincent_colin.c
 * @author armand
 * @date 01 avril 2023
 * @brief Sources du module vincent_colin.
 * @version 0.1
 */

#include "vincent_colin.h"
#include "cam.h"
#include "cube_types.h"
#include "cube_servo.h"
#include "screen.h"

/*
 * =====================================================================================
 * Types priv�s
 * =====================================================================================
 */

/**
 * @enum vincent_state_e
 * @brief Etats du projet
 */
typedef enum{
	VINCENT_INIT,				/*!< Initialisation*/
	VINCENT_RESET,				/*!< Remise en etat par defaut*/
	VINCENT_IDLE,				/*!< Etat d'attente, inactif*/
	VINCENT_SHUFFLE_SELECTED,	/*!< Melange selectionne*/
	VINCENT_SHUFFLE,			/*!< En cours de melange*/
	VINCENT_RESOLVE_SELECTED,	/*!< Resolution selectionnee*/
	VINCENT_MODELING,			/*!< En cours de modelisation*/
	VINCENT_RESOLVE				/*!< En cours de resolution*/
}vincent_state_e;

/**
 * @enum modeling_result_e
 * @brief Resultat du process de modelisation du Rubik's cube
 */
typedef enum {
	VINCENT_MODELING_FINISHED,	/*!< Modelisation terminee*/
	VINCENT_MODELING_PROCESSING,/*!< Modelisation en cours*/
	VINCENT_MODELING_NEW_FACE	/*!< Une nouvelle face a ete modelisee*/
}modeling_result_e;

/**
 * @enum modeling_state_e
 * @brief Phases de modelisation du Rubik's cube
 */
typedef enum{
	VINCENT_MODELING_PHASE_0,	/*!< Initialisation*/
	VINCENT_MODELING_PHASE_1,	/*!< Position par defaut, LED allumees*/
	VINCENT_MODELING_PHASE_2,	/*!< Modelisation face haute*/
	VINCENT_MODELING_PHASE_3,	/*!< Flip*/
	VINCENT_MODELING_PHASE_4,	/*!< Modelisation face arriere*/
	VINCENT_MODELING_PHASE_5,	/*!< Flip*/
	VINCENT_MODELING_PHASE_6,	/*!< Modelisation face basse*/
	VINCENT_MODELING_PHASE_7,	/*!< Flip*/
	VINCENT_MODELING_PHASE_8,	/*!< Modelisation face avant*/
	VINCENT_MODELING_PHASE_9,	/*!< Gauche, Flip*/
	VINCENT_MODELING_PHASE_10,	/*!< Modelisation face droite*/
	VINCENT_MODELING_PHASE_11,	/*!< Droite, Flip, Flip*/
	VINCENT_MODELING_PHASE_12,	/*!< Modelisation face gauche*/
	VINCENT_MODELING_PHASE_13,	/*!< Gauche, Flip, Centre, Flip, LED eteintes*/
	VINCENT_MODELING_PHASE_14,	/*!< Termine*/
}modeling_state_e;

/*
 * =====================================================================================
 * Prototypes priv�s
 * =====================================================================================
 */

void VINCENT_state();

void VINCENT_initCube(cube_s *cube);

modeling_result_e VINCENT_modelingProcess(cube_s *cube, bool_e *new_modelisation);

bool_e VINCENT_cubeResolved(cube_s *cube);

#if MAIN_TEST

modeling_result_e VINCENT_modelingTest(bool_e *new_modelisation);

#endif

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

#if MAIN_TEST

void VINCENT_testPixy(){
	static cube_s cube;
	CAM_init();
	while(1){
		CAM_askFor(CAM_LED_OFF);
		HAL_Delay(100);
		CAM_flush();
		HAL_Delay(1000);
		CAM_askFor(CAM_LED_PARTIAL_ON);
		HAL_Delay(500);
		CAM_flush();
		CAM_askFor(CAM_FACE);
		HAL_Delay(200);
		VINCENT_initCube(&cube);
		CAM_getFace(cube.up);
		HAL_Delay(1000);
	}
}

void VINCENT_testPixy2(){while(1){
	static cube_s cube;
	static 	vincent_state_e previous_state 	= VINCENT_INIT,
							state 			= VINCENT_INIT;

	bool_e entrance = state != previous_state;
	previous_state = state;

	switch(state){
		case VINCENT_INIT:
			CAM_init();
			VINCENT_initCube(&cube);
			state = VINCENT_RESET;
			break;
		case VINCENT_RESET:
			if(entrance){
				CAM_askFor(CAM_LED_OFF);
			}
			else if(CAM_isReady()){
				CAM_flush();
				state = VINCENT_RESOLVE_SELECTED;
			}
			break;
		case VINCENT_RESOLVE_SELECTED:
			if(entrance){
				CAM_askFor(CAM_LED_PARTIAL_ON);
			}
			else if(CAM_isReady()){
				CAM_flush();
				state = VINCENT_MODELING;
			}
			break;
		case VINCENT_MODELING:
			if(entrance){
				VINCENT_initCube(&cube);
				CAM_askFor(CAM_FACE);
			}
			else if(CAM_isReady()){
				CAM_getFace(cube.up);
				state = VINCENT_RESET;
			}
			break;
		default:
			state = VINCENT_RESET;
			break;
	}
}}

void VINCENT_testServos(void){while(1){
	CUBE_SERVO_process();

	static 	vincent_state_e previous_state 	= VINCENT_INIT,
							state 			= VINCENT_INIT;

	bool_e entrance = state != previous_state;
	previous_state = state;

	switch(state){
		case VINCENT_INIT:
			state = VINCENT_RESET;
			break;
		case VINCENT_RESET:
			state = VINCENT_MODELING;
			break;
		case VINCENT_MODELING:
			if(entrance){
				//Default
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
				//Flip
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
				//Flip
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
				//Flip
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
				//Left, flip
				CUBE_SERVO_addMvt(CUBE_SERVO_DROITE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
				//Right, flip, flip
				CUBE_SERVO_addMvt(CUBE_SERVO_GAUCHE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
				//Left, flip, centered, flip
				CUBE_SERVO_addMvt(CUBE_SERVO_DROITE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_ROT_COMPLEX);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
				//INITIAL POS !
			}
			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				state = VINCENT_RESET;
			}
			break;
		default:
			state = VINCENT_RESET;
			break;
	}
}}

void VINCENT_testScreen(void){while(1){

	SCREEN_process();

	static cube_s cube;
	static 	vincent_state_e previous_state 	= VINCENT_INIT,
							state 			= VINCENT_INIT;

	bool_e entrance = state != previous_state;
	previous_state = state;

	screen_action_e user_action = SCREEN_getLastUserAction();


	if(user_action == SCREEN_ACTION_CLOSE && state != VINCENT_INIT){
		state = VINCENT_RESET;
	}

	switch(state){
		case VINCENT_INIT:
			VINCENT_initCube(&cube);
			state = VINCENT_RESET;
			break;
		case VINCENT_RESET:
			state = VINCENT_IDLE;
			break;
		case VINCENT_IDLE:
			if(entrance){
				SCREEN_display(SCREEN_MENU, NULL);
			}
			if(user_action == SCREEN_ACTION_SHUFFLE){
				state = VINCENT_SHUFFLE_SELECTED;
			}else if(user_action == SCREEN_ACTION_RESOLVE){
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
//				cube_servo_complex_mvt_e moves[] = {CUBE_SERVO_BOTTOM_180};
//				uint8_t nb_moves = 1;
//				for(uint8_t i = 0; i < nb_moves; i++){
//					CUBE_SERVO_addMvt(moves[i]);
//				}
			}
//			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED)
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
				SCREEN_display(SCREEN_MODELING, &cube);
//				cube_servo_complex_mvt_e moves[] = {CUBE_SERVO_LEFT_180};
//				uint8_t nb_moves =1;
//				for(uint8_t i = 0; i < nb_moves; i++){
//					CUBE_SERVO_addMvt(moves[i]);
//				}
			}
//			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED)
				state = VINCENT_RESOLVE;
			break;
		case VINCENT_RESOLVE:
			if(entrance){
				SCREEN_display(SCREEN_RESOLVING, NULL);
//				cube_servo_complex_mvt_e moves[] = {CUBE_SERVO_BOTTOM_180, CUBE_SERVO_LEFT_180};
//				uint8_t nb_moves = 2;
//				for(uint8_t i = 0; i < nb_moves; i++){
//					CUBE_SERVO_addMvt(moves[i]);
//				}
			}
//			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED)
				state = VINCENT_RESET;
			break;
		default:
			state = VINCENT_RESET;
			break;
	}
}}

void VINCENT_projetFacade(){while(1){
	CUBE_SERVO_process();
	SCREEN_process();

	static cube_s cube;
	static 	vincent_state_e previous_state 	= VINCENT_INIT,
							state 			= VINCENT_INIT;
	static bool_e new_modelisation = false;

	bool_e entrance = state != previous_state;
	previous_state = state;

	screen_action_e user_action = SCREEN_getLastUserAction();
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
				CUBE_SERVO_flush();
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
			}else if(user_action == SCREEN_ACTION_RESOLVE){
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
				cube_servo_complex_mvt_e moves[] = {CUBE_SERVO_LEFT_180};
				uint8_t nb_moves = 1;
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
				SCREEN_display(SCREEN_MODELING, &cube);
			}
			modeling_result_e result = VINCENT_modelingTest(&new_modelisation);
			if(result == VINCENT_MODELING_FINISHED){
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
}}

#endif

/*
 * =====================================================================================
 * Fonctions priv�es
 * =====================================================================================
 */

void VINCENT_state(){
	static cube_s cube;
	static 	vincent_state_e previous_state 	= VINCENT_INIT,
							state 			= VINCENT_INIT;
	static bool_e new_modelisation = false;

	bool_e entrance = state != previous_state;
	previous_state = state;

	screen_action_e user_action = SCREEN_getLastUserAction();
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
			}else if(user_action == SCREEN_ACTION_RESOLVE){
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
	static 	modeling_state_e 	previous_state 	= VINCENT_MODELING_PHASE_0,
								state 			= VINCENT_MODELING_PHASE_0;

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
				CAM_getFace(cube->up);
				CAM_flush(); //Mesure de pr�caution
				state = VINCENT_MODELING_PHASE_3;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_3: //Flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
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
				CAM_getFace(cube->back);
				CAM_flush(); //Mesure de pr�caution
				state = VINCENT_MODELING_PHASE_5;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_5: //Flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
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
				CAM_getFace(cube->down);
				CAM_flush(); //Mesure de pr�caution
				state = VINCENT_MODELING_PHASE_7;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_7: //Flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
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
				CAM_getFace(cube->front);
				CAM_flush(); //Mesure de pr�caution
				state = VINCENT_MODELING_PHASE_9;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_9: //Right, flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_DROITE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
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
				CAM_getFace(cube->left);
				CAM_flush(); //Mesure de pr�caution
				state = VINCENT_MODELING_PHASE_11;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_11: //Left, flip, flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_GAUCHE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
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
				CAM_getFace(cube->right);
				CAM_flush(); //Mesure de pr�caution
				state = VINCENT_MODELING_PHASE_13;
				return_value = VINCENT_MODELING_NEW_FACE;
			}
			break;
		case VINCENT_MODELING_PHASE_13: //Right, flip, centered, flip
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_DROITE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_ROT_COMPLEX);
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

#if MAIN_TEST

modeling_result_e VINCENT_modelingTest(bool_e *new_modelisation){
	static 	modeling_state_e 	previous_state 	= VINCENT_MODELING_PHASE_0,
								state 			= VINCENT_MODELING_PHASE_0;

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
		case VINCENT_MODELING_PHASE_2: //Random moves
			if(entrance){
				CUBE_SERVO_addMvt(CUBE_SERVO_GAUCHE_90);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
				CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
				CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_POS);
			}
			else if(CUBE_SERVO_getState() == CUBE_SERVO_FINISHED){
				state = VINCENT_MODELING_PHASE_3;
			}
			break;
		case VINCENT_MODELING_PHASE_3: //End phase
			if(entrance){
				CAM_askFor(CAM_LED_OFF);
			}
			else if(CAM_isReady()){
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

#endif
