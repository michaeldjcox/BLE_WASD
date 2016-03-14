/**
 * Functions concerned with wired mode of the keyboard
 */

 /**
  * Starts USB 
  */
void start_USB() {
    Keyboard.begin();
    Remote.begin();
}

/**
 * Stops USB 
 */
void stop_USB() {
    Keyboard.end();
    Remote.end();  
}

/**
 * Sends the key report over USB
 */
void send_usb_report(KeyReport report) {
    log_key_report(keyReport);
    Keyboard.sendReport(&keyReport);
}

/**
 * Sends a media control key (rather than the HID key code)
 */
void send_usb_media(uint8_t hidKey) {
  switch (hidKey) {
    case (HID_PLAY_PAUSE):
      if (DEBUG) {
        Serial.println(F("Remote->PlayPause"));
      }
      Remote.playpause();
      Remote.clear();
      break;
    case (HID_STOP):
      if (DEBUG) {
        Serial.println(F("Remote->Stop"));
      }
      Remote.stop();
      Remote.clear();
      break;
    case (HID_NEXT_TRACK):
      if (DEBUG) {
        Serial.println(F("Remote->Next"));
      }
      Remote.next();
      Remote.clear();
      break;
    case (HID_PREV_TRACK):
      if (DEBUG) {
        Serial.println(F("Remote->Prev"));
      }
      Remote.previous();
      Remote.clear();
      break;
    case (HID_VOL_UP):
      if (DEBUG) {
        Serial.println(F("Remote->VolUp"));
      }
      Remote.increase();
      Remote.clear();
      break;
    case (HID_VOL_DWN):
      if (DEBUG) {
        Serial.println(F("Remote->VolDwn"));
      }
      Remote.decrease();
      Remote.clear();
      break;
    case (HID_MUTE):
      if (DEBUG) {
        Serial.println(F("Remote->Mute"));
      }
      Remote.mute();
      Remote.clear();
      break;
    default:
      return;
  }
}



