#include "cube_servo.h"


static cube_servo_rot_pos_e cube_rot_position;
static cube_servo_cage_pos_e cube_cage_position;
static cube_servo_primary_mvt_e * primary_mvt_list;



void cube_servo_process(cube_s cube){
	static cube_servo_state_e current_state = INIT;
	static cube_servo_state_e previous_state;

	switch(current_state){

		case INIT :
			//TODO
			break;

		case MAKE :
			//TODO
			break;

		case WAIT :
			//TODO
			break;

		case FINISHED :
			//TODO
			break;

		default :
			break;
	}
}
