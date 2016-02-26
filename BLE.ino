





void start_BLE(bool reset) {

  ble.begin(DEBUG); //verbose on-off


  if (reset) {
    /* Initialise the module */
    if(DEBUG){
       Serial.print(F("Initialising the Bluefruit LE module: "));
       Serial.println(F("Performing a factory reset: "));
    }
    ble.factoryReset();

    //Disable command echo from Bluefruit
    ble.echo(false);

    ble.info();

    //rename device
    ble.println("AT+GAPDEVNAME=BLE_WASD");

    /* Enable HID Service */
    if(DEBUG){
      Serial.println(F("Enable HID Service (including Keyboard): "));
    }
    if (! ble.sendCommandCheckOK(F( "AT+BleKeyboardEn=On"  ))) {
      if(DEBUG){
        Serial.println(F("Could not enable Keyboard"));
      }
    }

    /* Add or remove service requires a reset */
    if (! ble.reset() ) {
      if(DEBUG){
        Serial.println(F("Couldn't reset??"));
      }
    }

  }

}

void send_report() {  
  String cmd = "AT+BLEKEYBOARDCODE=" + 
                hex_to_str(report.modifiers) + 
                "-00-" + 
                hex_to_str(report.keys[0]) + "-" +
                hex_to_str(report.keys[1]) + "-" +
                hex_to_str(report.keys[2]) + "-" +
                hex_to_str(report.keys[3]) + "-" +
                hex_to_str(report.keys[4]) + "-" +
                hex_to_str(report.keys[5]);
                
    ble.println(cmd);  

    if(DEBUG){
      Serial.println(cmd);
    }
}


uint8_t is_modifier(uint8_t c, uint8_t is_long_key) {
  if (is_long_key) {
    switch (c) {
      case (PS2_LEFT_GUI):
      case (PS2_RIGHT_CTRL):
      case (PS2_RIGHT_ALT):
      case (PS2_RIGHT_GUI):
        return 1;
        break;
    }
  }
  else {
    switch (c) {
      case PS2_LEFT_CTRL:
      case PS2_LEFT_SHIFT:
      case PS2_LEFT_ALT:
      case PS2_RIGHT_SHIFT:
        return 1;
        break;
    }
  }
  return 0;
}

uint8_t is_media(uint8_t c) {
  switch (c) {
    case (PS2_PLAY_PAUSE):
    case (PS2_STOP):
    case (PS2_NEXT_TRACK):
    case (PS2_PREV_TRACK):
    case (PS2_VOL_UP):
    case (PS2_VOL_DWN):
    case (PS2_MUTE):
      return 1;
      break;
  }
  return 0;
}

void send_media(uint8_t c) {
  String str;
  switch (c) {
    case (PS2_PLAY_PAUSE):
      str = "PLAYPAUSE";
      break;
    case (PS2_STOP):
      str = "MEDIASTOP";
      break;
    case (PS2_NEXT_TRACK):
      str = "MEDIANEXT";
      break;
    case (PS2_PREV_TRACK):
      str = "MEDIAPREVIOUS";
      break;
    case (PS2_VOL_UP):
      str = "VOLUME+,100";
      break;
    case (PS2_VOL_DWN):
      str = "VOLUME-,100";
      break;
    case (PS2_MUTE):
      str = "MUTE";
      break;
  }
  ble.println("AT+BLEHIDCONTROLKEY=" + str);
}

void set_modifier(uint8_t c, uint8_t is_long_key) {
  if (is_long_key) {
    switch (c) {
      case (PS2_LEFT_GUI):
        modifiers |= 1 << 3;
        break;
      case (PS2_RIGHT_CTRL):
        modifiers |= 1 << 4;
        break;
      case (PS2_RIGHT_ALT):
        modifiers |= 1 << 6;
        break;
      case (PS2_RIGHT_GUI):
        modifiers |= 1 << 7;
        break;
    }
  }
  else {
    switch (c) {
      case PS2_LEFT_CTRL:
        modifiers |= 1 << 0;
        break;
      case PS2_LEFT_SHIFT:
        modifiers |= 1 << 1;
        break;
      case PS2_LEFT_ALT:
        modifiers |= 1 << 2;
        break;
      case PS2_RIGHT_SHIFT:
        modifiers |= 1 << 5;
        break;
    }
  }
}

void unset_modifier(uint8_t c, uint8_t is_long_key) {
  if (is_long_key) {
    switch (c) {
      case (PS2_LEFT_GUI):
        modifiers &= ~(1 << 3);
        break;
      case (PS2_RIGHT_CTRL):
        modifiers &= ~(1 << 4);
        break;
      case (PS2_RIGHT_ALT):
        modifiers &= ~(1 << 6);
        break;
      case (PS2_RIGHT_GUI):
        modifiers &= ~(1 << 7);
        break;
    }
  }
  else {
    switch (c) {
      case PS2_LEFT_CTRL:
        modifiers &= ~(1 << 0);
        break;
      case PS2_LEFT_SHIFT:
        modifiers &= ~(1 << 1);
        break;
      case PS2_LEFT_ALT:
        modifiers &= ~(1 << 2);
        break;
      case PS2_RIGHT_SHIFT:
        modifiers &= ~(1 << 5);
        break;
    }
  }
}


