/*****************************************************************************
  Moves bldc in set direction
******************************************************************************/
void move_BLDC(int direction_) {

  if (set_ == 0) {
    if ( direction_ == 1 ) {
      if ( EncoderCount < 1024 and EncoderCount >= 0) {
        out = EncoderCount;
      }
      if ( EncoderCount >= 1024 ) {
        out =  EncoderCount - ( EncoderCount / 1024) * 1024 ;
      }
      if ( EncoderCount  > -1024  and EncoderCount < 0) {
        out = 1024 + EncoderCount ;
      }
      if ( EncoderCount <= -1024 ) {
        out = EncoderCount + abs(1024 * (EncoderCount / 1024)) + 1024;
        if (out == 1024) {
          out =  0;
        }
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE A)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_1)) ) {
        currentStepA = abs((lookup_table_size) - ((offset_1) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_1))) {
        currentStepA = out + (offset_1);
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE B)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_1 + phaseShift)) ) {
        currentStepB = abs((lookup_table_size) - ((offset_1 + phaseShift) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_1 + phaseShift))) {
        currentStepB = out + (offset_1 + phaseShift);
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE C)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_1 + phaseShift_2)) ) {
        currentStepC = abs((lookup_table_size) - ((offset_1 + phaseShift_2) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_1 + phaseShift_2))) {
        currentStepC = out + (offset_1 + phaseShift_2);
      }

      EncoderCount = BLDC_encoder.getEncoderCount();

      A = map(pwmSin[abs(currentStepA)], 0, Max_Duty, 0, New_Max_Duty);
      B = map(pwmSin[abs(currentStepB)], 0, Max_Duty, 0, New_Max_Duty);
      C = map(pwmSin[abs(currentStepC)], 0, Max_Duty, 0, New_Max_Duty);

      PWM_ABC();

    } else {
      if ( EncoderCount < 1024 and EncoderCount >= 0) {
        out = 1024 - EncoderCount ;  /// good
      }
      if ( EncoderCount >= 1024 ) {
        out = -EncoderCount + abs(1024 * (EncoderCount / 1024)) + 1024;
        if (out == 1024) {
          out =  0;
        }
      }
      if ( EncoderCount  > -1024  and EncoderCount < 0) {
        out = abs(EncoderCount) ; // good
      }
      if ( EncoderCount <= -1024 ) {
        out =  abs(EncoderCount) + ( EncoderCount / 1024) * 1024 ; // good
        if (out == 1024) {
          out =  0;
        }
      }

      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE A)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_0)) ) {
        currentStepA = abs((lookup_table_size) - ((offset_0) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_0))) {
        currentStepA = out + (offset_0);
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE B)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_0 + phaseShift)) ) {
        currentStepB = abs((lookup_table_size) - ((offset_0 + phaseShift) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_0 + phaseShift))) {
        currentStepB = out + (offset_0 + phaseShift);
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE C)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_0 + phaseShift_2)) ) {
        currentStepC = abs((lookup_table_size) - ((offset_0 + phaseShift_2) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_0 + phaseShift_2))) {
        currentStepC = out + (offset_0 + phaseShift_2);
      }

      EncoderCount = BLDC_encoder.getEncoderCount();

      A = map(pwmSin[abs(currentStepA)], 0, Max_Duty, 0, New_Max_Duty);
      B = map(pwmSin[abs(currentStepB)], 0, Max_Duty, 0, New_Max_Duty);
      C = map(pwmSin[abs(currentStepC)], 0, Max_Duty, 0, New_Max_Duty);

      PWM_ACB();
    }
  } else {
    if ( direction_ == 1 ) {
      if ( EncoderCount < 1024 and EncoderCount >= 0) {
        out = EncoderCount;
      }
      if ( EncoderCount >= 1024 ) {
        out =  EncoderCount - ( EncoderCount / 1024) * 1024 ;
      }
      if ( EncoderCount  > -1024  and EncoderCount < 0) {
        out = 1024 + EncoderCount ;
      }
      if ( EncoderCount <= -1024 ) {
        out = EncoderCount + abs(1024 * (EncoderCount / 1024)) + 1024;
        if (out == 1024) {
          out =  0;
        }
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE A)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_1)) ) {
        currentStepA = abs((lookup_table_size) - ((offset_1) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_1))) {
        currentStepA = out + (offset_1);
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE B)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_1 + phaseShift_2)) ) {
        currentStepB = abs((lookup_table_size) - ((offset_1 + phaseShift_2) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_1 + phaseShift_2))) {
        currentStepB = out + (offset_1 + phaseShift_2);
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE C)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_1 + phaseShift)) ) {
        currentStepC = abs((lookup_table_size) - ((offset_1 + phaseShift) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_1 + phaseShift))) {
        currentStepC = out + (offset_1 + phaseShift);
      }

      EncoderCount = BLDC_encoder.getEncoderCount();

      A = map(pwmSin[abs(currentStepA)], 0, Max_Duty, 0, New_Max_Duty);
      B = map(pwmSin[abs(currentStepB)], 0, Max_Duty, 0, New_Max_Duty);
      C = map(pwmSin[abs(currentStepC)], 0, Max_Duty, 0, New_Max_Duty);

      PWM_ABC();

    } else {
      if ( EncoderCount < 1024 and EncoderCount >= 0) {
        out = 1024 - EncoderCount ;  /// good
      }
      if ( EncoderCount >= 1024 ) {
        out = -EncoderCount + abs(1024 * (EncoderCount / 1024)) + 1024;
        if (out == 1024) {
          out =  0;
        }
      }
      if ( EncoderCount  > -1024  and EncoderCount < 0) {
        out = abs(EncoderCount) ; // good
      }
      if ( EncoderCount <= -1024 ) {
        out =  abs(EncoderCount) + ( EncoderCount / 1024) * 1024 ; // good
        if (out == 1024) {
          out =  0;
        }
      }

      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE A)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_0)) ) {
        currentStepA = abs((lookup_table_size) - ((offset_0) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_0))) {
        currentStepA = out + (offset_0);
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE B)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_0 + phaseShift_2)) ) {
        currentStepB = abs((lookup_table_size) - ((offset_0 + phaseShift_2) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_0 + phaseShift_2))) {
        currentStepB = out + (offset_0 + phaseShift_2);
      }
      ////////////////////////////////////////////////////////////////////////////
      // offsets  maped encoder ticks by needed phase shift offsets (FOR PHASE C)
      // shift to the left , positive shift
      if (out >= (lookup_table_size - (offset_0 + phaseShift)) ) {
        currentStepC = abs((lookup_table_size) - ((offset_0 + phaseShift) + out));
      }
      if (out >= 0 and out < (lookup_table_size - (offset_0 + phaseShift))) {
        currentStepC = out + (offset_0 + phaseShift);
      }

      EncoderCount = BLDC_encoder.getEncoderCount();

      A = map(pwmSin[abs(currentStepA)], 0, Max_Duty, 0, New_Max_Duty);
      B = map(pwmSin[abs(currentStepB)], 0, Max_Duty, 0, New_Max_Duty);
      C = map(pwmSin[abs(currentStepC)], 0, Max_Duty, 0, New_Max_Duty);

      PWM_ACB();

    }
  }
}
