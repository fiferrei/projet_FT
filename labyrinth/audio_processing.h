#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H


#define FFT_SIZE 	1024

typedef enum {
	//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
	LEFT_CMPLX_INPUT = 0,
	RIGHT_CMPLX_INPUT,
	//Arrays containing the computed magnitude of the complex numbers
	LEFT_OUTPUT,
	RIGHT_OUTPUT,
} BUFFER_NAME_t;


void processAudioData(int16_t *data, uint16_t num_samples);
void triangulation(int16_t frequence_left, int16_t frequence_right);

/*
*	Returns the pointer to the BUFFER_NAME_t buffer asked
*/
float* get_audio_buffer_ptr(BUFFER_NAME_t name);

_Bool get_state_celebrate(void);
void set_state_celebrate(_Bool state);
_Bool get_labyrinth(void);
void set_labyrinth(_Bool state);

#endif /* AUDIO_PROCESSING_H */
