/**
 * @file cube_servo.c
 * @author evan
 * @date 01 avril 2023
 * @brief Sources du module screen.
 * @version 0.1
 */

#include "systick.h"
#include "stm32f1_timer.h"
#include "cube_servo.h"

/*
 * =====================================================================================
 * Types priv�s
 * =====================================================================================
 */

/**
 * @enum cube_servo_primary_mvt_e
 * @brief Mouvements primaires de servo-moteurs
 */
typedef enum{
	CUBE_SERVO_MAX_CLKWISE,		/*!< Position 90 degre horraire @deprecated Seule une amplitude de 90 degre est utilisee*/
	CUBE_SERVO_MAX_REVERSE,		/*!< Position 90 degre anti-horraire */
	CUBE_SERVO_MAX_REVERSE_PLUS,/*!< Position 90 degre anti-horraire plus loins */
	CUBE_SERVO_MIDDLE_POS,		/*!< Position 90 degre par defaut */
	CUBE_SERVO_MIDDLE_POS_PLUS,	/*!< Position 90 degre par defaut plus loins*/

	CUBE_SERVO_HOLD_CUBE,		/*!< Position de blocage du cube (les deux rangees supperieures)*/
	CUBE_SERVO_FLIP_UP,			/*!< Position de pivotage du cube*/
	CUBE_SERVO_DEFAULT_CAGE		/*!< Position par defaut de la cage (Le cube peut tourner librement)*/

}cube_servo_primary_mvt_e;

/**
 * @enum cube_servo_rot_pos_e
 * @brief Positions concretes du servo de rotation du cube
 */
typedef enum{
	CUBE_SERVO_POS_CENTER,	/*!< Position par defaut*/
	CUBE_SERVO_POS_LEFT,	/*!< Position gauche (anti-horraire)*/
	CUBE_SERVO_POS_RIGHT	/*!< Position droite (horraire) @deprecated Seule une amplitude de 90 degre est utilisee*/
}cube_servo_rot_pos_e;

/**
 * @enum cube_servo_cage_pos_e
 * @brief Positions concretes du servo de la cage
 */
typedef enum{
	CUBE_SERVO_POS_FLIP_UP,	/*!< Position de pivotage du cube*/
	CUBE_SERVO_POS_DEFAULT, /*!< Position par defaut*/
	CUBE_SERVO_POS_CUBE_LOCK/*!< Position de blocage du cube (les deux rangees supperieures)*/
}cube_servo_cage_pos_e;

/*
 * =====================================================================================
 * Prototypes priv�s
 * =====================================================================================
 */

void CUBE_SERVO_init(void);

void CUBE_SERVO_waitingTimer(void);

void CUBE_SERVO_handlePrimary(cube_servo_primary_mvt_e mvt);

void CUBE_SERVO_queue(cube_servo_primary_mvt_e mvt);

void CUBE_SERVO_unqueue(void);

void CUBE_SERVO_supportSetPosition(uint16_t position);

void CUBE_SERVO_cageSetPosition(uint16_t position);

void CUBE_SERVO_platform_pos_reach_process();

void CUBE_SERVO_platform_set_pos_to_reach(int position);

void CUBE_SERVO_cage_pos_reach_process();

void CUBE_SERVO_cage_set_pos_to_reach(int position);

/*
 * =====================================================================================
 * Variables priv�es
 * =====================================================================================
 */

static int CUBE_SERVO_cage_pos_to_reach;
static int CUBE_SERVO_cage_current_pos;

static int CUBE_SERVO_platform_pos_to_reach;
static int CUBE_SERVO_platform_current_pos;

static bool_e CUBE_SERVO_cage_new_pos = FALSE;
static bool_e CUBE_SERVO_platform_new_pos = FALSE;

static cube_servo_rot_pos_e cube_rot_position;
static cube_servo_cage_pos_e cube_cage_position;

