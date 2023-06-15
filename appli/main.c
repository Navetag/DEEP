/**
 * @file main.c
 * @author armand
 * @date 01 avril 2023
 * @brief Fonction main par defaut
 * @version 0.1
 */

#include <vincent_colin.h>
#include "stm32f1xx_hal.h"

/**
 * @fn int main(void)
 * @brief Fonction executee au lancement
 * 	- Boucle infinie executant :
 * 		- Si la macro de test est activee, les fonctions de test renseignees
 * 		- Si la macro de test n'est pas activee, le process du module "vincent_colin"
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
