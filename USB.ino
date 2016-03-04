
// Stores the parts of the USB HID key command

KeyReport keyReport;
KeyReport consumerReport;
uint8_t media_hid_key;

volatile boolean bluetooth=true;
volatile boolean usb=true;

/**
 * Updates the key report given a key press
 * 
 * Up to six keys can be simultanesouly pressed plus the modifier keys
 * 
 */
void report_add(uint8_t k) {
  uint8_t i;
  if (k >= HID_LEFT_CTRL) {
    keyReport.modifiers |= 1 << (k - HID_LEFT_CTRL);
  } else if (keyReport.keys[0] != k && keyReport.keys[1] != k &&
             keyReport.keys[2] != k && keyReport.keys[3] != k &&
             keyReport.keys[4] != k && keyReport.keys[5] != k) {
    for (i = 0; i < 6; ++i) {
      if (keyReport.keys[i] == 0) {
        keyReport.keys[i] = k;
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
  if (k >= HID_LEFT_CTRL) {
    keyReport.modifiers &= ~(1 << (k - HID_LEFT_CTRL));
  } else {
    for (i = 0; i < 6; ++i) {
      if (keyReport.keys[i] == k) {
        keyReport.keys[i] = 0;
        break;
      }
    }
  }
}

/**
 * Test modifiers to see if shift is currently pressed
 */
bool isShift() {
  return keyReport.modifiers == 0x02;
}

/**
 * Test modifiers to see if control is currently pressed
 */
bool isControl() {
  return keyReport.modifiers == 0x01;
}

/**
 * Test modifiers to see if control and shift are currently pressed
 */
bool isControlShift() {
  return keyReport.modifiers == 0x03;
}

/**
 * Clears all currently set modifiers - not sure if this is really needed
 */
void clear_modifiers() {
  keyReport.modifiers = 0;
}

/**
 * Adds the specified consumer key to the consumer key report
 */
void consumer_add(uint8_t k) {
  media_hid_key= k;
  consumerReport.modifiers=0x00;
  consumerReport.reserved=0x02;
      switch (k) {
      case (HID_PLAY_PAUSE):
        consumerReport.keys[0]=0x40;
        consumerReport.keys[1]=0x00;
        break;
      case (HID_STOP):
        consumerReport.keys[0]=0x00;
        consumerReport.keys[1]=0x10;
        break;
      case (HID_NEXT_TRACK):
        consumerReport.keys[0]=0x00;
        consumerReport.keys[1]=0x02;
        break;
      case (HID_PREV_TRACK):
        consumerReport.keys[0]=0x00;
        consumerReport.keys[1]=0x04;
        break;
      case (HID_VOL_UP):
        consumerReport.keys[0]=0x10;
        consumerReport.keys[1]=0x00;
        break;
      case (HID_VOL_DWN):
        consumerReport.keys[0]=0x20;
        consumerReport.keys[1]=0x00;
        break;
      case (HID_MUTE):
        // Don't know this one...
        consumerReport.keys[0]=0x00;
        consumerReport.keys[1]=0x00;
        break;
    }   
    consumerReport.keys[2]=0x00;
    consumerReport.keys[3]=0x00;
    consumerReport.keys[4]=0x00;
    consumerReport.keys[5]=0x00;
}

/**
 * Removes the specified consumer key to the consumer key report
 */
void consumer_remove(uint8_t k) {
  media_hid_key= 0;
  consumerReport.modifiers=0x00;  
  consumerReport.reserved=0x02;
  consumerReport.keys[0]=0x00;
  consumerReport.keys[1]=0x00;
  consumerReport.keys[2]=0x00;
  consumerReport.keys[3]=0x00;
  consumerReport.keys[4]=0x00;
  consumerReport.keys[5]=0x00;  
}

/**
 * Sends the current key report either over bluetooth or USB cable
 */
void send_report() {
  if (bluetooth) {
    send_report(keyReport);
  } 

 if (usb) {
    logKeyReport(keyReport);
    Keyboard.sendReport(&keyReport);
  }  
}

/**
 * Sends the current consumer key reports either over bluetooth or USB cable
 */
void send_consumer_report() {
  if (bluetooth) {
    send_media(media_hid_key);
  } 
  
  if (usb) {
    logKeyReport(consumerReport);
    Keyboard.sendReport(&consumerReport);
  }
}

/**
 * Outputs a debug string for the current key report
 */
void logKeyReport(KeyReport report) {
  if(DEBUG) {
    String cmd = "0xFD-" + reportToString(report);
    Serial.println(cmd);
  }
}

/**
 * Returns a string representation of a key report
 */
String reportToString(KeyReport report) {
      String cmd = hex_to_str(report.modifiers) + 
      hex_to_str(report.reserved) + "-" + 
      hex_to_str(report.keys[0]) + "-" +
      hex_to_str(report.keys[1]) + "-" +
      hex_to_str(report.keys[2]) + "-" +
      hex_to_str(report.keys[3]) + "-" +
      hex_to_str(report.keys[4]) + "-" +
      hex_to_str(report.keys[5]);
      return cmd;
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
 * Switches mode from bluetooth to USB and vice versa
 */
void switchMode() {
  if (bluetooth) {
    bluetooth=false;
    stop_BLE();
    Keyboard.begin();
    usb=true;    
  } else {
    usb=false;
    Keyboard.end();
    start_BLE(0); 
    bluetooth=true;
  }
}

/**
 * Performs a hard reset for the current mode - USB or bluetooth
 */
void reset() {
  if (bluetooth) {
    bluetooth=false;
    stop_BLE();
    start_BLE(1);
  } else {
    usb=false;
    Keyboard.end();
    Keyboard.begin();
  } 
}



