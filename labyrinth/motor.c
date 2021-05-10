#include <stdlib.h>
#include <stdint.h>
#include <motor.h>
#include <motors.h>
#include <leds.h>
#include <audio_processing.h>
#include <audio/play_melody.h>

#define POSITIVE_SPEED			400
#define NEGATIVE_SPEED			-400
#define CALIBRATE_SPEED			500
#define CALIBRATE_STEP			200
#define REDRESS_STEP			150
#define TWO_TURNS				2600
#define DEMI_TOUR				650
#define	QUART_DE_TOUR			325
#define FINISH_CORNER			450
#define RESET_VALUE				0
#define ON						1
#define OFF						0
#define CONVERSION_STEP			7.222f
#define STRAIGHT_LINE			1400

static uint8_t state_motor=0;
static _Bool come_home=0;

//the robot turns on itself
void turn (void){
	if (state_motor==ON){
		set_body_led(ON);
		left_motor_set_speed(NEGATIVE_SPEED);
		right_motor_set_speed(POSITIVE_SPEED);
	}
}

//the robot turns back
void turn_back (void){
	if (state_motor==ON){
		set_body_led(ON);
		right_motor_set_pos(RESET_VALUE);
		while (abs(right_motor_get_pos())<DEMI_TOUR){
			left_motor_set_speed(NEGATIVE_SPEED);
			right_motor_set_speed(POSITIVE_SPEED);
		}
		left_motor_set_speed(OFF);
		right_motor_set_speed(OFF);
		set_body_led(OFF);
	}
	right_motor_set_pos(RESET_VALUE);
}

//the robot turns right
void quarter_turn_right(void){
	if(state_motor==ON){
		set_led(LED3, ON);
		right_motor_set_pos(RESET_VALUE);
		while (abs(right_motor_get_pos())<QUART_DE_TOUR){
			left_motor_set_speed(POSITIVE_SPEED);
			right_motor_set_speed(NEGATIVE_SPEED);
		}
		//the robot leaves the bend to have walls on both sides
		right_motor_set_pos(RESET_VALUE);
		while (abs(right_motor_get_pos())<FINISH_CORNER){
			left_motor_set_speed(POSITIVE_SPEED);
			right_motor_set_speed(POSITIVE_SPEED);
		}
		set_led(LED3, OFF);
	}
	right_motor_set_pos(RESET_VALUE);
}

//the robot turns left
void quarter_turn_left(void){
	if (state_motor==ON){
		set_led(LED7, ON);
		right_motor_set_pos(RESET_VALUE);
		while (abs(right_motor_get_pos())<QUART_DE_TOUR){
			left_motor_set_speed(NEGATIVE_SPEED);
			right_motor_set_speed(POSITIVE_SPEED);
		}
		//the robot leaves the bend to have walls on both sides
		right_motor_set_pos(RESET_VALUE);
		while (abs(right_motor_get_pos())<FINISH_CORNER){
			left_motor_set_speed(POSITIVE_SPEED);
			right_motor_set_speed(POSITIVE_SPEED);
		}
		set_led(LED7,OFF);
	}
	right_motor_set_pos(RESET_VALUE);
}

//the robot goes forward
void go_forward(void){
	if (state_motor==ON){
		left_motor_set_speed(POSITIVE_SPEED);
		right_motor_set_speed(POSITIVE_SPEED);
	}
}

void go_forward_step(void){
	if (state_motor==ON){
		int16_t nb_step = abs(right_motor_get_pos()) + STRAIGHT_LINE;
		set_led(LED1, ON);
		while(abs(right_motor_get_pos()) < nb_step){
			left_motor_set_speed(POSITIVE_SPEED);
			right_motor_set_speed(POSITIVE_SPEED);
		}
		set_led(LED1, OFF);
		right_motor_set_pos(RESET_VALUE);
		stop();
	}
}

