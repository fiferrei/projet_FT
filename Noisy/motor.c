#include <stdlib.h>
#include <stdint.h>
#include <motor.h>
#include <motors.h>
#include <leds.h>
#include <capteurs.h>

#define POSITIVE_SPEED		400
#define NEGATIVE_SPEED		-400
#define CALIBRATE_SPEED		410
#define DEMI_TOUR			650
#define	QUART_DE_TOUR		325
#define RESET_VALUE			0
#define ON					1
#define OFF					0

static uint8_t state_motor=0;

void turn (void){
	if (state_motor==ON){
		set_body_led(ON);
		//right_motor_set_pos(RESET_VALUE);
		//while (abs(right_motor_get_pos())<DEMI_TOUR){
		left_motor_set_speed(NEGATIVE_SPEED);
		right_motor_set_speed(POSITIVE_SPEED);

		//left_motor_set_speed(OFF);
		//right_motor_set_speed(OFF);
		//set_body_led(OFF);
	}
	right_motor_set_pos(RESET_VALUE);
}

void quarter_turn_right(void){
	if(state_motor==ON){
		set_led(LED3, ON);
		right_motor_set_pos(RESET_VALUE);
		while (abs(right_motor_get_pos())<QUART_DE_TOUR){
			left_motor_set_speed(POSITIVE_SPEED);
			right_motor_set_speed(NEGATIVE_SPEED);
		}
		left_motor_set_speed(OFF);
		right_motor_set_speed(OFF);
		set_led(LED3, OFF);
	}
	left_motor_set_speed(POSITIVE_SPEED);
	right_motor_set_speed(POSITIVE_SPEED);
	right_motor_set_pos(RESET_VALUE);
}

void quarter_turn_left(void){
	if (state_motor==ON){
		set_led(LED7, ON);
		right_motor_set_pos(RESET_VALUE);
		while (abs(right_motor_get_pos())<QUART_DE_TOUR){
			left_motor_set_speed(NEGATIVE_SPEED);
			right_motor_set_speed(POSITIVE_SPEED);
		}
		left_motor_set_speed(POSITIVE_SPEED);
		right_motor_set_speed(POSITIVE_SPEED);
		set_led(LED7,OFF);
	}
	right_motor_set_pos(RESET_VALUE);
}

void go_forward(void){
	if (state_motor==ON){
		left_motor_set_speed(POSITIVE_SPEED);
		right_motor_set_speed(POSITIVE_SPEED);
	}
}
void calibrate_pos_right(void){
	if (state_motor==ON){
		left_motor_set_speed(POSITIVE_SPEED);
		right_motor_set_speed(CALIBRATE_SPEED);
	}
}

void calibrate_pos_left(void){
	if (state_motor==ON){
		left_motor_set_speed(CALIBRATE_SPEED);
		right_motor_set_speed(POSITIVE_SPEED);
	}
}

void dont_go(void){
	left_motor_set_speed(OFF);
	right_motor_set_speed(OFF);
}
void stop(void){
	state_motor=OFF;
	set_merit(OFF);
	set_front_led(OFF);
	set_body_led(OFF);
	set_led(LED5,ON);
	left_motor_set_speed(OFF);
	right_motor_set_speed(OFF);
}

void start(void){
	state_motor=ON;
	set_front_led(ON);
	set_led(LED5,OFF);
}
uint8_t get_state_motor(void){
	return state_motor;
}
//
//void set_state_motor(uint8_t motor){
//	state_motor=motor;
//}






