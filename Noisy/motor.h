#ifndef MOTOR_H
#define MOTOR_H


#define POSITION_NOT_REACHED	0
#define POSITION_REACHED       	1

void turn_back (void);
void quarter_turn_right(void);
void quarter_turn_left(void);
void go_forward(void);
void stop(void);
void start(void);
void dont_go(void);
uint8_t get_state_motor(void);
//void set_state_motor(uint8_t motor);

#endif /* MOTOR_H */
