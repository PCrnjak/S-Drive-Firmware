
/*************************************************************************
  License GPL-3.0
  Copyright (c) 2020 Petar Crnjak
  License file :
  https://github.com/PCrnjak/S_Drive---small-BLDC-driver/blob/master/LICENSE
**************************************************************************/

// TODO
//
// Known BUGS:
// When switching from voltage mode some other modes break:
// speed dir mode breaks and speed position mode breaks
//
// Board works more then 48 hours on 1000008 baud rate, while sending and receiveing data every 10ms
// Larger baud rates (More than 1000008) tend to break communication after 2-3 hours
// Larger send and receive intervals, more than 10ms alse break communication after 2-3 hours
// So ATM only stable is <1000008 baud rate and >10ms(100 Hz) update rate
//
// Problem with go to position and hold (Sometimes blocks code)
//
// This is not really a bug since EEPROM saving works great, it is not being used with full potential.
// Write proper EEPROM lib. ATM I am using only 1 sector of EEPROM. 
// This is not priority


#include <filters.h>
#include <Arduino.h>
#include <PID_v1.h>
#include <AS5040.h>
#include <QuadratureEncoder.h>
#include "SVPWM_table.h"
#include "Globals.h"
#include "Motor_parameters.h"
#include "Temperature_table.h"
#include <CircularBuffer.h>
#include <Wire.h>
#include "SparkFun_External_EEPROM.h"


AS5040 enc (CLK, CSn, DO) ;
ExternalEEPROM myMem;
TwoWire Wire2 (2, I2C_FAST_MODE);

CircularBuffer<int, 3> buffer; // 3*0.5s if motor is drawing max current for 1.5s it will shut down

IIR::ORDER  ORDER_  = IIR::ORDER::OD3; // Order (OD1 to OD4)
// Low-pass filter
Filter f(CUTOFF_FREQ, SAMPLING_TIME, ORDER_);

PID PID_speed(&Input, &Output, &Setpoint_speed, Kp_speed, Ki_speed, Kd_speed, DIRECT);
PID PID_position(&Input, &Output, &Setpoint, Kp_position, Ki_position, Kd_position, DIRECT);

HardwareTimer pwmtimer4(4); // We use timer 4 and pins that have acces to that timer to set PWM frequency

Encoders BLDC_encoder(ENCA, ENCB); // Input pins of Quadrature encoder

/***********************************************************************************************************
  Function prototypes
************************************************************************************************************/
void Set_PWM();
uint16_t Get_current();
void All_Enable_LOW();
void All_Enable_HIGH();
void PWM_ABC();
void PWM_ACB();
void Lock_position(int A_, int B_, int C_, int Amplitude);
bool BLDC_Setup(int delay_, int New_Max_DutyA , int steps_);
void open_loop(int32_t _steps , bool dir_ , int Time_, int duty_);
void Get_speed(int sample_time);
uint16_t Get_supply_voltage();
uint16_t Get_BEMF1();
uint16_t Get_BEMF2();
uint16_t Get_BEMF3();
void move_BLDC(int direction_);
void Current_sample(int sample_time);
void Gravity_compensation();
void Error_correction();
void Gravitiy_compensation_mode();
void Normal_position_hold();
void Position_hold_mode();
void PID_speed_mode();
void Slow_speed();
void Master_speed_control();
void Serial_diagnostic(int sample_time);
void GOTO_position_speed();
void GOTO_position_speed_no_hold();
void Current_protection(int sample_time);
uint8_t first_mode_select();
void diagnostic_mode();
uint32_t read_EEPROM_data();
void write_EEPROM_data();
void Temperature_sample(int sample_time);
void Get_temperature();