static cube_servo_primary_mvt_e primary_mvt_list[2048];

static cube_servo_state_e current_state = CUBE_SERVO_INIT;
static cube_servo_state_e previous_state = CUBE_SERVO_INIT;

int queue_pointer = -1;

static volatile int t;

/*
 * =====================================================================================
 * Fonctions publiques
 * =====================================================================================
 */

void CUBE_SERVO_process(){
	bool_e entrance = current_state != previous_state;
	previous_state = current_state;

	switch(current_state){
		case CUBE_SERVO_INIT :
			Systick_add_callback_function(&CUBE_SERVO_platform_pos_reach_process);
			Systick_add_callback_function(&CUBE_SERVO_cage_pos_reach_process);
			Systick_add_callback_function(&CUBE_SERVO_waitingTimer);
			CUBE_SERVO_init();
			for(int i = 0 ; i<500; i++){
				primary_mvt_list[i] = -1;
			}
			cube_rot_position = CUBE_SERVO_POS_CENTER;
			cube_cage_position = CUBE_SERVO_DEFAULT_CAGE;
			current_state = CUBE_SERVO_MAKE;
			break;
		case CUBE_SERVO_MAKE :
			if(queue_pointer < 0){
				current_state = CUBE_SERVO_FINISHED;
			}
			else{
				CUBE_SERVO_handlePrimary(primary_mvt_list[0]);
				current_state = CUBE_SERVO_WAIT_SERVO;
			}
			break;
		case CUBE_SERVO_WAIT_SERVO :
			if(entrance){
				t = CUBE_SERVO_TIME_WAIT_STATE_MS;
			}
			if(t<=0){
				current_state = CUBE_SERVO_MAKE;
			}
			break;
		case CUBE_SERVO_FINISHED :
			break;
		default :
			break;
	}
}

void CUBE_SERVO_addMvt(cube_servo_complex_mvt_e mvt){
	switch(mvt){
		case CUBE_SERVO_BOTTOM_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_BOTTOM_180:
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_BOTTOM_90:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);

			break;
		case CUBE_SERVO_TOP_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_TOP_180:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);

			break;
		case CUBE_SERVO_TOP_90:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_FRONT_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_FRONT_180:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_FRONT_90:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_BACK_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_BACK_180:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_BACK_90:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_LEFT_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);

			break;
		case CUBE_SERVO_LEFT_180:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_LEFT_90:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);

			break;
		case CUBE_SERVO_RIGHT_90_A:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_RIGHT_180:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_RIGHT_90:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS_PLUS);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_GAUCHE_90:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
			break;
		case CUBE_SERVO_DROITE_90:
			CUBE_SERVO_queue(CUBE_SERVO_MAX_CLKWISE);
			break;
		case CUBE_SERVO_FLIP:
			CUBE_SERVO_queue(CUBE_SERVO_FLIP_UP);
			break;
		case CUBE_SERVO_DEFAULT_POS:
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		case CUBE_SERVO_DEFAULT_CAGE_COMPLEX:
			CUBE_SERVO_queue(CUBE_SERVO_DEFAULT_CAGE);
			break;
		case CUBE_SERVO_DEFAULT_ROT_COMPLEX:
			CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
			break;
		default:
			break;
	}
	if(current_state == CUBE_SERVO_FINISHED) current_state = CUBE_SERVO_MAKE;
}

cube_servo_state_e CUBE_SERVO_getState(void){
	return current_state;
}

void CUBE_SERVO_flush(){
	queue_pointer = -1;
}

#if MAIN_TEST

