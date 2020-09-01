/*****************************************************************************
  Gravity compensation code
  We can move the actuator freely and when we stop it holds that position
******************************************************************************/

void Gravity_compensation() {

  static int32_t Prev_EncoderCount_t = 0;
  static uint32_t previousMicros_t = 0;
  uint32_t currentMicros_t = micros();

  if (currentMicros_t - previousMicros_t >= Grav_time) {
    previousMicros_t = currentMicros_t;

    if (Current_filter >= Grav_current_threshold and EncoderCount > Setpoint + Grav_setpoint_offset ) {
      Setpoint = Setpoint + Grav_compliance_speed;
    } else if (Current_filter >= Grav_current_threshold and EncoderCount < Setpoint - Grav_setpoint_offset ) {
      Setpoint = Setpoint - Grav_compliance_speed;
    }

    else if (Current_filter == 0 ) {
      Setpoint = Setpoint;
    }
    

  }
}


/*****************************************************************************
  This is used if we want to hold a position under load , regult PID position loop
  is not enough
******************************************************************************/
void Error_correction() {

  static int32_t Prev_EncoderCount_t = 0;
  static uint32_t previousMicros_t = 0;
  uint32_t currentMicros_t = micros();

  if (currentMicros_t - previousMicros_t >= Error_time) {
    previousMicros_t = currentMicros_t;
    PID_position.SetTunings(Kp_position, 0, 0.06); //3.2, 0, 0.06);

    if (Current_filter >= Error_current_threshold and EncoderCount > Setpoint_true + Error_setpoint_offset ) {
      Setpoint = Setpoint - Error_Setpoint_add;
    } else if (Current_filter >= Error_current_threshold and EncoderCount < Setpoint_true - Error_setpoint_offset ) {
      Setpoint = Setpoint + Error_Setpoint_add;
    }
    else if (Current_filter < Error_current_threshold) {
      Setpoint = Setpoint_true;
    }

  }
}


/*****************************************************************************
  Gravity compensation mode
******************************************************************************/
void Gravitiy_compensation_mode() {

  PID_position.SetSampleTime(Grav_PID_band);
  Gravity_compensation();

  Input = BLDC_encoder.getEncoderCount();
  if (Input < Setpoint ) {
    PID_position.SetControllerDirection(DIRECT);
    dir_ = 1;
  }
  if (Input > Setpoint ) {
    PID_position.SetControllerDirection(REVERSE);
    dir_ = 0;
  }

  PID_position.Compute();

}


/*****************************************************************************
  Normal position hold mode. If under load it will not reach desired position
  But because of that it can act like a spring .
******************************************************************************/
void Normal_position_hold() {
  PID_position.SetSampleTime(PID_band_pos);
  Input = EncoderCount;

  if (Input < Setpoint) {
    PID_position.SetControllerDirection(DIRECT);
    dir_ = 1;
  }
  if (Input > Setpoint) {
    PID_position.SetControllerDirection(REVERSE);
    dir_ = 0;
  }

  PID_position.Compute();

}



///TODO TEST IF KP,KI,KD CHANGE IF I CHANGE THEM HERE // PID_position.SetTunings(Kp_position, 0, 0);
/// DONE , RADI

/*****************************************************************************
  Position hold mode
******************************************************************************/
void Position_hold_mode() {
  PID_position.SetSampleTime(Error_PID_band);
  Error_correction();

  Input = EncoderCount;

  if (Input < Setpoint) {
    PID_position.SetControllerDirection(DIRECT);
    dir_ = 1;
  }
  if (Input > Setpoint) {
    PID_position.SetControllerDirection(REVERSE);
    dir_ = 0;
  }

  PID_position.Compute();

}

/*****************************************************************************
  Normal speed control that uses PID regulator
  Has a minimum speed of around 50 rpm
******************************************************************************/
void PID_speed_mode() {
  Input = abs(Measured_speed);
  PID_speed.Compute();
  dir_ = Master_dir;

}

/*****************************************************************************
  We run BLDC motor as a stepper
  Min speed that is not too jerky is around 7 RPM
  sample_time in us
******************************************************************************/
void Slow_speed() {

  static uint32_t previousMicros_t = 0;
  uint32_t currentMicros_t = micros();

  if (currentMicros_t - previousMicros_t >= sample_time_speed) {
    previousMicros_t = currentMicros_t;
    PID_position.SetTunings(20, 0, 0);
    if (Master_dir == 1) {
      Setpoint = Setpoint + 1;
      dir_ = 1;
    }

    if (Master_dir == 0) {
      Setpoint = Setpoint - 1;
      dir_ = 0;
    }
  }

}


/*****************************************************************************
  Speed control
  TODO  reset integral windup kada idem sa veće brzine na manju
******************************************************************************/

void Master_speed_control() {
  if (Setpoint_speed < 120) {
    Slow_speed();
    Normal_position_hold();

  } else {
    PID_speed_mode();
    Setpoint = EncoderCount;
  }

}


/*****************************************************************************
  Go to position "Setpoint_true" with Speed_ at M_dir direction
******************************************************************************/
void GOTO_position_speed() {

  if (flag_1 == 0 ) {
    Master_speed_control();


    if (Master_dir == 1) {
      if (EncoderCount >=  Setpoint_true) {
        Setpoint =  Setpoint_true;
        flag_1 = 1;
      }
    } else {
      if (EncoderCount <=  Setpoint_true) {
        Setpoint =  Setpoint_true;
        flag_1 = 1;
      }
    }

  } else {
    Position_hold_mode();

  }
}


void GOTO_position_speed_no_hold() {

  Master_speed_control();

  if (Master_dir == 1) {
    if (EncoderCount >=  Setpoint_true) {
      // Setpoint = Setpoint_true_;
      flag_1 = 1;
    }
  } else {
    if (EncoderCount <=  Setpoint_true) {
      // Setpoint = Setpoint_true_;
      flag_1 = 1;
    }
  }

  if (flag_1 == 1 and flag_3 == 0) {
    //Serial2.println("d novim koordinatama");

    /// pošalji zahtjev za novim koordinatama. // Serial.print("bla bla");
    flag_3 = 1; // kada receive novi zahtjev tada se ovo miće u 0 , znači u serial receive funkciji
  }

}
