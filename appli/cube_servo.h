
#include "config.h"
#include "macro_types.h"
#include "systick.h"

#define     TIME_WAIT_STATE_MS      1000


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

    READING_POSITION,
    DEFAULT_POS

}cube_servo_complex_mvt_e;


typedef enum{
    CLOCKWISE_90,
    REVERSE_90,

    HOLD_CUBE,
    FLIP_UP,
    FLIP_DOWN,
    SCAN_POS,
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
