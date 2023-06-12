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
 * Types privés
 * =====================================================================================
 */

typedef enum{
    SCREEN_STATE_WAIT,
	SCREEN_STATE_GET_PRESSED_BUTTON
}screen_state_e;

/*
 * =====================================================================================
 * Prototypes privés
 * =====================================================================================
 */

void SCREEN_init(void);


/*
 * =====================================================================================
 * Variables privées
 * =====================================================================================
 */

static screen_type_e screen_current_display;
static screen_type_e screen_previous_display;

/*
 * =====================================================================================
 * Fonctions publiques
 * =====================================================================================
 */

user_action_e SCREEN_getLastUserAction(void){
	//TODO
}

/*
    Demande d'affichage du menu demandé
*/
void SCREEN_display(screen_type_e type, cube_s * cube){
	//TODO
}

/*
    Machine à état
*/
void SCREEN_process(void){
	//TODO
}

/*
 * =====================================================================================
 * Fonctions privées
 * =====================================================================================
 */

void SCREEN_init(void){
	//TODO
}
