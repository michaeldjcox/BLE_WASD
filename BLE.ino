/*
 * Functions associated with BLE communication
 */

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
 * Stops bluetooth
 */
void stop_BLE() {
  ble.end();
}

/**
 * Sends the key report over BLE
 */
void send_report(KeyReport report) {  
    String cmd = "AT+BLEKEYBOARDCODE=" + reportToString(report);
    ble.println(cmd);  
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
      str += "MEDIASTOP";
      break;
    case (HID_NEXT_TRACK):
      str += "MEDIANEXT";
      break;
    case (HID_PREV_TRACK):
      str += "MEDIAPREVIOUS";
      break;
    case (HID_VOL_UP):
      str += "VOLUME+,100";
      break;
    case (HID_VOL_DWN):
      str += "VOLUME-,100";
      break;
    case (HID_MUTE):
      str += "MUTE";
      break;
  }
  ble.println(str);
}


