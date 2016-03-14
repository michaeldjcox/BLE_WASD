/**
 * Functions concerned with wired mode of the keyboard
 */

// Traces state of play/pause - this could get out of sync
volatile boolean playing = false;

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
      if (playing) {
        if (DEBUG) {
          Serial.println(F("Remote->Pause"));
        }
        Remote.pause();
        playing = false;
      } else {
        if (DEBUG) {
          Serial.println(F("Remote->Play"));
        }
        Remote.play();
        playing = true;
      }

      break;
    case (HID_STOP):
      if (DEBUG) {
        Serial.println(F("Remote->Stop"));
      }
      Remote.stop();
      break;
    case (HID_NEXT_TRACK):
      if (DEBUG) {
        Serial.println(F("Remote->Next"));
      }
      Remote.next();
      break;
    case (HID_PREV_TRACK):
      if (DEBUG) {
        Serial.println(F("Remote->Prev"));
      }
      Remote.previous();
      break;
    case (HID_VOL_UP):
      if (DEBUG) {
        Serial.println(F("Remote->VolUp"));
      }
      Remote.increase();
      break;
    case (HID_VOL_DWN):
      if (DEBUG) {
        Serial.println(F("Remote->VolDwn"));
      }
      Remote.decrease();
      break;
    case (HID_MUTE):
      if (DEBUG) {
        Serial.println(F("Remote->Mute"));
      }
      Remote.mute();
      break;
    default:
      return;
  }
}