void setup() {

  // https://github.com/rogerclarkmelbourne/Arduino_STM32/issues/222
  Serial.end(); // We use pins that are usually used for USB communication, so we need to kill those coms
  delay(5);
  // EEPROM chip is C232875 , 16Kb (2k x 8)
  Wire2.begin();
  Wire2.setClock(100000);

  myMem.setPageSize(16); //In bytes. Has 128 byte page size.
  myMem.enablePollForWriteComplete();
  myMem.setPageWriteTime(3); //3 ms max write time;
  myMem.setMemorySize(100);


  //RS485 COMS, Serial1
  //Serial1.begin(4000000); // PA9 = TX1 , PA10 = RX1
  //pinMode(PA10, INPUT_PULLUP); //// Rx PIN for Serial1
  //Serial2.begin(); // PA2 = TX2 , PA3 = RX2

  Serial2.begin(1000008);
  pinMode(PA3, INPUT_PULLUP); // needs pullup or it will receive trash and trigger

  delay(1000);

  pinMode(LED, OUTPUT);
  pinMode(TERMISTOR, INPUT_ANALOG);
  pinMode( RS485_RE_DE, OUTPUT);

  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(EN3, OUTPUT);

  pinMode(CSn, OUTPUT);
  pinMode(PROG, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(DO, INPUT);
  pinMode(INDEX, INPUT_PULLDOWN);

  pinMode(PULS1, PWM);
  pinMode(PULS2, PWM);
  pinMode(PULS3, PWM);

  pinMode(BEMF1, INPUT_ANALOG);
  pinMode(BEMF2, INPUT_ANALOG);
  pinMode(BEMF3, INPUT_ANALOG);

  pinMode(SUPPLY, INPUT_ANALOG);

  pinMode(CURRENT_pin, INPUT_ANALOG);

  pinMode( EEPROM_WP, OUTPUT);

  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
  digitalWrite(CSn, LOW);
  digitalWrite(LED, LOW);

  //attachInterrupt(INDEX, Index_trigger, RISING);

  myMem.begin(EEPROM_ADDRESS, Wire2);
  enc.begin();

  PID_position.SetOutputLimits(0, 1000);
  PID_speed.SetOutputLimits(0, 1000);

  // sample time in ms
  PID_position.SetSampleTime(1);
  PID_speed.SetSampleTime(3);

  // set pwm freq
  Set_PWM();

  lookup_table_size = sizeof(pwmSin) / sizeof(pwmSin[0]); // Find lookup table size
  phaseShift = (lookup_table_size * 2) / (MOTOR_POLES * 3) ;
  phaseShift_2 = phaseShift * 2;

  currentStepA = 0;
  currentStepB = currentStepA + phaseShift;
  currentStepC = currentStepB + phaseShift;

  currentStepA_ = currentStepA;
  currentStepB_ = currentStepB;
  currentStepC_ = currentStepC;

  // This fixed some errors so I left it here
  All_Enable_LOW();
  delay(50);
  All_Enable_HIGH();

  // Run setup wizard
  uint8_t Main_mode = first_mode_select();

  delay(5);

  /// lock to position
  /// A,B,C, AMPLITUDE
  if (set_ == 0) {
    Lock_position(pwmSin[0], pwmSin[phaseShift], pwmSin[phaseShift_2], 1800);
  } else {
    Lock_position(pwmSin[0], pwmSin[phaseShift_2], pwmSin[phaseShift], 1800);
  }

  // Type of homing determined by setup wizard.
  // No homing
  if (Main_mode == 1) {

    delay(300);
    short_circuit_current = Get_current();
    BLDC_encoder.setEncoderCount(0);
    delay(300);
    Sector_offset = 0;

    // Sector homing
  } else if (Main_mode == 2) {

    delay(200);
    short_circuit_current = Get_current();
    int16_t current_aps_number = enc.read(), DEC;
    BLDC_encoder.setEncoderCount(0);
    //delay(80);

    // Index homing
  } else if ( Main_mode == 3) {

  }

  // Reduce short circuit current
  short_circuit_current_reduced = short_circuit_current * 0.85;

  PID_position.SetMode(AUTOMATIC);
  PID_speed.SetMode(AUTOMATIC);

  Setpoint_true = 0;
  Setpoint = 0;
  Setpoint_speed = 20;

  // # is start condition
  // i.e. is board that is connected to S-drive receives # it knows motor is operational
  Serial2.print("#");
  Serial2.print("\n");
}

//Main loop runs at around 20 kHz average
void loop() {

  //int m1 = micros();
  Get_input();
  Current_sample(Main_current_sample_time); // 1ms
  Get_speed(Main_speed_sample_time);  // 3ms
  Serial_diagnostic(Main_serial_output_time); // Should be 10 ms i.e. 10000
  Current_protection(Main_current_protection_time); // 0.5s
  Temperature_sample(Main_temperature_sample_time); // 2s

  if ( Raise_error == 0) {
    if (mode == 1) {
      GOTO_position_speed();
    }

    else if (mode == 2) {
      Position_hold_mode();
    }

    else if ( mode == 3) {
      GOTO_position_speed_no_hold();
    }

    else if ( mode == 4) {
      Gravitiy_compensation_mode();
    }

    else if ( mode == 5) {
      Position_hold_mode();
    }

    else if ( mode == 6) {
      Normal_position_hold();
    }

    else if ( mode == 7) {
      GOTO_position_speed_no_hold();
    }

    else if ( mode == 8) {
      Normal_position_hold();
    }
    else if ( mode == 9) {
      // nothing
    }
  } else {
    PID_position.SetTunings(3.2, 0, 0);
    Gravitiy_compensation_mode();
  }

  New_Max_Duty = ((Max_Duty * Output) / 1000) ;
  move_BLDC(dir_);

  //int m2= micros();
  //int loop_time = m2 - m1;
  //Serial2.println(loop_time);
}
