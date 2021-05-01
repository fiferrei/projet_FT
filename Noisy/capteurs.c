#include <sensors/proximity.h>
#include <motor.h>
#include <audio/play_melody.h>
#include <leds.h>

#define SIDE_OBSTACLE			40
#define FRONT_OBSTACLE			80
#define NO_OBSTACLE_SECURITY	25
#define NEED_CALIBRATION		200
#define FRONT_SECURITY			20
#define IR1						0
#define IR3						2
#define IR6						5
#define IR8						7
#define ON						1
#define MERIT					2

static uint8_t merit=0;

void proximity(void){
	// if obstacle front, obstacle left and no obstacle right turn right
	if((get_calibrated_prox(IR1)>FRONT_OBSTACLE || get_calibrated_prox(IR8)>FRONT_OBSTACLE) &&
			get_calibrated_prox(IR6)>SIDE_OBSTACLE && get_calibrated_prox(IR3)<SIDE_OBSTACLE){
		quarter_turn_right();
		merit++;
	}

	//if obstacle front, obstacle right and no obstacle left turn left
	else if ((get_calibrated_prox(IR1)>FRONT_OBSTACLE || get_calibrated_prox(IR8)>FRONT_OBSTACLE) &&
			get_calibrated_prox(IR6)<SIDE_OBSTACLE && get_calibrated_prox(IR3)>SIDE_OBSTACLE){
		quarter_turn_left();
		merit++;
	}

	//if no obstacle front and obstacle in at least one side of the robot go forward
	else if ((get_calibrated_prox(IR1)<FRONT_OBSTACLE || get_calibrated_prox(IR8)<FRONT_OBSTACLE) &&
		(get_calibrated_prox(IR6)>SIDE_OBSTACLE || get_calibrated_prox(IR3)>SIDE_OBSTACLE)){

		if((get_calibrated_prox(IR6)-get_calibrated_prox(IR3))>NEED_CALIBRATION &&
		    (get_calibrated_prox(IR1)<FRONT_SECURITY && get_calibrated_prox(IR8)<FRONT_SECURITY)){
			calibrate_pos_left();
		}
		else if((get_calibrated_prox(IR3)-get_calibrated_prox(IR6))>NEED_CALIBRATION &&
			     (get_calibrated_prox(IR1)<FRONT_SECURITY && get_calibrated_prox(IR8)<FRONT_SECURITY)){
			calibrate_pos_right();
		}
		else{
			go_forward();
		}
	}

	//if no obstacle around, motor is on and the robots merits celebrate
	else if ((get_calibrated_prox(IR1)<NO_OBSTACLE_SECURITY || get_calibrated_prox(IR8)<NO_OBSTACLE_SECURITY) &&
			get_calibrated_prox(IR6)<NO_OBSTACLE_SECURITY && get_calibrated_prox(3)<NO_OBSTACLE_SECURITY &&
			get_state_motor()==ON && merit>=MERIT){
		merit=0;
		turn();
		while (get_state_motor()==ON){
			playMelody(SEVEN_NATION_ARMY, ML_SIMPLE_PLAY, NULL);
		}
		stopCurrentMelody();
	}
//	else{
//		dont_go();
//	}
}

void set_merit(uint8_t setting){
	merit=setting;
}
