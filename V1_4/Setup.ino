/*****************************************************************************
  Output of this function is mode we will be starting in:
  * 1 - no homing
  * 2 - sector homing
  * 3 - incremental homing

******************************************************************************/

uint8_t first_mode_select() {

  read_EEPROM_data();
  static bool mode_flag = 0;
  static char mode_data ;

  Serial2.println("S-Drive BLDC motor controller");
  Serial2.println("");
  Serial2.println("Created by: Petar Crnjak");
  Serial2.println("Firmware Version: 1.0");
  Serial2.println("Boards supported: - S_drive V3.0");
  Serial2.println("                  - S_drive V3.1");
  Serial2.println("License: GPL-3.0");
  Serial2.println("");

  Serial2.println("Commands:");
  Serial2.println("b -- start the motor.");
  Serial2.println("s -- motor setup.");
  Serial2.println("d -- motor diagnostic.");
  Serial2.println("");


  while ( mode_flag == 0) {
    if (Serial2.available() > 0) {

      mode_data = Serial2.read();

      if (mode_data == 'd') {
        mode_flag = 0;
        diagnostic_mode();
        Serial2.println("");
        Serial2.println("Commands:");
        Serial2.println("b -- start the motor.");
        Serial2.println("s -- motor setup.");
        Serial2.println("d -- motor diagnostic.");
        Serial2.println("");
        
      }

      if (mode_data == 'b') {
        digitalWrite(LED, LOW);
        mode_flag = 1;
        read_EEPROM_data();
        // print a condition that says that motor started working
        set_ = EEPROM_dir;
        Sector_offset = EEPROM_aps;

      }

      if (mode_data == 's') {
        bool exit_flag = 0;
        bool exit_flag2 = 0;
        digitalWrite(LED, LOW);

        Serial2.println("");
        Serial2.println("Commands in setup mode:");
        Serial2.println("s -- sector homing.");
        Serial2.println("n -- no homing.");
        Serial2.println("i -- incremental homing.");
        Serial2.println("q -- quit setup mode");
        Serial2.println("");
        Serial2.println("After selecting mode follow instructions on display ");

        while ( exit_flag == 0) {
          if (Serial2.available() > 0) {
            mode_data = Serial2.read();

            if (mode_data == 'q') {
              exit_flag = 1;
              Serial2.println("");
              Serial2.println("Commands:");
              Serial2.println("b -- start the motor.");
              Serial2.println("s -- motor setup.");
              Serial2.println("d -- motor diagnostic.");
              Serial2.println("");
            }


            if (mode_data == 's') {
              Serial2.println("/------------------------------/");
              Serial2.println("Place motor in position that will be zero. When done type ""y"".");
              uint8_t step_var = 0;
              while (exit_flag2 == 0) {
                if (Serial2.available() > 0) {
                  mode_data = Serial2.read();

                  if (mode_data == 'y' and step_var == 1) {

                    set_ = dir_check();
                    Serial2.println("/------------------------------/");
                    Serial2.print("Dir var is: "); Serial2.println(set_);
                    Serial2.println("/------------------------------/");
                    Serial2.println("Sector homing is done!");
                    Lock_position(pwmSin[0], pwmSin[phaseShift], pwmSin[phaseShift_2], 0);
                    EEPROM_dir = set_;
                    EEPROM_mode = 2;
                    EEPROM_aps = Sector_offset;
                    write_EEPROM_data();

                    Serial2.println("");
                    Serial2.println("Commands:");
                    Serial2.println("b -- start the motor.");
                    Serial2.println("s -- motor setup.");
                    Serial2.println("d -- motor diagnostic.");
                    Serial2.println("");

                    exit_flag2 = 1;
                    exit_flag = 1;

                  }

                  if (mode_data == 'y' and step_var == 0) {
                    Sector_offset = enc.read(), DEC;
                    Serial2.println("/------------------------------/");
                    Serial2.print("Aps position is: "); Serial2.println(Sector_offset);
                    Serial2.println("/------------------------------/");
                    Serial2.println("Move motor to positon where it can move left and right. When done type ""y"".");
                    step_var = 1;
                  }
                }
              }
            }

            if (mode_data == 'n') {
              Serial2.println("/------------------------------/");
              Serial2.println("Place the motor shaft in a position where it can move left and right freely.When done tipe ""y"".");
              while (exit_flag2 == 0) {
                if (Serial2.available() > 0) {
                  mode_data = Serial2.read();
                  if (mode_data == 'y') {
                    exit_flag2 = 1;
                    set_ = dir_check();
                    Serial2.print("Dir var is: "); Serial2.println(set_);
                    Lock_position(pwmSin[0], pwmSin[phaseShift], pwmSin[phaseShift_2], 0);
                    EEPROM_mode = 1;
                    EEPROM_dir = set_;
                    write_EEPROM_data();
                    exit_flag = 1;

                    Serial2.println("");
                    Serial2.println("Commands:");
                    Serial2.println("b -- start the motor.");
                    Serial2.println("s -- motor setup.");
                    Serial2.println("d -- motor diagnostic.");
                    Serial2.println("");

                  }
                }
              }

            }

            //TODO
            if (mode_data == 'i') {
              // stupid method to home
              // will write later, maybe
              exit_flag = 1;
            }
          }
        }
      }
    }
  }
  return EEPROM_mode;
}




