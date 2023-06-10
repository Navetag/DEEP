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
#include "macro_types.h"
#include "cube_types.h"



/*
 * Chaque élément de cette énumération doit prendre la valeur de la signature
 * définie sur la pixycam correspondante.
 */


typedef enum{
	CAM_LED_PARTIAL_ON,
	CAM_LED_FULL_ON,
	CAM_LED_OFF,
	CAM_FACE
}request_type_e;

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




void CAM_init();

bool_e CAM_isReady();

void CAM_askFor(request_type_e request);

void CAM_flush();

void CAM_getFace(face_t face);

#endif /* CAM_H_ */
