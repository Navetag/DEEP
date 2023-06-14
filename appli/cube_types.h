/*
 * cube_types.h
 *
 *  Created on: 9 juin 2023
 *      Author: caill
 */

#ifndef CUBE_TYPES_H_
#define CUBE_TYPES_H_

#include "macro_types.h"
#include "stdbool.h"

/*
 * Chaque élément de cette énumération doit prendre la valeur de la signature
 * définie sur la pixycam.
 */
typedef enum{
	CUBE_NONE = 0,
	CUBE_RED = 1,
	CUBE_ORANGE = 2,
	CUBE_YELLOW = 3,
	CUBE_GREEN = 4,
	CUBE_BLUE = 5,
	CUBE_WHITE = 6
}block_type_e;

typedef block_type_e face_t[3][3];

typedef struct{
	block_type_e up[3][3];
	block_type_e down[3][3];
	block_type_e front[3][3];
	block_type_e back[3][3];
	block_type_e left[3][3];
	block_type_e right[3][3];
}cube_s;

#endif /* CUBE_TYPES_H_ */