void CUBE_SERVO_test(){
	CUBE_SERVO_process();
//	CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
//	CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
//	CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
//	CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
//	CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
//	CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
//	CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);
//	CUBE_SERVO_addMvt(CUBE_SERVO_DEFAULT_CAGE_COMPLEX);
//	CUBE_SERVO_addMvt(CUBE_SERVO_FLIP);
//	CUBE_SERVO_queue(CUBE_SERVO_HOLD_CUBE);


//	CUBE_SERVO_queue(CUBE_SERVO_MAX_REVERSE);
//	CUBE_SERVO_queue(CUBE_SERVO_MIDDLE_POS);
	CUBE_SERVO_addMvt(CUBE_SERVO_BOTTOM_90_A);
	CUBE_SERVO_addMvt(CUBE_SERVO_TOP_90_A);

	while(1){
		CUBE_SERVO_process();
	}
}

#endif

/*
 * =====================================================================================
 * Fonctions priv�es
 * =====================================================================================
 */

void CUBE_SERVO_init(){
	//initialisation et lancement du timer1 � une p�riode de 10 ms
	TIMER_run_us(TIMER1_ID, CUBE_SERVO_PERIOD_TIMER*1000, FALSE); //10000us = 10ms

	//activation du signal PWM sur le canal 1 du timer 1 (broche PA8)
	TIMER_enable_PWM(TIMER1_ID, TIM_CHANNEL_1, 150, FALSE, FALSE);
	//rapport cyclique regl� pour une position servo de 50%
	CUBE_SERVO_supportSetPosition(CUBE_SERVO_MIDDLE_POS_VAL);
	TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_1, CUBE_SERVO_MIDDLE_POS_VAL+100);
	CUBE_SERVO_platform_current_pos = CUBE_SERVO_MIDDLE_POS_VAL;

	//activation du signal PWM sur le canal 1 du timer 1 (broche PA9)
	TIMER_enable_PWM(TIMER1_ID, TIM_CHANNEL_2, 150, FALSE, FALSE);
	//rapport cyclique regl� pour une position servo de 50%
	CUBE_SERVO_cageSetPosition(CUBE_SERVO_DEFAULT_CAGE_POS_VAL);
	TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_2, CUBE_SERVO_DEFAULT_CAGE_POS_VAL+100);
	CUBE_SERVO_cage_current_pos = CUBE_SERVO_DEFAULT_CAGE_POS_VAL;
	CUBE_SERVO_platform_new_pos = FALSE;
	CUBE_SERVO_cage_new_pos = FALSE;
}

void CUBE_SERVO_waitingTimer(){
	t = t-1;
}

void CUBE_SERVO_handlePrimary(cube_servo_primary_mvt_e mvt){
	switch(mvt){
		case CUBE_SERVO_MAX_CLKWISE :
			CUBE_SERVO_supportSetPosition(CUBE_SERVO_MAX_CLKWISE_POS_VAL);
			break;
		case CUBE_SERVO_MAX_REVERSE :
			CUBE_SERVO_supportSetPosition(CUBE_SERVO_MAX_REVERSE_POS_VAL);
			break;
		case CUBE_SERVO_MAX_REVERSE_PLUS :
			CUBE_SERVO_supportSetPosition(CUBE_SERVO_MAX_REVERSE_POS_VAL_PLUS);
			break;
		case CUBE_SERVO_MIDDLE_POS:
			CUBE_SERVO_supportSetPosition(CUBE_SERVO_MIDDLE_POS_VAL);
			break;
		case CUBE_SERVO_MIDDLE_POS_PLUS:
			CUBE_SERVO_supportSetPosition(CUBE_SERVO_MIDDLE_POS_VAL_PLUS);
			break;


		case CUBE_SERVO_HOLD_CUBE :
			CUBE_SERVO_cageSetPosition(CUBE_SERVO_HOLD_CUBE_POS_VAL);
			break;
		case CUBE_SERVO_FLIP_UP :
			CUBE_SERVO_cageSetPosition(CUBE_SERVO_FLIP_UP_POS_VAL);
			break;

		case CUBE_SERVO_DEFAULT_CAGE :
			CUBE_SERVO_cageSetPosition(CUBE_SERVO_DEFAULT_CAGE_POS_VAL);
			break;
	}
	CUBE_SERVO_unqueue();
}