void diagnostic_mode() {

  Serial2.println("Status LED will stay on while in diagnostic mode");
  digitalWrite(LED, HIGH);

  Serial2.print("MCU clock speed -- "); Serial2.print(F_CPU); Serial2.println(" Hz");

  Serial2.println("/------------------------------/");

  if (myMem.begin(EEPROM_ADDRESS, Wire2) == false) {
    Serial2.println("No memory detected");
  } else {
    Serial2.println("Detected EEPROM memory");
  }

  uint32_t temp_mem = read_EEPROM_data();
  Serial2.print("Data in EEPROM -- "); Serial2.println(temp_mem);
  Serial2.print("Mode -- "); Serial2.println(EEPROM_mode); Serial2.print("dir is: ");
  Serial2.println(EEPROM_dir); Serial2.print("Apsolute homing position -- "); Serial2.println(EEPROM_aps);
  Serial2.print("Mem size in bytes -- ");
  Serial2.println(myMem.length());

  Serial2.println("/------------------------------/");

  if (!enc.begin ()) {
    Serial2.println ("Error setting up AS5040") ;
  } else {
    Serial2.println ("AS5040 encoder detected");
  }

  Serial2.print("Apsolute position -- "); Serial2.println (enc.read (), DEC) ;
  Serial2.print("Encoder status -- "); Serial2.println (enc.valid () ? "OK" : "Fault") ;
  Serial2.print("Encoder Zaxis -- ");  Serial2.println (enc.Zaxis ()) ;
  //BLDC_encoder.setEncoderCount(420);
  Serial2.print("Incremental encoder count -- ");  Serial2.println (BLDC_encoder.getEncoderCount()) ;

  Serial2.println("/------------------------------/");

  uint16_t Vs = Get_supply_voltage();
  Serial2.print("Supply voltage -- "); Serial2.print(Vs); Serial2.println("mV");
  Get_temperature();
  Serial2.print(F("Temperature -- ")); Serial2.print(Temperature_global); Serial2.println("°C");

  Serial2.println("/------------------------------/");

  Lock_position(pwmSin[0], pwmSin[phaseShift], pwmSin[phaseShift_2], 800);
  delay(100);
  uint16_t BEMF_var1 = Get_BEMF1();
  uint16_t BEMF_var2 = Get_BEMF2();
  uint16_t BEMF_var3 = Get_BEMF3();

  pwmWrite(PULS1, 0);
  pwmWrite(PULS2, 0);
  pwmWrite(PULS3, 0);


  Serial2.print("Bemf sensor 1 voltage -- "); Serial2.print(BEMF_var1); Serial2.println("mV");
  Serial2.print("Bemf sensor 2 voltage -- "); Serial2.print(BEMF_var2); Serial2.println("mV");
  Serial2.print("Bemf sensor 3 voltage -- "); Serial2.print(BEMF_var3); Serial2.println("mV");

  Serial2.println("/------------------------------/");
  
  Lock_position(pwmSin[0], pwmSin[phaseShift], pwmSin[phaseShift_2], 1800);
  delay(300);
  uint16_t SC_current = Get_current();
  Serial2.print("Short circuit current -- "); Serial2.print(SC_current); Serial2.println("mA");
  Lock_position(pwmSin[0], pwmSin[phaseShift], pwmSin[phaseShift_2], 0);
  Serial2.println("Diagnostic done!");



}


uint32_t read_EEPROM_data() {

  uint32_t memRead;
  digitalWrite(EEPROM_WP, HIGH);
  delay(10);
  myMem.get(1, memRead);
  EEPROM_aps = memRead % 10000;
  EEPROM_dir = (memRead / 10000) % 10;
  EEPROM_mode = (memRead % 1000000) / 100000;
  return memRead;


}

void write_EEPROM_data() {

  uint32_t EEPROM_data_pack;
  EEPROM_data_pack = EEPROM_aps + EEPROM_dir * 10000 + EEPROM_mode * 100000;
  digitalWrite(EEPROM_WP, LOW);
  myMem.put(1, EEPROM_data_pack);

}
