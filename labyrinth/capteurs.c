#include <stdlib.h>
#include <stdint.h>
#include <sensors/proximity.h>
#include <motor.h>
#include <audio/play_melody.h>
#include <leds.h>
#include <motors.h>
#include <audio_processing.h>

#define SIDE_OBSTACLE			10
#define FRONT_OBSTACLE			110
#define NO_OBSTACLE_SECURITY	15
#define NEED_CALIBRATION		80
#define FRONT_SECURITY			30
#define MIDDLE_CROSS			250
#define MIDDLE_CROSS_R			20
#define OUT_CROSS				400
#define RESET					0
#define SET						1
#define IR1						0
#define IR3						2
#define IR6						5
#define IR8						7
#define LA						440
#define QUARTER_SECOND			250 //ms

static uint8_t impasse=0;
static uint8_t virage=0;
static uint8_t redress=0;

void proximity(void){
	// if obstacle front, obstacle left and no obstacle right turn right
	if((get_calibrated_prox(IR1)>FRONT_OBSTACLE || get_calibrated_prox(IR8)>FRONT_OBSTACLE) &&
		get_calibrated_prox(IR6)>SIDE_OBSTACLE && get_calibrated_prox(IR3)<SIDE_OBSTACLE){
		quarter_turn_right();
	}

	//if obstacle front, obstacle right and no obstacle left turn left
	else if ((get_calibrated_prox(IR1)>FRONT_OBSTACLE || get_calibrated_prox(IR8)>FRONT_OBSTACLE) &&
			  get_calibrated_prox(IR6)<SIDE_OBSTACLE && get_calibrated_prox(IR3)>SIDE_OBSTACLE){
		quarter_turn_left();
	}

	//if no obstacle front and one obstacle at each side of the robot go forward
	else if ((get_calibrated_prox(IR1)<FRONT_OBSTACLE || get_calibrated_prox(IR8)<FRONT_OBSTACLE) &&
			 (get_calibrated_prox(IR6)>SIDE_OBSTACLE && get_calibrated_prox(IR3)>SIDE_OBSTACLE)){

		//if the robot goes to close to the right wall, it goes a little to the left
		if((get_calibrated_prox(IR6)-get_calibrated_prox(IR3))>NEED_CALIBRATION &&
		   (get_calibrated_prox(IR1)<FRONT_SECURITY || get_calibrated_prox(IR8)<FRONT_SECURITY)){
			calibrate_pos_left();
			redress=SET;
		}
		//if the robot goes to close to the left wall, it goes a little to the right
		else if((get_calibrated_prox(IR3)-get_calibrated_prox(IR6))>NEED_CALIBRATION &&
			    (get_calibrated_prox(IR1)<FRONT_SECURITY || get_calibrated_prox(IR8)<FRONT_SECURITY)){
			calibrate_pos_right();
			redress=SET;
		}
		else{
			go_forward();
			redress=RESET;
		}
	}

	// if obstacle front and each side of the robot turns back
	else if ((get_calibrated_prox(IR1)>FRONT_OBSTACLE || get_calibrated_prox(IR8)>FRONT_OBSTACLE) &&
			  get_calibrated_prox(IR6)>SIDE_OBSTACLE && get_calibrated_prox(IR3)>SIDE_OBSTACLE){
		impasse=SET;
		turn_back();
	}

	//détecte un croisement
	else if ((get_calibrated_prox(IR1)<FRONT_SECURITY || get_calibrated_prox(IR8)<FRONT_SECURITY) &&
			  (get_calibrated_prox(IR6)<SIDE_OBSTACLE || get_calibrated_prox(IR3)<SIDE_OBSTACLE)){
		//go to the middle of the crossing
		right_motor_set_pos(RESET);
		while (abs(right_motor_get_pos())<MIDDLE_CROSS && redress==RESET){
			go_forward();
		}
		right_motor_set_pos(RESET);
		while (abs(right_motor_get_pos())<MIDDLE_CROSS_R && redress==SET){
			go_forward();
		}

		//if no obstacle around all the robot celebrate
		if (get_calibrated_prox(IR1)<NO_OBSTACLE_SECURITY && get_calibrated_prox(IR3)<NO_OBSTACLE_SECURITY &&
			get_calibrated_prox(IR6)<NO_OBSTACLE_SECURITY && get_calibrated_prox(IR8)<NO_OBSTACLE_SECURITY && get_state_celebrate()==SET){
			celebrate();
		}

		//croisement sans obstacle devant et à gauche 1ère tentative
		else if ((get_calibrated_prox(IR1)<FRONT_SECURITY || get_calibrated_prox(IR8)<FRONT_SECURITY) &&
			      get_calibrated_prox(IR6)<SIDE_OBSTACLE && get_calibrated_prox(IR3)>SIDE_OBSTACLE && impasse==RESET){
			playNote(LA, QUARTER_SECOND);
			virage=RESET;
			//go out of the crossing
			right_motor_set_pos(RESET);
			while(abs(right_motor_get_pos())<OUT_CROSS){
				go_forward();
			}
		}

		//croisement sans obstacle devant et à gauche 2ème tentative
		else if ((get_calibrated_prox(IR1)<FRONT_SECURITY || get_calibrated_prox(IR8)<FRONT_SECURITY) &&
				  get_calibrated_prox(IR6)>SIDE_OBSTACLE && get_calibrated_prox(IR3)<SIDE_OBSTACLE && impasse==SET){
			playNote(LA, QUARTER_SECOND);
			virage=RESET;
			impasse=RESET;
			quarter_turn_right();
		}

		//croisement sans obstacle devant et à droite 1ère tentative
		else if ((get_calibrated_prox(IR1)<FRONT_OBSTACLE || get_calibrated_prox(IR8)<FRONT_OBSTACLE) &&
				  get_calibrated_prox(IR6)>SIDE_OBSTACLE && get_calibrated_prox(IR3)<SIDE_OBSTACLE && impasse==RESET){
			playNote(LA, QUARTER_SECOND);
			virage=RESET;
			//go out of the crossing
			right_motor_set_pos(RESET);
			while(abs(right_motor_get_pos())<OUT_CROSS){
				go_forward();
			}
		}

		//croisement sans obstacle devant et à droite 2ème tentative
		else if ((get_calibrated_prox(IR1)<FRONT_OBSTACLE || get_calibrated_prox(IR8)<FRONT_OBSTACLE) &&
				  get_calibrated_prox(IR6)<SIDE_OBSTACLE && get_calibrated_prox(IR3)>SIDE_OBSTACLE && impasse==SET){
			playNote(LA, QUARTER_SECOND);
			virage=RESET;
			impasse=RESET;
			quarter_turn_left();
		}

		//croisement sans obstacle devant, à gauche et à droite 1ère tentative
		else if ((get_calibrated_prox(IR1)<FRONT_OBSTACLE || get_calibrated_prox(IR8)<FRONT_OBSTACLE) &&
				  get_calibrated_prox(IR6)<SIDE_OBSTACLE && get_calibrated_prox(IR3)<SIDE_OBSTACLE && impasse==RESET){
			playNote(LA, QUARTER_SECOND);
			virage=RESET;
			//go out of the crossing
			right_motor_set_pos(RESET);
			while(abs(right_motor_get_pos())<OUT_CROSS){
				go_forward();
			}
		}

		//croisement sans obstacle devant, à gauche et à droite 2ème tentative
		else if ((get_calibrated_prox(IR1)<FRONT_OBSTACLE || get_calibrated_prox(IR8)<FRONT_OBSTACLE) &&
				  get_calibrated_prox(IR6)<SIDE_OBSTACLE && get_calibrated_prox(IR3)<SIDE_OBSTACLE && impasse==SET && virage==RESET){
			virage=SET;
			impasse=RESET;
			playNote(LA, QUARTER_SECOND);
			quarter_turn_left();
		}

		//croisement sans obstacle devant, à gauche et à droite 3ème tentative
		else if ((get_calibrated_prox(IR1)<FRONT_OBSTACLE || get_calibrated_prox(IR8)<FRONT_OBSTACLE) &&
				  get_calibrated_prox(IR6)<SIDE_OBSTACLE && get_calibrated_prox(IR3)<SIDE_OBSTACLE && impasse==SET && virage==SET){
			virage=RESET;
			impasse=RESET;
			playNote(LA, QUARTER_SECOND);
			//go out of the crossing
			right_motor_set_pos(RESET);
			while(abs(right_motor_get_pos())<OUT_CROSS){
				go_forward();
			}
		}
	}
}
