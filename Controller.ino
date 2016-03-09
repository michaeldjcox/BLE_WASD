// Stores the state of the cable / bluetooth switch
volatile boolean bluetooth = true;
volatile boolean usb = false;

/**
 * Sends the current key report either over bluetooth or USB cable
 */
void send_key_report() {
  KeyReport keyReport = getKeyReport();
  if (bluetooth) {
    send_report(keyReport);
  } else if (usb) {
    log_key_report(keyReport);
    Keyboard.sendReport(&keyReport);
  } else {
    clear_key_report();
  }
}

/**
 * Sends the current consumer key reports either over bluetooth or USB cable
 */
void send_consumer_report() {
  KeyReport consumerReport = getConsumerReport();
  uint8_t mediaHidKey = getMediaHidKey();
  if (bluetooth) {
    send_media(mediaHidKey);
  } else if (usb) {
    log_key_report(consumerReport);
    Keyboard.sendReport(&consumerReport);
  } else {
    clear_consumer_report();
  }
}

/**
 * Clears all currently set modifiers - not sure if this is really needed
 */
void clear_all() {
  clear_key_report();
  send_key_report();
  send_ps2_msg((byte) PS2_SET_RESET_LEDS);
  clear_consumer_report();
  send_consumer_report();
}

/**
 * Switches mode from bluetooth to USB and vice versa
 */
void switch_mode() {
  if (bluetooth) {
    clear_all();
    bluetooth = false;
    clear_all();
    stop_BLE();
    Keyboard.begin();
    usb = true;
  } else {
    clear_all();
    usb = false;
    clear_all();
    Keyboard.end();
    start_BLE(0);
    bluetooth = true;
  }
}

/**
 * Performs a hard reset for the current mode - USB or bluetooth
 */
void reconfigure() {
  if (bluetooth) {
    clear_all();
    bluetooth = false;
    clear_all();
    stop_BLE();
    start_BLE(1);
    bluetooth = true;
  } else {
    clear_all();
    usb = false;
    clear_all();
    Keyboard.end();
    Keyboard.begin();
    usb = true;
  }
}

void start_keyboard() {
  if (bluetooth) {
    if (DEBUG) {
      Serial.println(F("Starting as Bluetooth keyboard"));
    }
    start_BLE(1);
  } else {
    if (DEBUG) {
      Serial.println(F("Starting as USB keyboard"));
    }
    Keyboard.begin();
  }
}




