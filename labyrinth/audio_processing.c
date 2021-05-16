#include <main.h>
#include <motors.h>
#include <motor.h>
#include <audio/microphone.h>
#include <audio_processing.h>
#include <fft.h>
#include <arm_math.h>

//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
static float micLeft_cmplx_input[2 * FFT_SIZE];
static float micRight_cmplx_input[2 * FFT_SIZE];
//Arrays containing the computed magnitude of the complex numbers
static float micLeft_output[FFT_SIZE];
static float micRight_output[FFT_SIZE];

#define MIN_VALUE_THRESHOLD 10000
#define MIN_FREQ 			25 //we don’t analyze before this index to not use resources for nothing
//triangulation is better with small frequencies
#define FREQ_TRIANG_L		28 //438 Hz
#define FREQ_TRIANG_H		29 //453 Hz
//high frequencies are needed so that the celebration and the start are not activated while the robot is running
#define FREQ_CELEBRATE_L	67 //1047 Hz
#define FREQ_CELEBRATE_H	68 //1062 Hz
#define FREQ_START_L 		80 //1250 Hz
#define FREQ_START_H 		81 //1265 Hz
#define MAX_FREQ 			100//we don’t analyze after this index to not use resources for nothing

#define ON					1
#define RESET_VALUE			0
#define START_INDEX			-1
#define MAX_ANGLE			46
#define MIN_ANGLE			-46

static _Bool state_celebrate=RESET_VALUE;
static _Bool labyrinth=RESET_VALUE;
/*
 * Finds the frequency of the signal
 * then calls different functions depending on said frequency
*/
void sound_remote(float* data_left, float* data_right){
	float max_norm_left = MIN_VALUE_THRESHOLD;
	float max_norm_right = MIN_VALUE_THRESHOLD;
	int16_t max_norm_index_left = START_INDEX;
	int16_t max_norm_index_right = START_INDEX;
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
	//allow the robot to celebrate (when he got out of the labyrinth)
	if(max_norm_index_left >= FREQ_CELEBRATE_L && max_norm_index_left <= FREQ_CELEBRATE_H){
		state_celebrate=ON;
	}
	//comes to the user via sound (once he celebrated)
	if(max_norm_index_left >= FREQ_TRIANG_L && max_norm_index_left <= FREQ_TRIANG_H && get_come_home()==ON){
		labyrinth=RESET_VALUE;
		state_celebrate=RESET_VALUE;
		triangulation(max_norm_index_left, max_norm_index_right);
	}
}

/*
 * Finds the angle of the incoming sound
 * frequence_left and frequence_right should be the same, 440 Hz here (FREQ_TRIANG)
 */
void triangulation(int16_t frequence_left, int16_t frequence_right){
	float phase_left = atan2f(micLeft_cmplx_input[2*frequence_left+1], micLeft_cmplx_input[2*frequence_left]);
	float phase_right = atan2f(micRight_cmplx_input[2*frequence_right+1], micRight_cmplx_input[2*frequence_right]);
	int16_t dephasage=(phase_right-phase_left)*180/PI; //compute the phase difference, which gives the angle

	//avoid aberrant values and check if the robot is already moving
	if(dephasage>MIN_ANGLE && dephasage<MAX_ANGLE && abs(right_motor_get_pos()==RESET_VALUE)){
		turn_angle(dephasage);
	}
}

/*
*	Callback called when the demodulation of the microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*/
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
		// FFT proccessing
		doFFT_optimized(FFT_SIZE, micRight_cmplx_input);
		doFFT_optimized(FFT_SIZE, micLeft_cmplx_input);

		// Magnitude processing
		arm_cmplx_mag_f32(micRight_cmplx_input, micRight_output, FFT_SIZE);
		arm_cmplx_mag_f32(micLeft_cmplx_input, micLeft_output, FFT_SIZE);

		//sends only one FFT result over 10
		nb_samples = 0;
		sound_remote(micLeft_output, micRight_output);
	}
}

float* get_audio_buffer_ptr(BUFFER_NAME_t name){
	if(name == LEFT_CMPLX_INPUT){
		return micLeft_cmplx_input;
	}
	else if (name == RIGHT_CMPLX_INPUT){
		return micRight_cmplx_input;
	}
	else if (name == LEFT_OUTPUT){
		return micLeft_output;
	}
	else if (name == RIGHT_OUTPUT){
		return micRight_output;
	}
	else{
		return NULL;
	}
}

_Bool get_state_celebrate(void){
	return state_celebrate;
}

void set_state_celebrate(_Bool state){
	state_celebrate=state;
}

_Bool get_labyrinth(void){
	return labyrinth;
}

void set_labyrinth(_Bool state){
	labyrinth=state;
}
