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
 * Stops USB
 */
void reconfigure_USB() {
  // Nothing to do here at the moment
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
#if defined (DEBUG) 
        Serial.println(F("Remote->PlayPause"));
#endif
      Remote.playpause();
      Remote.clear();
      break;
    case (HID_STOP):
#if defined (DEBUG) 
        Serial.println(F("Remote->Stop"));
#endif
      Remote.stop();
      Remote.clear();
      break;
    case (HID_NEXT_TRACK):
#if defined (DEBUG) 
        Serial.println(F("Remote->Next"));
#endif
      Remote.next();
      Remote.clear();
      break;
    case (HID_PREV_TRACK):
#if defined (DEBUG) 
        Serial.println(F("Remote->Prev"));
#endif
      Remote.previous();
      Remote.clear();
      break;
    case (HID_VOL_UP):
#if defined (DEBUG) 
        Serial.println(F("Remote->VolUp"));
#endif
      Remote.increase();
      Remote.clear();
      break;
    case (HID_VOL_DWN):
#if defined (DEBUG) 
        Serial.println(F("Remote->VolDwn"));
#endif
      Remote.decrease();
      Remote.clear();
      break;
    case (HID_MUTE):
#if defined (DEBUG) 
        Serial.println(F("Remote->Mute"));
#endif
      Remote.mute();
      Remote.clear();
      break;
    default:
      return;
  }
}



