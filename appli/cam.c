/*
 * cam.c
 *
 *  Created on: 1 avr. 2023
 *      Author: caill
 */


#include "cam.h"

blocks_received_s CAM_get_blocks(){
	/*
	 * Trame envoyée à la pixycam :
	 * 174,193 : 166-bit sync
	 * 31 : type du paquet
	 * 2 : taille du payload
	 * 255 : recevoir toutes les signatures
	 * MAX_BLOCKS : nombre de block maximal que l'on souhaite recevoir
	 */
//	uint8_t packet[6] = {174,193,32,2,255,MAX_BLOCKS};
	UART_puts(UART2_ID, (uint8_t[]){174,193,32,2,255,MAX_BLOCKS}, 6);

	HAL_Delay(100);

	//Reception de la réponse supposément reçue 100 ms plus tard
	uint8_t *buf = (uint8_t*)malloc(6+14*MAX_BLOCKS);
	uint8_t c;
	uint16_t buf_index = 0;
	while(UART_data_ready(UART2_ID) && buf_index < 6+14*MAX_BLOCKS)
	{
		c = UART_getc(UART2_ID);
		buf[buf_index] = c;
		buf_index++;
	}

	blocks_received_s blocks_received;

	//Le 4eme octet de la réponse indique la longueur du payload
	blocks_received.nb_blocks_received = buf[3]/14;
	if(blocks_received.nb_blocks_received > MAX_BLOCKS){
		//Condition normalement impossible, simple mesure de précaution
		blocks_received.nb_blocks_received = MAX_BLOCKS;
	}

	/*
	 * On parcours chacun des blocks de la trame de réponse pour les sauvegarder dans la structure
	 * Chaque block faisant une longueur de 14 octets et le premier commencant au 7eme octet
	 */
	buf_index = 6;
	for(uint8_t i = 0 ; i < blocks_received.nb_blocks_received; i++, buf_index += 14){
		block_s block;
		block.signature = (block_type_e)buf[buf_index];
		block.center_x = buf[buf_index+2] | (buf[buf_index+3] << 8);
		block.center_y = buf[buf_index+4];
		block.width = buf[buf_index+6] | (buf[buf_index+7] << 8);
		block.height = buf[buf_index+8];
		blocks_received.blocks[i] = block;
	}

	free(buf);

	return blocks_received;
}
