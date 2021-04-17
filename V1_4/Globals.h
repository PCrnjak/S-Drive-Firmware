/*****************************************************************************
  Global variables and pin definitions
******************************************************************************/

#ifndef GLOBAL_VARIABLES
#define GLOBAL_VARIABLES

/*****************************************************************************
  Pin definitions
  They are specific for S-drive V3 dont change them.
******************************************************************************/
#define LED PB1

#define TERMISTOR PA7

#define RS485_RE_DE PA8

#define EN1 PC13
#define EN2 PC14
#define EN3 PC15

#define PULS1 PB7
#define PULS2 PB8
#define PULS3 PB9

#define BEMF1 PA6
#define BEMF2 PA5
#define BEMF3 PA4

#define SUPPLY PA0

#define CURRENT_pin PB0

/*****************************************************************************/
//Encoder pins

#define CSn PA11 // ih HIGH state locks Index ,A,B to HIGH, going to LOW state releases them

#define CLK PA12

#define DO PB5

#define INDEX PB14

#define PROG PB15

#define ENCA PB12

#define ENCB PB13
 
//EEPROM pins

#define EEPROM_SCL PB10

#define EEPROM_SDA PB11

#define EEPROM_WP PA1

#define EEPROM_ADDRESS 0b1010111

/*****************************************************************************/

int32_t Setpoint_true; // Setpoint for position we want to hold
double Setpoint;       // This setpoint changes to achieve Setpoint_true
double Setpoint_speed; // Speed setpoint
double Input;          // Input for speed or position
// Output is usally value from 0-1000 that then get transformed into duty cycle value
double Output;         // Output for speed or position

const float SHUNT_RESISTOR = 0.1; // In ohms

/*****************************************************************************
  Flags and other variables
******************************************************************************/
bool set_;

int out ;

int32_t EncoderCount = 0;
int32_t EncoderCount_ = 0;

int16_t currentStepA;
int16_t currentStepB;
int16_t currentStepC;

int16_t currentStepA_;
int16_t currentStepB_;
int16_t currentStepC_;

uint16_t lookup_table_size;

bool Master_dir = 1;
bool dir_ = 1; // direction 1 is in when looking at shaft CCW

int16_t A;
int16_t B;
int16_t C;
uint16_t  Max_Duty = 0;

int16_t New_Max_Duty = 0;

int Measured_speed = 0;

uint16_t Current_ = 0;
uint16_t Current_filter = 0;

uint16_t phaseShift = 0;
uint16_t phaseShift_2 = 0;

int16_t Temperature_global = 0;

bool flag_1 = 0;
bool flag_3 = 0;
uint8_t mode = 4; // Gravity mode
uint16_t sample_time_speed = 0;

uint16_t short_circuit_current = 0; //We get this current at the motor startup
uint16_t short_circuit_current_reduced = 0;

uint8_t EEPROM_mode = 0;
bool    EEPROM_dir = 0;
int16_t EEPROM_aps = 0;

int16_t Sector_offset = 0;

bool Raise_error = 0; // 1 is error 0 is operating state
byte Raise_error_Temp = 0; // temperature error is 1
byte Raise_error_current = 0; // current error is 10


#endif
