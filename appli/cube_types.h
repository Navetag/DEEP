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
 * Chaque �l�ment de cette �num�ration doit prendre la valeur de la signature
 * d�finie sur la pixycam.
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
	face_t up;
	face_t down;
	face_t front;
	face_t back;
	face_t left;
	face_t right;
}cube_s;

#endif /* CUBE_TYPES_H_ */
