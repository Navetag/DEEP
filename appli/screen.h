/*
 * screen.h
 *
 *  Created on: 10 juin 2023
 *      Author: auger
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include "cube_types.h"
#include "config.h"
#include <stm32f1_ili9341.h>

#define SCREEN_TIMER_BETWEEN_ACTIONS 500 //ms

#define SCREEN_CLOSE_MIN_X 10
#define SCREEN_CLOSE_MAX_X 59
#define SCREEN_CLOSE_MIN_Y 10
#define SCREEN_CLOSE_MAX_Y 59
#define SCREEN_CLOSE_COLOR ILI9341_COLOR_RED

#define SCREEN_START_MIN_X 100
#define SCREEN_START_MAX_X 219
#define SCREEN_START_MIN_Y 150
#define SCREEN_START_MAX_Y 229
#define SCREEN_START_COLOR ILI9341_COLOR_GREEN

#define SCREEN_SHUFFLE_MIN_X 40
#define SCREEN_SHUFFLE_MAX_X 139
#define SCREEN_SHUFFLE_MIN_Y 100
#define SCREEN_SHUFFLE_MAX_Y 199
#define SCREEN_SHUFFLE_COLOR ILI9341_COLOR_BLUE

#define SCREEN_RESOLVE_MIN_X 40
#define SCREEN_RESOLVE_MAX_X 139
#define SCREEN_RESOLVE_MIN_Y 100
#define SCREEN_RESOLVE_MAX_Y 199
#define SCREEN_RESOLVE_COLOR ILI9341_COLOR_ORANGE
/*
    Enum�ration de l'�cran demand�
*/
typedef enum{
    SCREEN_MENU,
	SCREEN_RESOLVING_MENU,
	SCREEN_RESOLVING,
	SCREEN_MODELING,
	SCREEN_SHUFFLING,
	SCREEN_SHUFFLING_MENU
}screen_type_e;

/*
    Enum�ration de l'action r�cup�r�e par le tactile
*/
typedef enum{
	SCREEN_ACTION_NONE,
	SCREEN_ACTION_CLOSE,
	SCREEN_ACTION_START,
	SCREEN_ACTION_SHUFFLE,
	SCREEN_ACTION_RESOLVE
}screen_action_e;

/*
    R�cup�ration de l'action effectu�e via le tactile de l'�cran
*/
screen_action_e SCREEN_getLastUserAction(void);

/*
    Demande d'affichage du menu demand�
    Si le type est "SCREEN_MODELING" il est attendu que le second param�tre ne soit pas un pointeur null.
*/
void SCREEN_display(screen_type_e type, cube_s * cube);

/*
    Machine � �tat
*/
void SCREEN_process(void);

#endif /* SCREEN_H_ */
