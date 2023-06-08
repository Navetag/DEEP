#include "config.h"

/*
    Enumération de l'écran demandé
*/
typedef enum{
    MENU,
    SOLVING_MENU,
    SOLVING,
    SCANNING,
    SHUFFLING,
    SHUFFLING_MENU
}screen_type_e;

/*
    Enumération de l'action récupérée par le tactile
*/
typedef enum{
    NONE,
    CLOSE,
    START,
    SHUFFLE,
    SOLVE
}user_action_e;

typedef enum{
    WAIT,
    GET_PRESSED_BUTTON
}screen_state_e;


/*
    Récupération de l'action effectuée via le tactile de l'écran
*/
user_action_e screen_getLastUserAction(void);

/*
    Demande d'affichage du menu demandé
*/
void screen_display(screen_type_e, cube_s * cube);

/*
    Machine à état
*/
void screen_process(void);


void screen_init(void);


