


/*****************************************************************************
  A_,B_,C_ are positions in our sine table
  B_ must be offset by phase shift and C_ by 2 phase shifts from A_
  Amplitude represents "voltage" applied to phases (actually duty cycle value)
  Max_Duty is maximum duty cycle value we can use:
  (1800 for 72MHZ CPU_CLOCK and PWM freq of 40kHz)
******************************************************************************/
void Lock_position(int A_, int B_, int C_, int Amplitude) {

  A = map(A_, 0, Max_Duty, 0, Amplitude);
  B = map(B_, 0, Max_Duty, 0, Amplitude);
  C = map(C_, 0, Max_Duty, 0, Amplitude);

  PWM_ABC();
}

/*****************************************************************************
  Set PWM frequency for IN1,IN2,IN3 pins
  These pins drive phases A,B,C
  Duty cycle controls voltage applied to each phase
******************************************************************************/

void Set_PWM() {

  Max_Duty = F_CPU / PWM_freq ; // maximum duty cycle value ( in our case 1800)
  pwmtimer4.pause(); // pause timer
  pwmtimer4.setPrescaleFactor(1); // sets prescale , 1 means 72Mhz is devided by one, so period is 1 / 72 Mhz
  pwmtimer4.setOverflow(Max_Duty); //every 2000, (1800 bi bilo 40khz) ticks of 1/72 Mhz we overflow, so our period is 2000 * (1/72Mhz) = 2.7777*e-5
  pwmtimer4.refresh(); /// our frequency is 1 / 2.77777*e-5 == 36 khz
  pwmtimer4.resume(); /// continue with new settings
  /// this means that we can adjust our duty cycle with values 0-2000, 0 meaning 0%, 1000 = 50% and 2000 = 100%

}


/*****************************************************************************
  Get current reading in mA
  Max returned value is 5000mA  , or 5A
******************************************************************************/
uint16_t Get_current() {
  uint16_t raw = analogRead(CURRENT_pin); // reads voltage from 0(0) to 3.3V(4095). 3.3v means that 5A of current is flowing
  uint16_t C_ = map(raw, 0, 4095, 0, 5000); // Map our reading to 0-5A times 1000, so we can avoid floats
  return C_;
}


/*****************************************************************************
  Get voltage on thermistor that is in series with 51kohm resistor
  Then by using this code: https://github.com/PCrnjak/Thermistor-table-generator-using-Steinhart-Hart-equation
  We got lookup table that connects value from analog read to temperature
******************************************************************************/
void Get_temperature() {

  int16_t raw = analogRead(TERMISTOR);
  int16_t T_ = map(raw, 0, 4095, 0, 1023); // Downscale our 12 bit ADC to 10 bit since our lookup table is 10 bit
  Temperature_global = Temp_table[T_];

}

/*****************************************************************************
  Disable all 3 phases
******************************************************************************/
void All_Enable_LOW() {
  digitalWrite(EN1, LOW);
  digitalWrite(EN2, LOW);
  digitalWrite(EN3, LOW);
}


/*****************************************************************************
  Enable all 3 phases
******************************************************************************/
void All_Enable_HIGH() {
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
}

/*****************************************************************************
  PWM sequance for 1 direction
******************************************************************************/
void PWM_ABC() {
  //pwmWrite(PIN_,DUTY_CYCLE);
  pwmWrite(PULS1, A);
  pwmWrite(PULS2, B);
  pwmWrite(PULS3, C);
}

/*****************************************************************************
  PWM sequance for 0 direction
  As you can see only phases B and C are switched
  That causes reverese direction
******************************************************************************/
void PWM_ACB() {
  //pwmWrite(PIN_,DUTY_CYCLE);
  pwmWrite(PULS1, A);
  pwmWrite(PULS2, C);
  pwmWrite(PULS3, B);
}

/*****************************************************************************
  Get supply voltage in mV
******************************************************************************/
uint16_t Get_supply_voltage() {
  uint16_t raw = analogRead(SUPPLY);
  uint16_t v_ = map(raw, 0, 4095, 0, 4000); //max supply voltage is 40v DC
  return v_;

}

uint16_t Get_BEMF1() {
  uint16_t raw = analogRead(BEMF1);
  uint16_t BEMF_ = map(raw, 0, 4095, 0, 4000); //max supply voltage is 40v DC
  return BEMF_;

}

uint16_t Get_BEMF2() {
  uint16_t raw = analogRead(BEMF2);
  uint16_t BEMF_ = map(raw, 0, 4095, 0, 4000); //max supply voltage is 40v DC
  return BEMF_;

}

