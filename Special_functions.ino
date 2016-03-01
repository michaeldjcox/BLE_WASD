/**
 * special_functions - intercept keyboard commands before they get sent
 * 
 * Used to: 
 * 
 * 1) reconfigure the keyboard and clear any stuck modifiers
 * 2) Reset BLE on Ctrl-Shift-Esc
 * 3) Send media key commands
 * 
 * Could be modified to do whatever you would like...
 * 
 * Possibly convert the keypad into a T9 keyboard
 * 
 * Return a 1 to send the key and a 0 to block it from being sent
 */

bool special_functions(uint8_t hid_key, KeyReport keyReport, bool brk){

  //reset modifiers
  if (hid_key == HID_ENTER && brk) { //reset just to clean up every now and then
    if (!brk) {
        keyReport.modifiers = 0;
    }
  }

  //reconfigure keyboard
  if(hid_key == HID_ESC && keyReport.modifiers == 0x03){
    if (!brk) {
        start_BLE(1); //reset Keyboard
    }
    return 0;
  }

  //reconfigure keyboard
  if(hid_key == HID_ESC && keyReport.modifiers == 0x01){
    if (!brk) {
      if (onBle()) {
        stop_BLE();
        start_USB();
      } else {
        stop_USB();
        start_BLE(0); //reset Keyboard
      }
    }
    return 0;
  }

  if (is_media(hid_key)) {
    if (bluetooth) {
      if (!brk) {
        send_media(hid_key);
      }
      return 0;
    }
  }

  //remap keypad to work as T9 keyboard...
  
  return 1;
  
}

