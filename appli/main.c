/**
  ******************************************************************************
  * @file    main.c
  * @author  Nirgal
  * @date    03-July-2019
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32f1xx_hal.h"
#include "stm32f1_uart.h"
#include "stm32f1_sys.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"
#include "systick.h"
#include "stm32f1_ili9341.h"
#include "cam.h"
#include "stm32f1_xpt2046.h"


void TFT_Init();

void writeLED(bool_e b)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, b);
}

bool_e readButton(void)
{
	return !HAL_GPIO_ReadPin(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN);
}

static volatile uint32_t t = 0;
void process_ms(void)
{
	if(t)
		t--;
}

void TFT_Init(){
	ILI9341_Init();
	ILI9341_Rotate(ILI9341_Orientation_Landscape_1);
	ILI9341_Fill(ILI9341_COLOR_WHITE);
	XPT2046_init();
}
int main(void)
{
	//Initialisation de la couche logicielle HAL (Hardware Abstraction Layer)
	//Cette ligne doit rester la première étape de la fonction main().
	HAL_Init();
	//Initialisation de l'UART2 à la vitesse de 115200 bauds/secondes (92kbits/s) PA2 : Tx  | PA3 : Rx.
		//Attention, les pins PA2 et PA3 ne sont pas reliées jusqu'au connecteur de la Nucleo.
		//Ces broches sont redirigées vers la sonde de débogage, la liaison UART étant ensuite encapsulée sur l'USB vers le PC de développement.
	UART_init(UART2_ID,19200);
#if CAM_UART1
		UART_init(UART1_ID,19200);
#endif

	//TFT_Init();
	//"Indique que les printf sortent vers le périphérique UART2."
//	SYS_set_std_usart(UART2_ID, UART2_ID, UART2_ID);

	//Initialisation du port de la led Verte (carte Nucleo)
	BSP_GPIO_PinCfg(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);

	//Initialisation du port du bouton bleu (carte Nucleo)
	BSP_GPIO_PinCfg(BLUE_BUTTON_GPIO, BLUE_BUTTON_PIN, GPIO_MODE_INPUT,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH);

	//On ajoute la fonction process_ms à la liste des fonctions appelées automatiquement chaque ms par la routine d'interruption du périphérique SYSTICK
	Systick_add_callback_function(&process_ms);

	blocks_received_s blocks_received;

	bool_e previous_press = FALSE;
	bool_e current_press = FALSE;
	bool_e pressed = FALSE;
	//ILI9341_demo();
	XPT2046_init();
	while(1)	//boucle de tâche de fond
	{
		XPT2046_demo();
//		ILI9341_demo();
//		current_press = readButton();
//		pressed = current_press && !previous_press;
//		previous_press = current_press;
//
//		if(pressed){
//			CAM_get_blocks(&blocks_received);
//			current_press = current_press;

//		}

	}
}
