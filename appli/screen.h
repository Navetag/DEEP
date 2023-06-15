/**
 * @file screen.h
 * @author armand
 * @date 01 avril 2023
 * @brief Header du module screen.
 * @version 0.1
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#include "cube_types.h"
#include "config.h"
#include <stm32f1_ili9341.h>

/**
 * @def SCREEN_TIMER_BETWEEN_ACTIONS
 * @brief Temps d'attente entre la detection de chaque action utilisateur en ms
 */
#define SCREEN_TIMER_BETWEEN_ACTIONS 500

//Caracteristiques de l'action "Close"
#define SCREEN_CLOSE_MIN_X 10
#define SCREEN_CLOSE_MAX_X 59
#define SCREEN_CLOSE_MIN_Y 10
#define SCREEN_CLOSE_MAX_Y 59
#define SCREEN_CLOSE_COLOR ILI9341_COLOR_RED

//Caracteristiques de l'action "Start"
#define SCREEN_START_MIN_X 100
#define SCREEN_START_MAX_X 219
#define SCREEN_START_MIN_Y 150
#define SCREEN_START_MAX_Y 229
#define SCREEN_START_COLOR ILI9341_COLOR_GREEN

//Caracteristiques de l'action "Shuffle"
#define SCREEN_SHUFFLE_MIN_X 40
#define SCREEN_SHUFFLE_MAX_X 139
#define SCREEN_SHUFFLE_MIN_Y 100
#define SCREEN_SHUFFLE_MAX_Y 199
#define SCREEN_SHUFFLE_COLOR ILI9341_COLOR_BLUE

//Caracteristiques de l'action "Resolve"
#define SCREEN_RESOLVE_MIN_X 180
#define SCREEN_RESOLVE_MAX_X 279
#define SCREEN_RESOLVE_MIN_Y 100
#define SCREEN_RESOLVE_MAX_Y 199
#define SCREEN_RESOLVE_COLOR ILI9341_COLOR_ORANGE

/**
 * @enum screen_type_e
 * @brief Types d'affichage de l'ihm
 */
typedef enum{
    SCREEN_MENU,			/*!< Menu : choix de melange ou de resolution*/
	SCREEN_RESOLVING_MENU,	/*!< Menu de resolution : choix de validation ou d'arret et instructions*/
	SCREEN_RESOLVING,		/*!< Resolution : choix d'arret et informations*/
	SCREEN_MODELING,		/*!< Modelisation : choix d'arret et patron du cube*/
	SCREEN_SHUFFLING,		/*!< Menu de resolution : choix d'arret et informations*/
	SCREEN_SHUFFLING_MENU	/*!< Menu de resolution : choix de validation ou d'arret et instructions*/
}screen_type_e;

/**
 * @enum screen_action_e
 * @brief Types d'actions de l'utilisateur captees
 */
typedef enum{
	SCREEN_ACTION_NONE,		/*!< Aucune action captee*/
	SCREEN_ACTION_CLOSE,	/*!< Choix d'arret*/
	SCREEN_ACTION_START,	/*!< Choix de validation*/
	SCREEN_ACTION_SHUFFLE,	/*!< Choix de melange*/
	SCREEN_ACTION_RESOLVE	/*!< Choix de resolution*/
}screen_action_e;

/**
 * @brief Fourni la derniere action utilisateur percue - non bloquant
 * @return L'action captee
 */
screen_action_e SCREEN_getLastUserAction(void);

/**
 * @brief Met a jour l'affichage de l'ecran - non bloquant
 * @param type Type d'ecran a afficher
 * @param[in] cube Structure du cube dont le patron doit etre affiche (Uniquement pour le type "SCREEN_MODELING")
 */
void SCREEN_display(screen_type_e type, cube_s * cube);

/**
 * @brief Machine a etat du process du module - non bloquant
 */
void SCREEN_process(void);

#endif /* SCREEN_H_ */
