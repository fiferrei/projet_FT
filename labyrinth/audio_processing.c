#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <motors.h>
#include <motor.h>
#include <audio/microphone.h>
#include <audio_processing.h>
//#include <communications.h>
#include <fft.h>
#include <arm_math.h>

//semaphore
static BSEMAPHORE_DECL(sendToComputer_sem, TRUE);

//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
static float micLeft_cmplx_input[2 * FFT_SIZE];
static float micRight_cmplx_input[2 * FFT_SIZE];
static float micFront_cmplx_input[2 * FFT_SIZE];
static float micBack_cmplx_input[2 * FFT_SIZE];
//Arrays containing the computed magnitude of the complex numbers
static float micLeft_output[FFT_SIZE];
static float micRight_output[FFT_SIZE];
static float micFront_output[FFT_SIZE];
static float micBack_output[FFT_SIZE];

#define MIN_VALUE_THRESHOLD 10000

#define MIN_FREQ 			25 //we don’t analyze before this index to not use resources for nothing
#define FREQ_START_L 		80 //1250 Hz
#define FREQ_START_H 		81 //1265 Hz
#define FREQ_CELEBRATE_L	67 //1047 Hz
#define FREQ_CELEBRATE_H	68 //1063 Hz
#define FREQ_TRIANG_L		28 //438 Hz
#define FREQ_TRIANG_H		29 //453 Hz
#define MAX_FREQ 			100//we don’t analyze after this index to not use resources for nothing
#define ON					1
#define RESET_VALUE			0

static uint8_t state_celebrate=0;
static uint8_t labyrinth=0;
/*
*	Callback called when the demodulation of the four microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*	
*	params :
*	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
*							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
*	uint16_t num_samples	Tells how many data we get in total (should always be 640)
*/
void sound_remote(float* data_left, float* data_right){
	float max_norm_left = MIN_VALUE_THRESHOLD;
	float max_norm_right = MIN_VALUE_THRESHOLD;
	int16_t max_norm_index_left = -1;
	int16_t max_norm_index_right = -1;
	//search for the highest peak
	for(uint16_t i = MIN_FREQ ; i <= MAX_FREQ ; i++){
		if(data_left[i] > max_norm_left){
			max_norm_left = data_left[i];
			max_norm_index_left = i;
		}
		if(data_right[i] > max_norm_right){
			max_norm_right = data_right[i];
			max_norm_index_right = i;
		}
	}
	//allow the robot to move
	if(max_norm_index_left >= FREQ_START_L && max_norm_index_left <= FREQ_START_H){
		start();
		labyrinth=ON;
		state_celebrate=RESET_VALUE;
	}
	//allow the robot to celebrate
	if(max_norm_index_left >= FREQ_CELEBRATE_L && max_norm_index_left <= FREQ_CELEBRATE_H){
		state_celebrate=ON;
	}
	//comes to the user via sound
	if(max_norm_index_left >= FREQ_TRIANG_L && max_norm_index_left <= FREQ_TRIANG_H && get_come_home()==ON){
		labyrinth=RESET_VALUE;
		state_celebrate=RESET_VALUE;
		triangulation(max_norm_index_left, max_norm_index_right);
	}
}

void triangulation(int16_t frequence_left, int16_t frequence_right){
	float phase_left = atan2f(micLeft_cmplx_input[2*frequence_left+1], micLeft_cmplx_input[2*frequence_left]);
	float phase_right = atan2f(micRight_cmplx_input[2*frequence_right+1], micRight_cmplx_input[2*frequence_right]);
	int16_t dephasage=(phase_right-phase_left)*180/PI;

	if(dephasage>-46 && dephasage<46 && abs(right_motor_get_pos()==RESET_VALUE)){ //avoid aberrant values
		turn_angle(dephasage);
	}
}

void processAudioData(int16_t *data, uint16_t num_samples){

	static uint16_t nb_samples = 0;
	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4){
		//construct an array of complex numbers. Put 0 to the imaginary part
		micRight_cmplx_input[nb_samples] = (float)data[i + MIC_RIGHT];
		micLeft_cmplx_input[nb_samples] = (float)data[i + MIC_LEFT];
		nb_samples++;
		micRight_cmplx_input[nb_samples] = 0;
		micLeft_cmplx_input[nb_samples] = 0;
		nb_samples++;
		//stop when buffer is full
		if(nb_samples >= (2 * FFT_SIZE)){
			break;
		}
	}
	if(nb_samples >= (2 * FFT_SIZE)){
		/* FFT proccessing
	 	*
		* This FFT function stores the results in the input buffer given.
		* This is an "In Place" function.
		*/
		doFFT_optimized(FFT_SIZE, micRight_cmplx_input);
		doFFT_optimized(FFT_SIZE, micLeft_cmplx_input);
		/* Magnitude processing
		*
		* Computes the magnitude of the complex numbers and
		* stores them in a buffer of FFT_SIZE because it only contains
		* real numbers.
		*
		*/
		arm_cmplx_mag_f32(micRight_cmplx_input, micRight_output, FFT_SIZE);
		arm_cmplx_mag_f32(micLeft_cmplx_input, micLeft_output, FFT_SIZE);
		//sends only one FFT result over 10 for 1 mic to not flood the computer
		nb_samples = 0;
		sound_remote(micLeft_output, micRight_output);
	}
}
	/*
	*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs.
	*
	*/

float* get_audio_buffer_ptr(BUFFER_NAME_t name){
	if(name == LEFT_CMPLX_INPUT){
		return micLeft_cmplx_input;
	}
	else if (name == RIGHT_CMPLX_INPUT){
		return micRight_cmplx_input;
	}
	else if (name == FRONT_CMPLX_INPUT){
		return micFront_cmplx_input;
	}
	else if (name == BACK_CMPLX_INPUT){
		return micBack_cmplx_input;
	}
	else if (name == LEFT_OUTPUT){
		return micLeft_output;
	}
	else if (name == RIGHT_OUTPUT){
		return micRight_output;
	}
	else if (name == FRONT_OUTPUT){
		return micFront_output;
	}
	else if (name == BACK_OUTPUT){
		return micBack_output;
	}
	else{
		return NULL;
	}
}

uint8_t get_state_celebrate(void){
	return state_celebrate;
}

void set_state_celebrate(uint8_t state){
	state_celebrate=state;
}

uint8_t get_labyrinth(void){
	return labyrinth;
}

void set_labyrinth(uint8_t state){
	labyrinth=state;
}
