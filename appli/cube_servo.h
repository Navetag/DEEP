
#include "config.h"
#include "macro_types.h"
#include "systick.h"
#include "stm32f1_timer.h"

#define     TIME_WAIT_STATE_MS      1000

#define PERIOD_TIMER	10

#define MAX_REVERSE_POS_VAL		0
#define MAX_CLKWISE_POS_VAL		100
#define MIDDLE_POS_VALUE_VAL	50

#define FLIP_UP_POS_VAL			50
#define DEFAULT_CAGE_POS_VAL	20
#define HOLD_CUBE_POS_VAL		0



typedef enum{
    BOTTOM_90_A,
    BOTTOM_180,
    BOTTOM_90,

    TOP_90_A,
    TOP_180,
    TOP_90,

    FRONT_90_A,
    FRONT_180,
    FRONT_90,

    BACK_90_A,
    BACK_180,
    BACK_90,

    LEFT_90_A,
    LEFT_180,
    LEFT_90,

    RIGHT_90_A,
    RIGHT_180,
    RIGHT_90,

    GAUCHE_90,
    DROITE_90,
    FLIP,


    DEFAULT_POS

}cube_servo_complex_mvt_e;


typedef enum{
    MAX_CLKWISE,
    MAX_REVERSE,
	MIDDLE_POS,

    HOLD_CUBE,
    FLIP_UP,
    DEFAULT_CAGE         //Le doigt ne touche pas et le cube peut tourner

}cube_servo_primary_mvt_e;



typedef enum{
	INIT,
    WAIT_SERVO,
    MAKE,
    FINISHED
}cube_servo_state_e;

typedef enum{
    POS_CENTER,
    POS_LEFT,
    POS_RIGHT
}cube_servo_rot_pos_e;

typedef enum{
    POS_FLIP_UP,
    POS_DEFAULT,
    POS_CUBE_LOCK
}cube_servo_cage_pos_e;



/*
    Machine à état
        - gère une pile FIFO contenant les mvts en attente (tableau de )
        - récupère l'état du cube
*/
void cube_servo_process();

/*
    Ajoute un mouvement dans la pile
*/
bool_e cube_servo_addMvt(cube_servo_complex_mvt_e mvt);

/*
    Renvoie l'état de la machine servo
*/
cube_servo_state_e cube_servo_getState(void);

void cube_servo_init(void);


void handle_primary(cube_servo_primary_mvt_e mvt);

void queue(cube_servo_primary_mvt_e mvt);

void unqueue();

void CUBE_SERVO_support_set_position(uint16_t position);

void CUBE_SERVO_cage_set_position(uint16_t position);
