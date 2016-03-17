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
 * Switches mode from bluetooth to USB and vice versa
 */
void switch_mode() {
  if (bluetooth) {
    if (DEBUG) {
      Serial.println(F("Switching to USB keyboard"));
    }
    clear_all();
    bluetooth = false;
    clear_all();
    stop_BLE();
    start_USB();
    usb = true;
  } else {
    if (DEBUG) {
      Serial.println(F("Switching to Bluetooth keyboard"));
    }
    clear_all();
    usb = false;
    clear_all();
    stop_USB();
    start_BLE();
    bluetooth = true;
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
    clear_all();
    bluetooth = false;
    clear_all();
    stop_BLE();
    start_BLE();
    reconfigure_BLE();
    bluetooth = true;
  } else {
    if (DEBUG) {
      Serial.println(F("Reconfiguring USB keyboard"));
    }
    clear_all();
    usb = false;
    clear_all();
    stop_USB();
    start_USB();
    reconfigure_USB();
    usb = true;
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
    start_BLE();
  } else {
    if (DEBUG) {
      Serial.println(F("Starting as USB keyboard"));
    }
    start_USB();
  }
}




