#include "cube_servo.h"


static cube_servo_rot_pos_e cube_rot_position;
static cube_servo_cage_pos_e cube_cage_position;
static cube_servo_primary_mvt_e primary_mvt_list[500];

static cube_servo_state_e current_state = INIT;
static cube_servo_state_e previous_state;

int queue_pointer = -1;

static volatile int t;

void CUBE_SERVO_waitingTimer(){
	t = t-1;
}

void CUBE_SERVO_process(){
	bool_e entrance = (current_state != previous_state)?TRUE:FALSE;
	previous_state = current_state;

	switch(current_state){

		case INIT :
			Systick_add_callback_function(&waitingTimer);
			cube_servo_init();
			for(int i = 0 ; i<500; i++){
				primary_mvt_list[i] = -1;
			}
			cube_rot_position = POS_CENTER;
			cube_cage_position = DEFAULT_CAGE;
			current_state = MAKE;
			break;

		case MAKE :
			if(queue_pointer < 0){
				current_state = FINISHED;
			}
			else{
				handle_primary(primary_mvt_list[0]);
				current_state = MAKE;
			}
			break;

		case WAIT_SERVO :
			if(entrance){
				t = TIME_WAIT_STATE_MS;
			}
			if(t<=0){
				current_state = MAKE;
			}
			break;

		case FINISHED :
			break;

		default :
			break;
	}
}

bool_e CUBE_SERVO_addMvt(cube_servo_complex_mvt_e mvt){
	switch(mvt){
		case BOTTOM_90_A:

			break;
		case BOTTOM_180:

			break;
		case BOTTOM_90:

			break;
		case TOP_90_A:

			break;
		case TOP_180:

			break;
		case TOP_90:

			break;
		case FRONT_90_A:

			break;
		case FRONT_180:

			break;
		case FRONT_90:

			break;
		case BACK_90_A:

			break;
		case BACK_180:

			break;
		case BACK_90:

			break;
		case LEFT_90_A:

			break;
		case LEFT_180:

			break;
		case LEFT_90:

			break;
		case RIGHT_90_A:

			break;
		case RIGHT_180:

			break;
		case RIGHT_90:

			break;
		case GAUCHE_90:

			break;
		case DROITE_90:

			break;
		case FLIP:

			break;
		case READING_POSITION:

			break;
		case DEFAULT_POS:

			break;

	}
}


cube_servo_state_e CUBE_SERVO_getState(void){
	return current_state;
}

void CUBE_SERVO_init(){
	//initialisation et lancement du timer1 à une période de 10 ms
	TIMER_run_us(TIMER1_ID, PERIOD_TIMER*1000, FALSE); //10000us = 10ms
	//activation du signal PWM sur le canal 1 du timer 1 (broche PA8)
	TIMER_enable_PWM(TIMER1_ID, TIM_CHANNEL_1, 150, FALSE, FALSE);
	//rapport cyclique reglé pour une position servo de 50%
	SERVO_set_position(50);


	//initialisation et lancement du timer1 à une période de 10 ms
	TIMER_run_us(TIMER1_ID, PERIOD_TIMER*1000, FALSE); //10000us = 10ms
	//activation du signal PWM sur le canal 1 du timer 1 (broche PA9)
	TIMER_enable_PWM(TIMER1_ID, TIM_CHANNEL_2, 150, FALSE, FALSE);
	//rapport cyclique reglé pour une position servo de 50%
	SERVO_set_position(50);


}

void CUBE_SERVO_handle_primary(cube_servo_primary_mvt_e mvt){
	//TODO
}

void CUBE_SERVO_queue(cube_servo_primary_mvt_e mvt){
	//TODO
}

void CUBE_SERVO_unqueue(){
	//TODO
}
