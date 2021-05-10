#ifndef MOTOR_H
#define MOTOR_H

void turn(void);
void turn_back(void);
void quarter_turn_right(void);
void quarter_turn_left(void);
void go_forward(void);
void turn_angle(float angle);
void calibrate_pos_right(void);
void calibrate_pos_left(void);
void stop(void);
void start(void);
uint8_t get_state_motor(void);
void celebrate(void);

#endif /* MOTOR_H */
