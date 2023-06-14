/*
 * cam.h
 *
 *  Created on: 1 avr. 2023
 *      Author: caill
 */

#ifndef CAM_H_
#define CAM_H_

#include "config.h"
#include "macro_types.h"
#include "cube_types.h"

#define CAM_MAX_BLOCKS 				15
#define CAM_ORIGIN_SQUARE_CENTER 	26
#define CAM_SPACES_BETWEEN_SQUARES 	65

typedef enum{
	CAM_LED_PARTIAL_ON,
	CAM_LED_FULL_ON,
	CAM_LED_OFF,
	CAM_FACE
}request_type_e;

void CAM_init(void);

bool_e CAM_isReady(void);

void CAM_askFor(request_type_e request);

void CAM_flush(void);

void CAM_getFace(block_type_e face[3][3]);

#endif /* CAM_H_ */
