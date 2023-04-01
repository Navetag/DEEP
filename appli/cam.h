/*
 * cam.h
 *
 *  Created on: 1 avr. 2023
 *      Author: caill
 */

#ifndef CAM_H_
#define CAM_H_

#define MAX_BLOCKS 15

#include "config.h"
#include "stm32f1_uart.h"
#include <stdlib.h>


/*
 * Chaque élément de cette énumération doit prendre la valeur de la signature
 * définie sur la pixycam correspondante.
 */
typedef enum{
	RED = 1,
	ORANGE = 2,
	YELLOW = 3,
	GREEN = 4,
	BLUE = 5,
	WHITE = 6
}block_type_e;

typedef struct{
	block_type_e signature;
	uint16_t center_x;
	uint16_t center_y;
	uint16_t width;
	uint16_t height;
}block_s;

typedef struct{
	block_s blocks[MAX_BLOCKS];
	uint8_t nb_blocks_received;
}blocks_received_s;

blocks_received_s CAM_get_blocks();

#endif /* CAM_H_ */
