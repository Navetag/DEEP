/*
 * screen.c
 *
 *  Created on: 10 juin 2023
 *      Author: auger
 */

#include "screen.h"
#include "config.h"

/*
 * =====================================================================================
 * Types priv�s
 * =====================================================================================
 */

typedef enum{
    SCREEN_STATE_WAIT,
	SCREEN_STATE_GET_PRESSED_BUTTON
}screen_state_e;

/*
 * =====================================================================================
 * Prototypes priv�s
 * =====================================================================================
 */

void SCREEN_init(void);

/*
 * =====================================================================================
 * Fonctions publiques
 * =====================================================================================
 */

user_action_e SCREEN_getLastUserAction(void){
	//TODO
}

/*
    Demande d'affichage du menu demand�
*/
void SCREEN_display(screen_type_e type, cube_s * cube){
	//TODO
}

/*
    Machine � �tat
*/
void SCREEN_process(void){
	//TODO
}

/*
 * =====================================================================================
 * Fonctions priv�es
 * =====================================================================================
 */

void SCREEN_init(void){
	//TODO
}
