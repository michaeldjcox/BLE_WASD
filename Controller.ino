/*
 * Controller responsible for tracking whether we are in wired or wireless mode and routing key presses
 * accordingly
 */

// Stores the state of the cable / bluetooth switch
volatile boolean bluetooth = true;
volatile boolean usb = false;

/**
 * Sends the current key report either over bluetooth or USB cable
 */
void send_key_report() {
  KeyReport keyReport = getKeyReport();
  if (bluetooth) {
    send_ble_report(keyReport);
  } else if (usb) {
    send_usb_report(keyReport);
  } else {
    clear_key_report();
  }
}

/**
 * Sends the current consumer key reports either over bluetooth or USB cable
 */
void send_media_control(uint8_t mediaHidKey) {
  if (bluetooth) {
    send_ble_media(mediaHidKey);
  } else if (usb) {
    send_usb_media(mediaHidKey);
  }
}

/**
 * Clears all currently set modifiers - not sure if this is really needed
 */
void clear_all() {
  clear_key_report();
  send_key_report();
  clear_LEDs();
}

/**
 * Starts wireless mode ensuring that we don't process key presses until we are ready
 */
void startup_BLE(boolean reset) {
  start_BLE();
  if (reset) {
    reconfigure_BLE();
  }
  // Show two LEDs - a broken line = wireless
  flash_LEDs(5);
  bluetooth = true;
  start_PS2();
}



/**
 * Stops wireless mode ensuring that further key presses are ignored
 */
void shutdown_BLE() {
  stop_PS2();
  clear_all();
  bluetooth = false;
  stop_BLE();
}

/**
 * Starts wired mode ensuring that we don't process key presses until we are ready
 */
void startup_USB(boolean reset) {
  start_USB();
  if (reset) {
    reconfigure_USB();
  }
  // Show three LEDs - a full line = wired
  flash_LEDs(7);
  usb = true;
  start_PS2();
}

/**
 * Stops wired mode ensuring that further key presses are ignored
 */
void shutdown_USB() {
  stop_PS2();
  clear_all();
  usb = false;
  stop_USB();
}

/**
 * Switches mode from bluetooth to USB and vice versa
 */
void switch_mode() {
  if (bluetooth) {
    if (DEBUG) {
      Serial.println(F("Switching to USB keyboard"));
    }
    shutdown_BLE();
    startup_USB(false);
  } else {
    if (DEBUG) {
      Serial.println(F("Switching to Bluetooth keyboard"));
    }
    shutdown_USB();
    startup_BLE(false);
  }
}

/**
 * Performs a hard reset for the current mode - USB or bluetooth
 */
void reconfigure() {
  if (bluetooth) {
    if (DEBUG) {
      Serial.println(F("Reconfiguring Bluetooth keyboard"));
    }
    shutdown_BLE();
    startup_BLE(true);
  } else {
    if (DEBUG) {
      Serial.println(F("Reconfiguring USB keyboard"));
    }
    shutdown_USB();
    startup_USB(true);
  }
}

/**
 * Method to perform the initial keyboard start
 *
 * Currently this defaults to bluetooth
 */
void start_keyboard() {
  if (bluetooth) {
    if (DEBUG) {
      Serial.println(F("Starting as Bluetooth keyboard"));
    }
    startup_BLE(false);
  } else {
    if (DEBUG) {
      Serial.println(F("Starting as USB keyboard"));
    }
    startup_USB(false);
  }
}




