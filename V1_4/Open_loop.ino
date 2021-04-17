
/*****************************************************************************
  TODO dir check...
  // Legacy stuff maybe not working??
******************************************************************************/
bool BLDC_Setup(int delay_, int New_Max_DutyA , int steps_) {

  int incrm = 1;
  static int prev_encoder ;
  static bool dir_CW_ABC ;
  static bool dir_CW_ACB;
  static bool flag_setup = 0;
  static bool flag_end = 0;
  static bool flag_dir = 0;
  uint32_t currentMicros = 0;
  static uint32_t previousMicros = 0;

  while (flag_end == 0) {

    if (flag_setup == 0 ) {
      prev_encoder = BLDC_encoder.getEncoderCount();
      flag_setup = 1;
    }

    currentMicros = micros();
    if (currentMicros - previousMicros >= delay_) {

      previousMicros = currentMicros;

      A = map(pwmSin[currentStepA], 0, Max_Duty, 0, New_Max_DutyA);
      B = map(pwmSin[currentStepB], 0, Max_Duty, 0, New_Max_DutyA);
      C = map(pwmSin[currentStepC], 0, Max_Duty, 0, New_Max_DutyA);

      if (flag_dir == 0) {
        PWM_ABC();
      } else {
        PWM_ACB();
      }

      currentStepA = currentStepA + incrm ;
      currentStepB = currentStepB + incrm ;
      currentStepC = currentStepC + incrm ;

      if (currentStepB == lookup_table_size) {
        currentStepB = 0;
      }

      if (currentStepC == lookup_table_size) {
        currentStepC = 0;
      }

      if (currentStepA == steps_ and flag_dir == 0) {
        EncoderCount = BLDC_encoder.getEncoderCount();
        currentStepA = currentStepA_;
        currentStepB = currentStepB_;
        currentStepC = currentStepC_;
        flag_dir = 1;
        if (EncoderCount > prev_encoder ) {
          dir_CW_ABC = 1; // When shaft is rotating CW with ABC phase order ,encoder is counting up
        } else {
          dir_CW_ABC = 0;  // When shaft is rotating CW with ABC phase order ,encoder is counting down
        }

        delay(250);
      }

      if (currentStepA  == steps_ * 2  and flag_dir == 1) {
        EncoderCount = BLDC_encoder.getEncoderCount();
        currentStepA = currentStepA_;
        currentStepB = currentStepB_;
        currentStepC = currentStepC_;
        flag_end = 1;
        flag_dir = 0;
        flag_setup = 0;
        if (EncoderCount > prev_encoder ) {
          dir_CW_ACB = 1; // When shaft is rotating CW with ACB phase order ,encoder is counting up
        } else {
          dir_CW_ACB = 0;  // When shaft is rotating CW with ACB phase order ,encoder is counting down
        }

        flag_end = 1;
        previousMicros  = 0;
        delay(250);
        if ( dir_CW_ACB == 1) {

          return 1;

        } else {

          return 0;
        }

      }
    }
  }
}


/*****************************************************************************
  Input variables are : Number of steps we want, direction of rotation
  Time between each step ( in microseconds) , duty cycle of pwm wave(torque)
******************************************************************************/
void open_loop(int32_t _steps , bool dir_ , int Time_, int duty_) {

  uint32_t currentMicros = 0;
  static uint32_t previousMicros = 0;
  int total_steps;

  currentStepA = currentStepA_;
  currentStepB = currentStepB_;
  currentStepC = currentStepC_;

  while ( total_steps < _steps) {
    currentMicros = micros();
    if (currentMicros - previousMicros >= Time_) {
      previousMicros = currentMicros;
      A = map(pwmSin[currentStepA], 0, Max_Duty, 0, duty_);
      B = map(pwmSin[currentStepB], 0, Max_Duty, 0, duty_);
      C = map(pwmSin[currentStepC], 0, Max_Duty, 0, duty_);
      if (dir_ == 1 ) {
        PWM_ABC();
      } else {
        PWM_ACB();
      }

      currentStepA = currentStepA + 1;
      currentStepB = currentStepB + 1;
      currentStepC = currentStepC + 1;

      total_steps = total_steps + 1;

      Serial2.print("Step: ");
      Serial2.println(total_steps);

      if (currentStepA > lookup_table_size) {
        currentStepA = 0;
      }

      if (currentStepB > lookup_table_size) {
        currentStepB = 0;
      }

      if (currentStepC > lookup_table_size) {
        currentStepC = 0;
      }

    }
  }

  currentStepA = currentStepA_;
  currentStepB = currentStepB_;
  currentStepC = currentStepC_;


}

/*****************************************************************************
  This works dont touch!!
******************************************************************************/
bool dir_check() {

  int16_t temp_enc = BLDC_encoder.getEncoderCount();
  /// ALWAYS KEEP DIR 0 HERE !!!!
  open_loop(100 , 1 , 2000, 900);
  int16_t temp_enc_2 = BLDC_encoder.getEncoderCount();
  if (temp_enc > temp_enc_2) {
    return 1;
  } else {
    return 0;
  }
}
