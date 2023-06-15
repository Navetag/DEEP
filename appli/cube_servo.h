/**
 * @file cube_servo.h
 * @author evan
 * @date 01 avril 2023
 * @brief Header du module screen.
 * @version 0.1
 */

#ifndef CUBE_SERVO_H_
#define CUBE_SERVO_H_

#include "macro_types.h"
#include "config.h"

/**
 * @def CUBE_SERVO_TIME_WAIT_STATE_MS
 * @brief Temps d'attente dans l'etat "WAIT" (en ms)
 */
#define CUBE_SERVO_TIME_WAIT_STATE_MS 	2000

/**
 * @def CUBE_SERVO_PERIOD_TIMER
 * @brief Periode du timer de pilotage des servo-moteurs (en ms)
 */
#define CUBE_SERVO_PERIOD_TIMER			10

/**
 * @def CUBE_SERVO_MAX_REVERSE_POS_VAL
 * @brief Rapport cyclique correspondant a la position anti-horraire
 */
#define CUBE_SERVO_MAX_REVERSE_POS_VAL			10

/**
 * @def CUBE_SERVO_MAX_REVERSE_POS_VAL_PLUS
 * @brief Rapport cyclique correspondant a la position legerement plus loins que anti-horraire
 */
#define CUBE_SERVO_MAX_REVERSE_POS_VAL_PLUS		0

/**
 * @def CUBE_SERVO_MAX_CLKWISE_POS_VAL
 * @brief Rapport cyclique correspondant a la position horaire
 * @deprecated Seule une amplitude de 90 degres est utilise
 */
#define CUBE_SERVO_MAX_CLKWISE_POS_VAL			500

/**
 * @def CUBE_SERVO_MIDDLE_POS_VAL
 * @brief Rapport cyclique correspondant a la position par defaut
 */
#define CUBE_SERVO_MIDDLE_POS_VAL				115

/**
 * @def CUBE_SERVO_MIDDLE_POS_VAL_PLUS
 * @brief Rapport cyclique correspondant a la position par defaut legerement plus loin
 */
#define CUBE_SERVO_MIDDLE_POS_VAL_PLUS			130

/**
 * @def CUBE_SERVO_FLIP_UP_POS_VAL
 * @brief Rapport cyclique correspondant a la position de cage faisant pivoter le cube
 */
#define CUBE_SERVO_FLIP_UP_POS_VAL		0

/**
 * @def CUBE_SERVO_DEFAULT_CAGE_POS_VAL
 * @brief Rapport cyclique correspondant a la position de cage par defaut
 */
#define CUBE_SERVO_DEFAULT_CAGE_POS_VAL	45

/**
 * @def CUBE_SERVO_HOLD_CUBE_POS_VAL
 * @brief Rapport cyclique correspondant a la position de cage bloquant les deux rangées supérieures du cube
 */
#define CUBE_SERVO_HOLD_CUBE_POS_VAL	80

/**
 * @enum cube_servo_complex_mvt_e
 * @brief Mouvements complexes de servo-moteurs
 */
typedef enum{
    CUBE_SERVO_BOTTOM_90_A,			/*!< Face basse 90 degre anti-horraire*/
	CUBE_SERVO_BOTTOM_180,			/*!< Face basse 180 degre*/
	CUBE_SERVO_BOTTOM_90,			/*!< Face basse 90 degre horraire*/

	CUBE_SERVO_TOP_90_A,			/*!< Face haute 90 degre anti-horraire*/
	CUBE_SERVO_TOP_180,				/*!< Face haute 180 degre*/
	CUBE_SERVO_TOP_90,				/*!< Face haute 90 degre horraire*/

	CUBE_SERVO_FRONT_90_A,			/*!< Face avant 90 degre anti-horraire*/
	CUBE_SERVO_FRONT_180,			/*!< Face avant 180 degre*/
	CUBE_SERVO_FRONT_90,			/*!< Face avant 90 degre horraire*/

	CUBE_SERVO_BACK_90_A,			/*!< Face arriere 90 degre anti-horraire*/
	CUBE_SERVO_BACK_180,			/*!< Face arriere 180 degre*/
	CUBE_SERVO_BACK_90,				/*!< Face arriere 90 degre horraire*/

	CUBE_SERVO_LEFT_90_A,			/*!< Face gauche 90 degre anti-horraire*/
	CUBE_SERVO_LEFT_180,			/*!< Face gauche 180 degre*/
	CUBE_SERVO_LEFT_90,				/*!< Face gauche 90 degre horraire*/

	CUBE_SERVO_RIGHT_90_A,			/*!< Face droite 90 degre anti-horraire*/
	CUBE_SERVO_RIGHT_180,			/*!< Face droite 180 degre*/
	CUBE_SERVO_RIGHT_90,			/*!< Face droite 90 degre horraire*/

	CUBE_SERVO_GAUCHE_90,			/*!< Mouvement primaire : rotation 90 degre a gauche (anti-horraire)*/
	CUBE_SERVO_DROITE_90, 			/*!< Mouvement primaire : rotation 90 degre a droite (horraire) @deprecated Seule une amplitude de 90 degre est utilisee*/
	CUBE_SERVO_FLIP,				/*!< Mouvement primaire : cage en position de pivotage*/

	CUBE_SERVO_DEFAULT_POS,			/*!< Positions par defauts (cage puis rotation)*/
	CUBE_SERVO_DEFAULT_ROT_COMPLEX,	/*!< Mouvement primaire : rotation par defaut*/
	CUBE_SERVO_DEFAULT_CAGE_COMPLEX /*!< Mouvement primaire : cage par defaut*/
}cube_servo_complex_mvt_e;

/**
 * @enum cube_servo_state_e
 * @brief Etat du process du module
 */
typedef enum{
	CUBE_SERVO_INIT,		/*!< Initialisation du module*/
	CUBE_SERVO_WAIT_SERVO,	/*!< Pause entre chaque mouvement de servo-moteur*/
	CUBE_SERVO_MAKE,		/*!< Application d'un mouvement de servo-moteur*/
	CUBE_SERVO_FINISHED		/*!< Mouvements des servo-moteurs finis*/
}cube_servo_state_e;

/**
 * @brief Machine a etat du process du module - non bloquant :
 * 		- Gere l'application d'une sequence de mouvements primaires stockes dans une pile FIFO aux servo-moteurs
 * 		- Se positionne en etat "FINISHED" quand les servo-moteurs ont fini leurs courses et que la pile est vide
 * 		- Se charge de l'initialisation et doit etre appelle en boucle
 */
void CUBE_SERVO_process(void);

/**
 * @brief Ajoute un momuvement complexe a la pile de mouvements - non bloquant
 * 		- Le mouvement est decompose en une sequence de mouvements primaires
 * @param mvt Le mouvement complexe a empiler
 */
void CUBE_SERVO_addMvt(cube_servo_complex_mvt_e mvt);

/**
 * @brief Retourne l'etat courant de la machine a etat du process - non bloquant
 * @return L'etat courant de la machine a etat du process
 */
cube_servo_state_e CUBE_SERVO_getState(void);

/**
 * @brief Vide la pile de mouvements - non bloquant
 */
void CUBE_SERVO_flush(void);

#if MAIN_TEST

/**
 * @brief Fonction de test du module - bloquant
 */
void CUBE_SERVO_test(void);

#endif

#endif /* CUBE_SERVO_H_ */
