#include <sensors/proximity.h>
#include <motor.h>
#include <audio/play_melody.h>

#define SIDE_OBSTACLE		50
#define FRONT_OBSTACLE		100
static uint8_t merit=0;
melody_t* song = NULL;

void proximity(void){
	if ((get_calibrated_prox(0)<FRONT_OBSTACLE || get_calibrated_prox(7)<FRONT_OBSTACLE) &&
		(get_calibrated_prox(5)>SIDE_OBSTACLE || get_calibrated_prox(2)>SIDE_OBSTACLE)){
		go_forward();
	}
	else if((get_calibrated_prox(0)>FRONT_OBSTACLE || get_calibrated_prox(7)>FRONT_OBSTACLE) &&
			get_calibrated_prox(6)>SIDE_OBSTACLE && get_calibrated_prox(3)<SIDE_OBSTACLE){
		quarter_turn_right();
		merit++;
	}
	else if ((get_calibrated_prox(0)>FRONT_OBSTACLE || get_calibrated_prox(7)>FRONT_OBSTACLE) &&
			get_calibrated_prox(5)<SIDE_OBSTACLE && get_calibrated_prox(2)>SIDE_OBSTACLE){
		quarter_turn_left();
		merit++;
	}
	else if (get_calibrated_prox(0)<8 && get_calibrated_prox(7)<8 &&
			get_calibrated_prox(5)<8 && get_calibrated_prox(2)<8 &&
			get_state_motor()==1 && merit>1){
		merit=0;
		while (get_state_motor()==1){
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
