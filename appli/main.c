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
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "systick.h"
#include "stm32f1_ili9341.h"
#include "stm32f1_xpt2046.h"
#include "config.h"

//void TFT_Init();
//
//void writeLED(bool_e b)
//{
//	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
//}

//bool_e readButton(void)
//{
//	return !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
//}

//static volatile uint32_t t = 0;
//void process_ms(void)
//{
//	if(t)
//		t--;
//}

//void TFT_Init(){
//	ILI9341_Init();
//	ILI9341_Rotate(ILI9341_Orientation_Landscape_1);
//	ILI9341_Fill(ILI9341_COLOR_WHITE);
//	XPT2046_init();
//}
int main(void)
{
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	//Cette ligne doit rester la première étape de la fonction main().
	HAL_Init();



	//TFT_Init();
	//"Indique que les printf sortent vers le périphérique UART2."
//	SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	//Initialisation du port de la led Verte (carte Nucleo)
//	BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
//	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//On ajoute la fonction process_ms à la liste des fonctions appelées automatiquement chaque ms par la routine d'interruption du périphérique SYSTICK
//	Systick_add_callback_function(&process_ms);

	while(1)	//boucle de tâche de fond
	{
		/*
		 * TODO
		 * EVAN
		 * - Petites corrections cube-servo.c
		 * - Mouvements -> cube en pos initiale ?
		 * - Nouveaux mouvements dispo : flip down, rot 0 etc
		 *
		 * ARMAND
		 * - Elaboration tests des modules
		 * 		- R�cup�ration d'une face pixy : servo pos default + led ON + get Face
		 *		- Mod�lisation du cube : toute la mod�lisation (cube_s complete)
		 *		- Fonctions bloquantes ?
		 *		- Machine a �tat simple de vincent
		 */
#if SERVO_TEST
		CUBE_SERVO_test();
#endif
#if MAIN_TEST
		// Tests � effectuer

#else
		// Comportement de production

		VINCENT_process();
#endif
	}
}
