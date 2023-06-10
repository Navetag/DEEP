/*
 * screen.h
 *
 *  Created on: 10 juin 2023
 *      Author: auger
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include "cube_types.h"

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
	SCREEN_ACTION_SOLVE
}user_action_e;

/*
    R�cup�ration de l'action effectu�e via le tactile de l'�cran
*/
user_action_e SCREEN_getLastUserAction(void);

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