void CUBE_SERVO_queue(cube_servo_primary_mvt_e mvt){
	queue_pointer ++;
	primary_mvt_list[queue_pointer] = mvt;
}

void CUBE_SERVO_unqueue(){
	for(int i = 0; i <= queue_pointer; i++){
		primary_mvt_list[i] = primary_mvt_list[i+1];
	}
	queue_pointer--;
}

void CUBE_SERVO_supportSetPosition(uint16_t position){
//	if(position > 100)
//		position = 100; //�cretage si l'utilisateur demande plus de 100%
	CUBE_SERVO_platform_set_pos_to_reach(position);
	//TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_1, position+100);*/

}

void CUBE_SERVO_cageSetPosition(uint16_t position){
//	if(position > 100)
//		position = 100; //�cretage si l'utilisateur demande plus de 100%
	CUBE_SERVO_cage_set_pos_to_reach(position);
	//TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_2, position+100);
}

void CUBE_SERVO_platform_pos_reach_process(){
	static int time = 0;
	if(time<10){
		time++;
	}

	else{
		if(CUBE_SERVO_platform_new_pos == TRUE){
			if(CUBE_SERVO_platform_pos_to_reach > CUBE_SERVO_platform_current_pos){
				CUBE_SERVO_platform_current_pos = ((CUBE_SERVO_platform_current_pos +2) >CUBE_SERVO_platform_pos_to_reach)?CUBE_SERVO_platform_pos_to_reach:(CUBE_SERVO_platform_current_pos +2);
				TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_1, CUBE_SERVO_platform_current_pos+100);

			}
			else if (CUBE_SERVO_platform_pos_to_reach < CUBE_SERVO_platform_current_pos){
				CUBE_SERVO_platform_current_pos = ((CUBE_SERVO_platform_current_pos -2) <CUBE_SERVO_platform_pos_to_reach)?CUBE_SERVO_platform_pos_to_reach:(CUBE_SERVO_platform_current_pos -2);
				TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_1, CUBE_SERVO_platform_current_pos+100);
			}
			else{
				CUBE_SERVO_platform_new_pos == FALSE;
			}
		}

		time=0;
	}
}

void CUBE_SERVO_platform_set_pos_to_reach(int position){
	CUBE_SERVO_platform_new_pos = TRUE;
	CUBE_SERVO_platform_pos_to_reach = position;
}

void CUBE_SERVO_cage_pos_reach_process(){
	static int time = 0;
	if(time<10){
		time++;
	}

	else{
		if(CUBE_SERVO_cage_new_pos == TRUE){
			if(CUBE_SERVO_cage_pos_to_reach > CUBE_SERVO_cage_current_pos){
				CUBE_SERVO_cage_current_pos = ((CUBE_SERVO_cage_current_pos +2) >CUBE_SERVO_cage_pos_to_reach)?CUBE_SERVO_cage_pos_to_reach:(CUBE_SERVO_cage_current_pos +2);
				TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_2, CUBE_SERVO_cage_current_pos+100);

			}
			else if (CUBE_SERVO_cage_pos_to_reach < CUBE_SERVO_cage_current_pos){
				CUBE_SERVO_cage_current_pos = ((CUBE_SERVO_cage_current_pos -2) <CUBE_SERVO_cage_pos_to_reach)?CUBE_SERVO_cage_pos_to_reach:(CUBE_SERVO_cage_current_pos -2);
				TIMER_set_duty(TIMER1_ID, TIM_CHANNEL_2, CUBE_SERVO_cage_current_pos+100);
			}
			else{
				CUBE_SERVO_cage_new_pos == FALSE;
			}
		}

		time=0;
	}
}

void CUBE_SERVO_cage_set_pos_to_reach(int position){
	CUBE_SERVO_cage_new_pos = TRUE;
	CUBE_SERVO_cage_pos_to_reach = position;

}
