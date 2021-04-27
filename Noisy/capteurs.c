#include <sensors/proximity.h>
#include <motor.h>
#include <audio/play_melody.h>

static uint8_t merit=0;
melody_t* song = NULL;

void proximity(void){
	if ((get_calibrated_prox(0)<200 || get_calibrated_prox(7)<200) &&
		(get_calibrated_prox(5)>25 || get_calibrated_prox(2)>25)){
		go_forward();
	}
	else if((get_calibrated_prox(0)>200 || get_calibrated_prox(7)>200) &&
			get_calibrated_prox(6)>20 && get_calibrated_prox(3)<30){
		quarter_turn_right();
		merit++;
	}
	else if ((get_calibrated_prox(0)>200 || get_calibrated_prox(7)>200) &&
			get_calibrated_prox(5)<30 && get_calibrated_prox(2)>30){
		quarter_turn_left();
		merit++;
	}
	else if (get_calibrated_prox(0)<8 && get_calibrated_prox(7)<8 &&
			get_calibrated_prox(5)<8 && get_calibrated_prox(2)<8 &&
			get_state_motor()==1 && merit>1){
		merit=0;
		while (get_state_motor()==1){
			turn_back();
			//playMelody(1,2, song);
		}
	}
}