uint16_t Get_BEMF3() {
  uint16_t raw = analogRead(BEMF3);
  uint16_t BEMF_ = map(raw, 0, 4095, 0, 4000); //max supply voltage is 40v DC
  return BEMF_;

}

/*****************************************************************************
  calculate the speed of the motor in sample_time period
  Sample time is entered in us so 2500 will be 2.5 ms
  returns nothing speed is saved in global variable sped
******************************************************************************/
void Get_speed(int sample_time) {

  static int32_t Prev_EncoderCount_v = 0;
  static uint32_t previousMicros_v = 0;
  uint32_t currentMicros_v = micros();

  if (currentMicros_v - previousMicros_v >= sample_time) {
    previousMicros_v = currentMicros_v;
    int32_t EncoderCount_v = BLDC_encoder.getEncoderCount();

    /*****************************************************************************
    N[RPM] = (d(change in angle) / sample_time) * (60/(2*pi))
    * We know that we have 10 bit encoder that means one rotation is equal to 1024 ticks
    * 360 deg / 1024 ticks = 0.35156 deg per tick or 6.135923e-3
    * N[RPM] = (d(change in ticks) / sample_time) * (60/(2*pi)) * 6.135923e-3
    * Since sample time is in microseconds forumla is:
    * N[RPM] = (d(change in ticks) / sample_time) * (60/(2*pi)) * 6.135923e-3 * 10e6
    * N[RPM] = Measured_speed = ((EncoderCount_v - Prev_EncoderCount_v) * 58573) / sample_time;
    * Minimum speed you can mesure with this forumla is 1 encoder tick / sample time or :
    * MIN_Measured_speed = 58573 / sample_time; For default value for sample_time of 3000
    * MIN_Measured_speed = 19.524 RPM
    ******************************************************************************/

    Measured_speed = ((EncoderCount_v - Prev_EncoderCount_v) * 58573) / sample_time; // *6134
    Prev_EncoderCount_v = EncoderCount_v ;

  }
}

/*****************************************************************************
  We sample current every sample_time and then low pass filter the values
  Result is "clean" current
******************************************************************************/
void Current_sample(int sample_time) {

  static int32_t Prev_EncoderCount_c = 0;
  static uint32_t previousMicros_c = 0;
  uint32_t currentMicros_c = micros();

  if (currentMicros_c - previousMicros_c >= sample_time) {
    previousMicros_c = currentMicros_c;
    Current_ = Get_current();
    Current_filter = f.filterIn(Current_);

    // View with Serial Plotter
    // Serial1.print(Current_, DEC);
    // Serial1.print(",");
    // Serial1.println(Current_filter);

  }
}

/*****************************************************************************
  We sample current every sample_time us and calculate average of last x samples (CircularBuffer<int, 6> buffer;)
  In case above every 6 samples. That can be changed in main file
  if average value is larger equal or a bit less then short circuit current raise error
******************************************************************************/
void Current_protection(int sample_time) {

  static int32_t Prev_EncoderCount_c = 0;
  static uint32_t previousMicros_c = 0;
  uint32_t currentMicros_c = micros();
  if (currentMicros_c - previousMicros_c >= sample_time) {

    previousMicros_c = currentMicros_c;
    buffer.push(Current_filter);
    int avg = 0;

    // the following ensures using the right type for the index variable
    using index_t = decltype(buffer)::index_t;
    for (index_t i = 0; i < buffer.size(); i++) {
      avg += buffer[i] / buffer.size();
    }
    if (avg >= short_circuit_current_reduced) {

      //Enter gravity compensation mode
      //mode = 4;
      Grav_current_threshold = 3;
      Grav_compliance_speed = 3;
      PID_position.SetTunings(3.2, 0, 0);
      Raise_error = 1;
      Raise_error_current = 1;

    }
  }

}


/*****************************************************************************
  Get temperature every sample time and check if it is within limits, if
  not raise error (10)
******************************************************************************/
void Temperature_sample(int sample_time) {

  static int32_t Prev_EncoderCount_c = 0;
  static uint32_t previousMicros_c = 0;
  uint32_t currentMicros_c = micros();

  if (currentMicros_c - previousMicros_c >= sample_time) {
    previousMicros_c = currentMicros_c;
    Get_temperature();
    if (Temperature_global >= Error_temperature) {
      Grav_current_threshold = 3;
      Grav_compliance_speed = 3;
      PID_position.SetTunings(3.2, 0, 0);
      Raise_error = 1;
      Raise_error_Temp = 10;
    }

  }

}


/*
  void Index_trigger(){
  Serial2.println("index triggered");
  index_trigger = 1;
  index_encoder_num = BLDC_encoder.getEncoderCount();
  Serial2.println(index_encoder_num);
  detachInterrupt(INDEX);
  }
*/
