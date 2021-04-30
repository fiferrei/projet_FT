#include <sensors/proximity.h>
#include <motor.h>
#include <audio/play_melody.h>

#define SIDE_OBSTACLE			40
#define FRONT_OBSTACLE			100
#define NO_OBSTACLE_SECURITY	15
#define IR1						0
#define IR3						2
#define IR6						5
#define IR8						7
#define ON						1
#define MERIT					2

static uint8_t merit=0;

void proximity(void){
	//if no obstacle front and obstacle in at least one side of the robot go forward
	if ((get_calibrated_prox(IR1)<FRONT_OBSTACLE || get_calibrated_prox(IR8)<FRONT_OBSTACLE) &&
		(get_calibrated_prox(IR6)>SIDE_OBSTACLE || get_calibrated_prox(IR3)>SIDE_OBSTACLE)){
		go_forward();
	}
	// if obstacle front, obstacle left and no obstacle right turn right
	else if((get_calibrated_prox(IR1)>FRONT_OBSTACLE || get_calibrated_prox(IR8)>FRONT_OBSTACLE) &&
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
	//if no obstacle around, motor is on and the robots merits celebrate
	else if (get_calibrated_prox(IR1)<NO_OBSTACLE_SECURITY && get_calibrated_prox(IR8)<NO_OBSTACLE_SECURITY &&
			get_calibrated_prox(IR6)<NO_OBSTACLE_SECURITY && get_calibrated_prox(3)<NO_OBSTACLE_SECURITY &&
			get_state_motor()==ON && merit>=MERIT){
		merit=0;
		while (get_state_motor()==ON){
			playMelody(SEVEN_NATION_ARMY, ML_SIMPLE_PLAY, NULL);
			turn_back();
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
