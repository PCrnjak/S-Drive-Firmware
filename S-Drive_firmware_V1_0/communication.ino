
/*****************************************************************************
  Function that gets input from serial port 2.
  Modes and commands available:
  #### Commands need to be sent correctly or else it will be ignored. ####
  #### replace words with numbers.Kp and speed can be float values!

    1 - Go to position and hold:
        h(position),speed,Kp,current_threshold
        example: h100,20,3.1,12

    2 - Speed to position and sent flag
        s(position),speed

    3 - Gravitiy compensation mode
        g(current_threshold),compliance_speed

    4 - Position hold mode
        p(Kp),current_threshold

    5 - Speed mode with direction
        o(direction 0 or 1),speed

    6 - Jump to position
        j(position),Kp,current_threshold


    7 - Voltage mode
        v(direction 0 or 1),voltage(0-1000)

    8 - Disable motor
        d

    9 - Enable motor
        e

   10 - Clear error
        c

******************************************************************************/
void Get_input(void) {

  static uint8_t i_ = 0;
  static uint8_t i2 = 0;
  static char data[30];
  static char data2[30];
  static uint8_t comma = 1;
  static bool flag_2 = 0;

  static int out1_ = 0;
  static float out2_ = 0;
  static float out3_ = 0;
  static int out4_ = 0;
  static int out5_ = 0;


  if (Serial2.available() > 0) {

    data[i_] = Serial2.read();

    if (data[i_] == 'h' or data[i_] == 'p' or data[i_] == 's' or data[i_] == 'g' or data[i_] == 'd' or data[i_] == 'e' or data[i_] == 'o' or data[i_] == 'j' or data[i_] == 'c' or data[i_] == 'v') {
      flag_2 = 1;
    }
    if ( data[i_] == '\n') {
      flag_2 = 0;
    }

    if (i_ != 0) {
      switch (data[0]) {

        /*****************************************************************************
          GOTO position and hold (with error comp)
        ******************************************************************************/
        case 'h':
          data2[i2 - 1] = data[i_];

          if (data[i_]  == '\n' and comma == 4) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out4_ = atoi(data2);
            Error_current_threshold = out4_;
            Setpoint_speed = out2_;
            // if speed is really small we use position control to emulate speed control
            // Here we calculate needed sample time so we dont calculate it in main loop,
            // but only when we receive new one
            sample_time_speed = 6108652 / (Slow_speed_adjustment_variable * Setpoint_speed); // TODO
            Setpoint_true = out1_ - Sector_offset ;
            Kp_position = out3_ ; /// PID_position.SetTunings(out3_, 0, 0);// TODO dodaj tu ovih 0.06 kada drži poziciju
            PID_position.SetTunings(Kp_speed_emulation, 0, 0);
            comma = 1 ;
            i2 = 0;
            i_ = 0;


            if (Setpoint_true > EncoderCount) {
              Master_dir = 1;
              dir_ = 1;
            } else {
              Master_dir = 0;
              dir_ = 0;
            }

            mode = 1;
            flag_1 = 0;


          }

          if (data[i_]  == ',' and comma == 3) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out3_ = atof(data2);
            comma = comma + 1 ;
            i2 = 0;
          }

          if (data[i_]  == ',' and comma == 2) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out2_ = atof(data2);
            comma = comma + 1 ;
            i2 = 0;
          }


          if (data[i_]  == ',' and comma == 1) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out1_ = atoi(data2);
            comma = comma + 1 ;
            i2 = 0;
          }

          break;
        /******************************************************************************/

        /*****************************************************************************
           Holds current position (strong hold)
        ******************************************************************************/
        case 'p':

          data2[i2 - 1] = data[i_];

          if (data[i_]  == '\n' and comma == 2) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out1_ = atoi(data2);
            comma =  1 ;
            i2 = 0;
            i_ = 0;

            All_Enable_HIGH();

            Error_current_threshold = out1_;
            Kp_position = out2_ ;

            Setpoint = EncoderCount;
            Setpoint_true = EncoderCount;
            if (Setpoint_true > EncoderCount) {
              Master_dir = 1;
              dir_ = 1;
            } else {
              Master_dir = 0;
              dir_ = 0;
            }
            mode = 5;

          }

          if (data[i_]  == ',' and comma == 1) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out2_ = atof(data2);
            comma = comma + 1 ;
            i2 = 0;
          }

          break;
        /******************************************************************************/

        /*****************************************************************************
          Set speed and move there , at setpoint send command
        ******************************************************************************/
        case 's':

          data2[i2 - 1] = data[i_];

          if (data[i_]  == '\n' and comma == 2) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out2_ = atof(data2);
            comma =  1 ;
            i2 = 0;
            i_ = 0;

            flag_3 = 0;
            flag_1 = 0;
            Setpoint_speed = out2_;

            /*****************************************************************************
                Ts = 60/(1024 * speed[RPM]) since time Ts needs to be in microseconds
                Ts = 60 000 000 / (1024 * speed[RPM])
                Ts = 58593 / speed[RPM]
                6108652 / 58593 = 104.24 ~ 105
                So we can write 58593 / speed[RPM] as :
                sample_time_speed = 6108652 / (Slow_speed_adjustment_variable * Setpoint_speed);
            ******************************************************************************/

            sample_time_speed = 6108652 / (Slow_speed_adjustment_variable * Setpoint_speed);
            Setpoint_true = out1_ - Sector_offset;
            PID_position.SetTunings(Kp_speed_emulation, 0, 0);

            if (Setpoint_true > EncoderCount) {
              Master_dir = 1;
              dir_ = 1;
            } else {
              Master_dir = 0;
              dir_ = 0;
            }

            mode = 3;

          }

          if (data[i_]  == ',' and comma == 1) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out1_ = atoi(data2);
            comma = comma + 1 ;
            i2 = 0;
          }

          break;
        /******************************************************************************/

        /*****************************************************************************
          Set direction (0,1) and speed
        ******************************************************************************/
        case 'o':

          data2[i2 - 1] = data[i_];

          if (data[i_]  == '\n' and comma == 2) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out2_ = atof(data2);
            comma =  1 ;
            i2 = 0;
            i_ = 0;


            Setpoint_speed = out2_;
            // if speed is really small we use position control to emulate speed control
            // Here we calculate needed sample time so we dont calculate it in main loop,
            // but only when we receive new one

            /*****************************************************************************
                Ts = 60/(1024 * speed[RPM]) since time Ts needs to be in microseconds
                Ts = 60 000 000 / (1024 * speed[RPM])
                Ts = 58593 / speed[RPM]
                6108652 / 58593 = 104.24 ~ 105
                So we can write 58593 / speed[RPM] as :
                sample_time_speed = 6108652 / (Slow_speed_adjustment_variable * Setpoint_speed);
            ******************************************************************************/

            sample_time_speed = 6108652 / (Slow_speed_adjustment_variable * Setpoint_speed);
            Master_dir = out1_;
            dir_ = Master_dir;
            PID_position.SetTunings(Kp_speed_emulation, 0, 0);
            //  Setpoint = EncoderCount; // ovo razjebe sve?
            mode = 7;

          }

          if (data[i_]  == ',' and comma == 1) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out1_ = atoi(data2);
            comma = comma + 1 ;
            i2 = 0;
          }

          break;
        /******************************************************************************/

        /*****************************************************************************
          Gravity comp mode
        ******************************************************************************/
        case 'g':

          data2[i2 - 1] = data[i_];

          if (data[i_]  == '\n' and comma == 2) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out2_ = atoi(data2);
            comma =  1 ;
            i2 = 0;
            i_ = 0;

            All_Enable_HIGH();
            Setpoint = EncoderCount;

            Grav_current_threshold = out1_;
            Grav_compliance_speed = out2_ ;
            PID_position.SetTunings(3.2, 0, 0);
            mode = 4;

          }

          if (data[i_]  == ',' and comma == 1) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out1_ = atoi(data2);
            comma = comma + 1 ;
            i2 = 0;
          }

          break;
        /******************************************************************************/

        /*****************************************************************************
          Disable motor
        ******************************************************************************/
        case 'd':
          comma =  1;
          i2 = 0;
          i_ = 0;
          All_Enable_LOW();
          mode = 5;
          break;
        /******************************************************************************/

        /*****************************************************************************
          Enable motor ( and hold position )
        ******************************************************************************/
        case 'e':
          comma =  1 ;
          i2 = 0;
          i_ = 0;
          Setpoint = EncoderCount;
          Setpoint_true = EncoderCount;
          PID_position.SetTunings(3.2, 0, 0);
          All_Enable_HIGH();
          mode = 6;
          break;

        default:
          i_ = 0;
          i2 = 0;
          break;
        /******************************************************************************/

        /*****************************************************************************
          Jump to the position and hold
        ******************************************************************************/
        case 'j':

          data2[i2 - 1] = data[i_];

          if (data[i_]  == '\n' and comma == 2) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out2_ = atof(data2);
            comma =  1 ;
            i2 = 0;
            i_ = 0;

            All_Enable_HIGH();

            Setpoint = out1_ -  Sector_offset;
            Setpoint_true = out1_ - Sector_offset;
            Kp_position = out2_ ;
            PID_position.SetTunings(Kp_position , 0, 0.06);

            if (Setpoint_true > EncoderCount) {
              Master_dir = 1;
              dir_ = 1;
            } else {
              Master_dir = 0;
              dir_ = 0;
            }

            mode = 8;

          }

          if (data[i_]  == ',' and comma == 1) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out1_ = atoi(data2);
            comma = comma + 1 ;
            i2 = 0;
          }

          break;
        /******************************************************************************/

        /*****************************************************************************
          Clear error.
        ******************************************************************************/

        case 'c':

          Raise_error = 0;
          Raise_error_Temp = 0;
          Raise_error_current = 0;
          comma =  1 ;
          i2 = 0;
          i_ = 0;
          //When error is cleared motor stays in gravity mode. You need to send new mode command to enter it.
          Setpoint = EncoderCount;
          Grav_current_threshold = 3;
          Grav_compliance_speed = 3;
          mode = 4;

          break;
        /******************************************************************************/


        /*****************************************************************************
          Set direction (0,1) and voltage (it is scaled to value from 0-1000)
          0 = no voltage, 1000 max voltage. Basically use BLDC as a DC motor
        ******************************************************************************/
        case 'v':

          data2[i2 - 1] = data[i_];

          if (data[i_]  == '\n' and comma == 2) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out4_ = atoi(data2);
            comma =  1 ;
            i2 = 0;
            i_ = 0;


            Output = out4_;
            Master_dir = out1_;
            dir_ = Master_dir;
            //  Setpoint = EncoderCount; // ovo razjebe sve?
            mode = 9;

          }

          if (data[i_]  == ',' and comma == 1) {
            data[i_] == '\0';
            data2[i2 - 1] = data[i_];
            out1_ = atoi(data2);
            comma = comma + 1 ;
            i2 = 0;
          }

          break;
          /******************************************************************************/


      }
    }
    if (flag_2 == 1) {
      i_ = i_ + 1;
      i2 = i2 + 1;
    }
    if (flag_2 == 0) {
      i_ = 0;
      i2 = 0;
      comma = 1 ;
    }


  }
}



/*****************************************************************************
  Code that spits out valuable data every "sample_time"
  Data is sent like this:
  Position,Current,Speed,Temperature,Supply_voltage,error
******************************************************************************/
void Serial_diagnostic(int sample_time) {

  static int32_t Prev_EncoderCount_v = 0;
  static uint32_t previousMicros_v = 0;
  uint32_t currentMicros_v = micros();

  if (currentMicros_v - previousMicros_v >= sample_time) {
    previousMicros_v = currentMicros_v;
    uint16_t Vs = Get_supply_voltage();
    uint16_t BEMF_var = Get_BEMF1();
    Serial2.print("t");
    Serial2.print(EncoderCount + Sector_offset);
    Serial2.print(",");
    Serial2.print(Current_filter);
    Serial2.print(",");
    Serial2.print(Measured_speed);
    Serial2.print(",");
    Serial2.print(Temperature_global);
    Serial2.print(",");
    Serial2.print(Vs);
    Serial2.print(",");
    //Serial2.print(BEMF_var);
    //Serial2.print("\n");
    Serial2.print(Raise_error_Temp + Raise_error_current);
    Serial2.print("\n");

  }
}
