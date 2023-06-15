/**
 * @file cube_types.h
 * @author armand
 * @date 01 avril 2023
 * @brief Header du module screen.
 * @version 0.1
 */
#ifndef CUBE_TYPES_H_
#define CUBE_TYPES_H_

#include "macro_types.h"
#include "stdbool.h"

/**
 * @enum block_type_e
 * @brief Type de signature / carre d'une face
 *
 * Chaque element doit prendre la valeur de la signature de la Pixy2 associée
 */
typedef enum{
	CUBE_NONE = 0,	/*!< Type par defaut*/
	CUBE_RED = 1,	/*!< Type couleur rouge*/
	CUBE_ORANGE = 2,/*!< Type couleur orange*/
	CUBE_YELLOW = 3,/*!< Type couleur jaune*/
	CUBE_GREEN = 4,	/*!< Type couleur verte*/
	CUBE_BLUE = 5,	/*!< Type couleur bleue*/
	CUBE_WHITE = 6	/*!< Type couleur blanche*/
}block_type_e;

/**
 * @struct cube_s
 * @brief Modele d'un Rubik's cube
 */
typedef struct{
	block_type_e up[3][3];		/*!< Face haute*/
	block_type_e down[3][3];	/*!< Face basse*/
	block_type_e front[3][3];	/*!< Face avant*/
	block_type_e back[3][3];	/*!< Face arriere*/
	block_type_e left[3][3];	/*!< Face gauche*/
	block_type_e right[3][3];	/*!< face droite*/
}cube_s;

#endif /* CUBE_TYPES_H_ */
