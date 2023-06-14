/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
  * @brief   Default main function.
  ******************************************************************************
*/

#include <vincent_colin.h>
#include "stm32f1xx_hal.h"

/*
 * @brief	Initialise l'USARTx - 8N1 - vitesse des bits (baudrate) indiqu� en param�tre
 * @func	void UART_init(uint8_t uart_id, uart_interrupt_mode_e mode)
 * @param	uart_id est le num�ro de l'UART concern� :
 * 				UART1_ID
 * 				UART2_ID
 * 				UART3_ID
 * @param	baudrate indique la vitesse en baud/sec
 * 				115200	vitesse propos�e par d�faut
 * 				9600	vitesse couramment utilis�e
 * 				19200	...
 * @post	Cette fonction initialise les broches suivante selon l'USART choisit en parametre :
 * 				USART1 : Rx=PA10 et Tx=PA9 		ou avec remap : Rx=PB7 et Tx=PB6
 * 				USART2 : Rx=PA3 et Tx=PA2 		ou avec remap : Rx=PD6 et Tx=PD5
 * 				USART3 : Rx=PB11 et Tx=PB10 	ou avec remap : Rx=PD9 et Tx=PD8
 * 				La gestion des envois et reception se fait en interruption.
 */
int main(void)
{
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	//Cette ligne doit rester la premiere etape de la fonction main().
	HAL_Init();

	while(1){

#if MAIN_TEST
		// Fonction(s) de test a executer

#else
		// Comportement de production
		VINCENT_process();

#endif

	}
}
