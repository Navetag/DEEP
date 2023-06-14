/*
 * screen.c
 *
 *  Created on: 10 juin 2023
 *      Author: auger
 */

#include "screen.h"
#include "config.h"
#include "systick.h"
#include "stm32f1_ili9341.h"
#include "stm32f1_xpt2046.h"
#include "macro_types.h"

/*
 * =====================================================================================
 * Types priv�s
 * =====================================================================================
 */

typedef enum{
	SCREEN_STATE_INIT,
    SCREEN_STATE_WAIT,
	SCREEN_STATE_GET_PRESSED_BUTTON
}screen_state_e;

/*
 * =====================================================================================
 * Prototypes priv�s
 * =====================================================================================
 */

void SCREEN_init(void);

void SCREEN_timerCallback();

/*
 * =====================================================================================
 * Variables priv�es
 * =====================================================================================
 */

static screen_action_e last_user_action = SCREEN_ACTION_NONE;
static screen_type_e current_display;
static uint16_t timer = 0;

/*
 * =====================================================================================
 * Fonctions publiques
 * =====================================================================================
 */

screen_action_e SCREEN_getLastUserAction(void){
	screen_action_e return_value = last_user_action;
	last_user_action = SCREEN_ACTION_NONE;
	return return_value;
}

/*
    Demande d'affichage du menu demand�
*/
void SCREEN_display(screen_type_e type, cube_s * cube){
	current_display = type;
	switch (type) {
		case SCREEN_MENU:
			ILI9341_Fill(ILI9341_COLOR_WHITE);
			ILI9341_DrawFilledRectangle(SCREEN_SHUFFLE_MIN_X, SCREEN_SHUFFLE_MIN_Y, SCREEN_SHUFFLE_MAX_X, SCREEN_SHUFFLE_MAX_Y, SCREEN_SHUFFLE_COLOR);
			ILI9341_DrawFilledRectangle(SCREEN_RESOLVE_MIN_X, SCREEN_RESOLVE_MIN_Y, SCREEN_RESOLVE_MAX_X, SCREEN_RESOLVE_MAX_Y, SCREEN_RESOLVE_COLOR);
			break;
		case SCREEN_SHUFFLING_MENU:
			ILI9341_Fill(ILI9341_COLOR_WHITE);
			ILI9341_DrawFilledRectangle(SCREEN_CLOSE_MIN_X, SCREEN_CLOSE_MIN_Y, SCREEN_CLOSE_MAX_X, SCREEN_CLOSE_MAX_Y, SCREEN_CLOSE_COLOR);
			ILI9341_DrawFilledRectangle(SCREEN_START_MIN_X, SCREEN_START_MIN_Y, SCREEN_START_MAX_X, SCREEN_START_MAX_Y, SCREEN_START_COLOR);
			break;
		case SCREEN_SHUFFLING:
			ILI9341_Fill(ILI9341_COLOR_WHITE);
			ILI9341_DrawFilledRectangle(SCREEN_CLOSE_MIN_X, SCREEN_CLOSE_MIN_Y, SCREEN_CLOSE_MAX_X, SCREEN_CLOSE_MAX_Y, SCREEN_CLOSE_COLOR);
			break;
		case SCREEN_RESOLVING_MENU:
			ILI9341_Fill(ILI9341_COLOR_WHITE);
			ILI9341_DrawFilledRectangle(SCREEN_CLOSE_MIN_X, SCREEN_CLOSE_MIN_Y, SCREEN_CLOSE_MAX_X, SCREEN_CLOSE_MAX_Y, SCREEN_CLOSE_COLOR);
			ILI9341_DrawFilledRectangle(SCREEN_START_MIN_X, SCREEN_START_MIN_Y, SCREEN_START_MAX_X, SCREEN_START_MAX_Y, SCREEN_START_COLOR);
			break;
		case SCREEN_MODELING:
			ILI9341_Fill(ILI9341_COLOR_WHITE);
			ILI9341_DrawFilledRectangle(SCREEN_CLOSE_MIN_X, SCREEN_CLOSE_MIN_Y, SCREEN_CLOSE_MAX_X, SCREEN_CLOSE_MAX_Y, SCREEN_CLOSE_COLOR);
			break;
		case SCREEN_RESOLVING:
			ILI9341_Fill(ILI9341_COLOR_WHITE);
			ILI9341_DrawFilledRectangle(SCREEN_CLOSE_MIN_X, SCREEN_CLOSE_MIN_Y, SCREEN_CLOSE_MAX_X, SCREEN_CLOSE_MAX_Y, SCREEN_CLOSE_COLOR);
			break;
		default:
			break;
	}
}


/*
    Machine � �tat
*/
void SCREEN_process(void){
	static screen_state_e 	state = SCREEN_STATE_INIT,
							previous_state = SCREEN_STATE_INIT;

	bool_e entrance = state != previous_state;
	previous_state = state;

	static int16_t x,y;

	switch (state) {
		case SCREEN_STATE_INIT:
			SCREEN_init();
			state = SCREEN_STATE_GET_PRESSED_BUTTON;
			break;
		case SCREEN_STATE_GET_PRESSED_BUTTON:
			if(XPT2046_getMedianCoordinates(&x, &y, XPT2046_COORDINATE_SCREEN_RELATIVE))
			{
				if(			SCREEN_SHUFFLE_MIN_X <= x && x <= SCREEN_SHUFFLE_MAX_X &&
							SCREEN_SHUFFLE_MIN_Y <= y && y <= SCREEN_SHUFFLE_MAX_Y){
					last_user_action = SCREEN_ACTION_SHUFFLE;
					state = SCREEN_STATE_WAIT;
				}else if(	SCREEN_RESOLVE_MIN_X <= x && x <= SCREEN_RESOLVE_MAX_X &&
							SCREEN_RESOLVE_MIN_Y <= y && y <= SCREEN_RESOLVE_MAX_Y){
					last_user_action = SCREEN_ACTION_RESOLVE;
					state = SCREEN_STATE_WAIT;
				}else if(	SCREEN_CLOSE_MIN_X <= x && x <= SCREEN_CLOSE_MAX_X &&
							SCREEN_CLOSE_MIN_Y <= y && y <= SCREEN_CLOSE_MAX_Y){
					last_user_action = SCREEN_ACTION_CLOSE;
					state = SCREEN_STATE_WAIT;
				}else if(	SCREEN_START_MIN_X <= x && x <= SCREEN_START_MAX_X &&
							SCREEN_START_MIN_Y <= y && y <= SCREEN_START_MAX_Y){
					last_user_action = SCREEN_ACTION_START;
					state = SCREEN_STATE_WAIT;
				}
			}
			break;
		case SCREEN_STATE_WAIT:
			if(entrance)
				timer = SCREEN_TIMER_BETWEEN_ACTIONS;
			if(!timer)
				state = SCREEN_STATE_GET_PRESSED_BUTTON;
		default:
			break;
	}
}

/*
 * =====================================================================================
 * Fonctions priv�es
 * =====================================================================================
 */

void SCREEN_init(void){
	Systick_add_callback_function(&SCREEN_timerCallback);

	ILI9341_Init();	//initialisation de l'�cran TFT
	ILI9341_Rotate(ILI9341_Orientation_Landscape_2);
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	XPT2046_init();	//initialisation du tactile
}

void SCREEN_timerCallback(){
	if(timer){
		timer--;
	}
}