void turn_angle(int16_t angle){
	int16_t nb_step = abs(CONVERSION_STEP*angle);		//465*angle //415*angle
	if (state_motor == ON){
		right_motor_set_pos(RESET_VALUE);
		if (angle < 0){
			set_led(LED7, ON);
			while(abs(right_motor_get_pos()) < nb_step){
				left_motor_set_speed(NEGATIVE_SPEED);
				right_motor_set_speed(POSITIVE_SPEED);
			}
			set_led(LED7,OFF);
		}
		else {
			set_led(LED3, ON);
			while(abs(right_motor_get_pos()) < nb_step){
				right_motor_set_speed(NEGATIVE_SPEED);
				left_motor_set_speed(POSITIVE_SPEED);
			}
			set_led(LED3,OFF);
		}
		go_forward_step();
	}
}

//the robot is to close to the wall, it goes a little to the right
void calibrate_pos_right(void){
	if (state_motor==ON){
		set_led(LED3, ON);
		//the robot moves away from the right wall
		right_motor_set_pos(RESET_VALUE);
		while (abs(right_motor_get_pos())<CALIBRATE_STEP){
			left_motor_set_speed(POSITIVE_SPEED);
			right_motor_set_speed(CALIBRATE_SPEED);
		}
		//the robot straightens up to be parallel to the walls again
		right_motor_set_pos(RESET_VALUE);
		while (abs(right_motor_get_pos())<REDRESS_STEP){
			left_motor_set_speed(CALIBRATE_SPEED);
			right_motor_set_speed(POSITIVE_SPEED);
		}
		set_led(LED3, OFF);
		right_motor_set_pos(RESET_VALUE);
		left_motor_set_speed(POSITIVE_SPEED);
		right_motor_set_speed(POSITIVE_SPEED);
	}
}

//the robot is to close to the wall, it goes a little to the left
void calibrate_pos_left(void){
	if (state_motor==ON){
		set_led(LED7, ON);
		//the robot moves away from the left wall
		left_motor_set_pos(RESET_VALUE);
		while (abs(left_motor_get_pos())<CALIBRATE_STEP){
			left_motor_set_speed(CALIBRATE_SPEED);
			right_motor_set_speed(POSITIVE_SPEED);
		}
		//the robot straightens up to be parallel to the walls again
		left_motor_set_pos(RESET_VALUE);
		while (abs(left_motor_get_pos())<REDRESS_STEP){
			left_motor_set_speed(POSITIVE_SPEED);
			right_motor_set_speed(CALIBRATE_SPEED);
		}
		set_led(LED7, OFF);
		left_motor_set_pos(RESET_VALUE);
		left_motor_set_speed(POSITIVE_SPEED);
		right_motor_set_speed(POSITIVE_SPEED);
	}
}

//the robot can't move anymore
void stop(void){
	state_motor=OFF;
	set_front_led(OFF);
	set_body_led(OFF);
	set_led(LED5,ON);
	come_home=OFF;
	set_labyrinth(OFF);
	set_state_celebrate(OFF);
	left_motor_set_speed(OFF);
	right_motor_set_speed(OFF);
}

//the robot can move
void start(void){
	state_motor=ON;
	set_front_led(ON);
	set_led(LED5,OFF);
}

//say if the robot can move or not
uint8_t get_state_motor(void){
	return state_motor;
}

//celebrate at the end of the labyrinth
void celebrate(void){
	right_motor_set_pos(RESET_VALUE);
	while (abs(right_motor_get_pos())<TWO_TURNS){
		turn();
		playMelody(SEVEN_NATION_ARMY, ML_SIMPLE_PLAY, NULL);
	}
	stopCurrentMelody();
	set_body_led(OFF);
	set_state_celebrate(OFF);
	set_labyrinth(OFF);
	left_motor_set_speed(OFF);
	right_motor_set_speed(OFF);
	come_home=ON;
	right_motor_set_pos(RESET_VALUE);
}

_Bool get_come_home(void){
	return come_home;
}







