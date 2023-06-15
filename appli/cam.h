/**
 * @file cam.h
 * @author armand
 * @date 01 avril 2023
 * @brief Header du module screen.
 * @version 0.1
 */

#ifndef CAM_H_
#define CAM_H_

#include "config.h"
#include "macro_types.h"
#include "cube_types.h"

/**
 * @def CAM_MAX_BLOCKS
 * @brief nombre de signatures maximales requetees a la Pixy
 */
#define CAM_MAX_BLOCKS 				15
/**
 * @def CAM_ORIGIN_SQUARE_CENTER
 * @brief Coordonnees du centre d'un carre d'une face ramenee vers l'origine
 */
#define CAM_ORIGIN_SQUARE_CENTER 	26
/**
 * @def CAM_ORIGIN_SQUARE_CENTER
 * @brief Ecart entre deux centres de carre d'une face en pixels
 */
#define CAM_SPACES_BETWEEN_SQUARES 	65

/**
 * @enum request_type_e
 * @brief Liste des requetes addressables a la Pixy2
 */
typedef enum{
	CAM_LED_PARTIAL_ON,	/*!< Allumer des deux LED supperieures */
	CAM_LED_FULL_ON,	/*!< Allumer des trois LED */
	CAM_LED_OFF,		/*!< Eteindre toutes les LED */
	CAM_FACE			/*!< Demander les signatures */
}request_type_e;

/**
 * @brief Initialise la communication UART avec la Pixy2 - non bloquant
 */
void CAM_init(void);

/**
 * @brief Informe de l'etat du buffer d'entree de la communication UART - non bloquant
 * @return TRUE si un octet est dans le buffer, FALSE si non
 */
bool_e CAM_isReady(void);

/**
 * @brief Envoie une trame de donnee a la Pixy2 - non bloquant
 * @param request Type de trame envoyee
 */
void CAM_askFor(request_type_e request);

/**
 * @brief Vide le buffer de la communication UART - non bloquant
 */
void CAM_flush(void);

/**
 * @brief Recupere les octets du buffer correspondant a la reponse d'une requete de signatures et la
 * convertie en une face du Rubik's cube - non bloquant
 * @param[out] face Face du cube percue par la Pixy2
 */
void CAM_getFace(block_type_e face[3][3]);

#endif /* CAM_H_ */
