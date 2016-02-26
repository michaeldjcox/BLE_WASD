/*
 * Functions associated with BLE communication
 */

// Stores the parts of the BLE key command
KeyReport report;

/**
 * Start the low energy bluetooth module
 */
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

/**
 * Updates the key report given a key press
 * 
 * Up to six keys can be simultanesouly pressed plus the modifier keys
 * 
 */
void report_add(uint8_t k) {
  uint8_t i;
  if (k >= PS2_EXTENDED) {
    report.modifiers |= 1 << (k - PS2_EXTENDED);
  } else if (report.keys[0] != k && report.keys[1] != k &&
             report.keys[2] != k && report.keys[3] != k &&
             report.keys[4] != k && report.keys[5] != k) {
    for (i = 0; i < 6; ++i) {
      if (report.keys[i] == 0) {
        report.keys[i] = k;
        break;
      }
    }
  }
}

/**
 * Updates the key report given a key release
 */
void report_remove(uint8_t k) {
  uint8_t i;
  if (k >= PS2_EXTENDED) {
    report.modifiers &= ~(1 << (k - PS2_EXTENDED));
  } else {
    for (i = 0; i < 6; ++i) {
      if (report.keys[i] == k) {
        report.keys[i] = 0;
        break;
      }
    }
  }
}

/**
 * Sends the key report over BLE
 */
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

/**
 * Create a two character hex string for debugging
 */
String hex_to_str(uint8_t hex) {
  String str = String(hex, HEX);
  if (hex < 16) {
    str = "0" + str;
  }
  return str;
}

/**
 * Tests if a HID key code corresponds to a media key
 */
uint8_t is_media(uint8_t c) {
  
  switch (c) {
    case (HID_PLAY_PAUSE):
    case (HID_STOP):
    case (HID_NEXT_TRACK):
    case (HID_PREV_TRACK):
    case (HID_VOL_UP):
    case (HID_VOL_DWN):
    case (HID_MUTE):
      return 1;
      break;
  }
  return 0;
}

/**
 * Sends a media control key (rather than the HID key code)
 */
void send_media(uint8_t c) {
  String str = "AT+BLEHIDCONTROLKEY=";
  switch (c) {
    case (HID_PLAY_PAUSE):
      str += "PLAYPAUSE";
      break;
    case (HID_STOP):
      str = "MEDIASTOP";
      break;
    case (HID_NEXT_TRACK):
      str = "MEDIANEXT";
      break;
    case (HID_PREV_TRACK):
      str = "MEDIAPREVIOUS";
      break;
    case (HID_VOL_UP):
      str = "VOLUME+,100";
      break;
    case (HID_VOL_DWN):
      str = "VOLUME-,100";
      break;
    case (HID_MUTE):
      str = "MUTE";
      break;
  }
  ble.println(str);

  if(DEBUG){
    Serial.println(str);
  }
}


