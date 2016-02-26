
/**
 * special_functions - intercept keyboard commands before they get sent
 * Used to reconfigure the keyboard and clear any stuck modifiers
 * Can be used to do whatever youd like...
 * Possibly convert the keypad into a T9 keyboard
 * Return a 1 to send the key and a 0 to block it from being sent
 **/

bool special_functions(uint8_t hid_key, bool is_long_key, uint8_t modifiers, bool brk){

  //reset modifiers
  if (hid_key == HID_ENTER && brk) { //reset just to clean up every now and then
    if (!brk) {
        modifiers = 0;
    }
  }

  //reconfigure keyboard
  if(hid_key == HID_ESC && modifiers == 0x03){
    if (!brk) {
        start_BLE(1); //reset Keyboard
    }
    return 0;
  }

          if (hid_key == HID_PLAY_PAUSE || hid_key == HID_STOP || hid_key == HID_NEXT_TRACK || hid_key == HID_PREV_TRACK ||
                hid_key == HID_VOL_UP || hid_key == HID_VOL_DWN || hid_key == HID_MUTE) {
            if (!brk) {
                String str="NOTHING";
                if (hid_key == HID_PLAY_PAUSE) {
                    str = "PLAYPAUSE";

                } else if (hid_key == HID_STOP) {
                    str = "MEDIASTOP";

                } else if (hid_key == HID_NEXT_TRACK) {
                    str = "MEDIANEXT";

                } else if (hid_key == HID_PREV_TRACK) {
                    str = "MEDIAPREVIOUS";

                } else if (hid_key == HID_VOL_UP) {
                    str = "VOLUME+,100";

                } else if (hid_key == HID_VOL_DWN) {
                    str = "VOLUME-,100";

                } else if (hid_key == HID_MUTE) {
                    str = "MUTE";
                }

String cmd="AT+BLEHIDCONTROLKEY=" + str;
                
                ble.println();

                    if(DEBUG){
      Serial.println(cmd);
    }
            }
            return 0;
        }

  //remap keypad to work as T9 keyboard...
  
  return 1;
  
}

