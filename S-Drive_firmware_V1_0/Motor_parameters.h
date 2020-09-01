/*****************************************************************************
  Variables that can be adjusted so the S-drive works for your motor
  For more details how to setup this check documentation on github!
******************************************************************************/

#ifndef PARAMS
#define PARAMS

#define MOTOR_POLES 22 // number of motor poles on rotor of motor

/*****************************************************************************
  Used to filter current
******************************************************************************/
const float CUTOFF_FREQ   = 20.0;  //Cutoff frequency in Hz (20)
const float SAMPLING_TIME = 0.001; //Sampling time in seconds.

/*****************************************************************************
  Variables for position PID loop
******************************************************************************/
double Kp_position = 3; //3
double Ki_position = 0;
double Kd_position = 0; // 0.06

/*****************************************************************************
  Variables for speed PID loop
******************************************************************************/
double Kp_speed = 0.3; //08
double Ki_speed = 75;//75
double Kd_speed = 0;
/*****************************************************************************/

uint16_t PWM_freq = 40000 ; // PWM switching frequency (40 kHz)

/*****************************************************************************
  Variables for Gravity compensation
******************************************************************************/
int Grav_time = 2000;
int Grav_current_threshold = 3;
int Grav_setpoint_offset = 2;
int Grav_compliance_speed = 3;
int Grav_PID_band = 2;
/*****************************************************************************
  Variables for Error compensation
******************************************************************************/
int Error_time = 7000; //13000 ili 7000 rade ok najčešće
int Error_current_threshold = 3; // nikad nemoj stavljat 1 jer u ne opterećenom stanju oscilira oko 1 i 0 i to onda uzrokuje oscilacije pri držanju pozicije
int Error_setpoint_offset = 2;
int Error_Setpoint_add = 1;
int Error_PID_band = 3;
/*****************************************************************************
  Bandwidht for slow speed position speed control.
******************************************************************************/
int PID_band_pos = 1;

int Kp_speed_emulation = 18;
int Slow_speed_adjustment_variable = 105;

int offset_1 = 23; //23 for 22P 37 for 14P
int offset_0 = 70; //70 for 22P 111 for 14P

uint16_t Main_current_sample_time = 1000;
uint16_t Main_speed_sample_time = 3000;
uint16_t Main_serial_output_time = 10000;
uint16_t Main_current_protection_time = 500000;
uint16_t Main_temperature_sample_time = 2000000;

int16_t Error_temperature = 55;

#endif
