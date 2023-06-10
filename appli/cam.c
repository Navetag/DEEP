/*
 * cam.c
 *
 *  Created on: 1 avr. 2023
 *      Author: caill
 */

#include "cam.h"
#include "stm32f1_uart.h"

/*
 * =====================================================================================
 * Types privés
 * =====================================================================================
 */

typedef struct{
	block_type_e signature;
	uint16_t center_x;
	uint16_t center_y;
	uint16_t width;
	uint16_t height;
}block_s;

typedef struct{
	block_s blocks[CAM_MAX_BLOCKS];
	uint8_t nb_blocks_received;
}blocks_received_s;

/*
 * =====================================================================================
 * Prototypes privés
 * =====================================================================================
 */

void CAM_cleanFace(face_t face);

void CAM_getBlocksReceived(blocks_received_s *blocks_received);

/*
 * =====================================================================================
 * Fonctions publiques
 * =====================================================================================
 */

void CAM_init(){
	UART_init(UART2_ID,19200);
}

bool_e CAM_isReady(){
	return UART_data_ready(UART2_ID);
}

void CAM_askFor(request_type_e request){

	/*
	 * Trames envoyées à la pixycam :
	 *
	 * 174,193 : 16-bit sync
	 * 32 : type du paquet : signatures
	 * 2 : taille du payload
	 * 255 : recevoir toutes les signatures
	 * CAM_MAX_BLOCKS : nombre de block maximal que l'on souhaite recevoir
	 *
	 * 174,193 : 16-bit sync
	 * 22 : type du paquet : commande des LED
	 * 2 : taille du payload
	 * 1/0 : LED superieures ON / OFF
	 * 1/0 : LED centrale ON / OFF
	 */

	if(request == CAM_LED_PARTIAL_ON){
		uint8_t packet[6] = {174,193,22,2,1,0};
		UART_puts(UART2_ID, packet, 6);
	}else if(request == CAM_LED_FULL_ON){
		uint8_t packet[6] = {174,193,22,2,1,1};
		UART_puts(UART2_ID, packet, 6);
	}else if(request == CAM_LED_OFF){
		uint8_t packet[6] = {174,193,22,2,0,0};
		UART_puts(UART2_ID, packet, 6);
	}else if(request == CAM_FACE){
		uint8_t packet[6] = {174,193,32,2,255,CAM_MAX_BLOCKS};
		UART_puts(UART2_ID, packet, 6);
	}
}

void CAM_flush(){
	while(UART_data_ready(UART2_ID)){
		UART_getc(UART2_ID);
	}
}

void CAM_getFace(face_t face){

	CAM_cleanFace(face);

	blocks_received_s blocks_received;
	CAM_getBlocksReceived(&blocks_received);

	if(!blocks_received.nb_blocks_received) return;

	//Toutes les signatures sont ramenées vers le centre de manière relative

	int16_t min_y = -1,
			min_x = -1;

	for(uint8_t i = 0; i < blocks_received.nb_blocks_received; i++){
		uint16_t y = blocks_received.blocks[i].center_y - (uint16_t)(blocks_received.blocks[i].width / 2);
		uint16_t x = blocks_received.blocks[i].center_x - (uint16_t)(blocks_received.blocks[i].height / 2);
		if(min_x == -1 || x < min_x) min_x = x;
		if(min_y == -1 || y < min_y) min_y = y;
	}

	for(uint8_t i = 0; i < blocks_received.nb_blocks_received; i++){
		blocks_received.blocks[i].center_x -= min_x;
		blocks_received.blocks[i].center_y -= min_y;
	}

	//Le centre du premier carré de la face aux coordonnées CAM_ORIGIN_SQUARE_CENTERxCAM_ORIGIN_SQUARE_CENTER pixels
	//Chaqu'un des centres de carré sont séparés par CAM_SPACES_BETWEEN_SQUARES pixels

	int16_t max_y,
			max_x,
			square_center_y,
			square_center_x;

	for(uint8_t y = 0; y < 3; y++){
		for(uint8_t x = 0; x < 3; x++){
			for(uint8_t i = 0; i < blocks_received.nb_blocks_received; i++){

				square_center_y = CAM_ORIGIN_SQUARE_CENTER + y*CAM_SPACES_BETWEEN_SQUARES;
				square_center_x = CAM_ORIGIN_SQUARE_CENTER + y*CAM_SPACES_BETWEEN_SQUARES;

				min_y = blocks_received.blocks[i].center_y - (uint16_t)(blocks_received.blocks[i].width / 2);
				max_y = min_y + blocks_received.blocks[i].width;
				min_x = blocks_received.blocks[i].center_x - (uint16_t)(blocks_received.blocks[i].height / 2);
				max_x = min_x + blocks_received.blocks[i].height;

				//Si, pour chaque carré, son centre est compris dans une signature perçue, il prend sa couleur.
				//La face est initialisé blanche et le blanc n'est pas détécté.
				if(	min_y <= square_center_y && square_center_y <= max_y &&
					min_x <= square_center_x && square_center_x <= max_x){
					face[y][x] = blocks_received.blocks[i].signature;
					break;
				}
			}
		}
	}


}

/*
 * =====================================================================================
 * Fonctions privées
 * =====================================================================================
 */

void CAM_cleanFace(face_t face){
	for(uint8_t i = 0; i < 3; i++){
		for(uint8_t j = 1; j < 3; j++){
			face[i][j] = CUBE_WHITE;
		}
	}
}


void CAM_getBlocksReceived(blocks_received_s *blocks_received){
	//Reception de la réponse supposément reçue
		uint8_t buf[6+14*CAM_MAX_BLOCKS];
		uint16_t buf_index = 0;

		while(UART_data_ready(UART2_ID) && buf_index < 6+14*CAM_MAX_BLOCKS)
		{
			buf[buf_index] = UART_getc(UART2_ID);
			buf_index++;
		}

		//Le 4eme octet de la réponse indique la longueur du payload
		blocks_received->nb_blocks_received = buf[3]/14;
		if(blocks_received->nb_blocks_received > CAM_MAX_BLOCKS){
			//Condition normalement impossible, simple mesure de précaution
			blocks_received->nb_blocks_received = CAM_MAX_BLOCKS;
		}

		/*
		 * On parcours chacun des blocks de la trame de réponse pour les sauvegarder dans la structure
		 * Chaque block faisant une longueur de 14 octets et le premier commencant au 7eme octet
		 */
		buf_index = 6;
		for(uint8_t i = 0 ; i < blocks_received->nb_blocks_received; i++, buf_index += 14){
			blocks_received->blocks[i].signature = (block_type_e)buf[buf_index];
			blocks_received->blocks[i].center_x = buf[buf_index+2] | (buf[buf_index+3] << 8);
			blocks_received->blocks[i].center_y = buf[buf_index+4];
			blocks_received->blocks[i].width = buf[buf_index+6] | (buf[buf_index+7] << 8);
			blocks_received->blocks[i].height = buf[buf_index+8];
		}
}
