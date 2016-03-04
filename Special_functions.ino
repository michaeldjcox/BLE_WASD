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
 
bool special_functions(uint8_t hid_key, bool brk) {

  //reset modifiers - just to clean up every now and then
  if (hid_key == HID_ENTER && brk) {  
    if (!brk) {
        clear_modifiers();
    }
  }

  //reconfigure keyboard
  if(hid_key == HID_ESC && isControlShift()){
    if (!brk) {
        reset();        
    }
    return 0;
  }

  //switch bluetooth to USB or back
  if(hid_key == HID_ESC && isControl()){
    if (!brk) {
      switchMode();
    }
    return 0;
  }

  if (is_media(hid_key)) {    
    if (!brk) {
      consumer_add(hid_key);
    } else {
      consumer_remove(hid_key);
    }
    send_consumer_report();
    return 0;
  }

  //remap keypad to work as T9 keyboard...
  
  return 1;
  
}

/**
 * Tests if a HID key code corresponds to a media key
 */
uint8_t is_media(uint8_t c) {
  
  switch (c) {
    case (HID_PLAY_PAUSE):
    case (HID_STOP):
    case (HID_NEXT_TRACK):
    case (HID_PREV_TRACK):
    case (HID_VOL_UP):
    case (HID_VOL_DWN):
    case (HID_MUTE):
      return 1;
      break;
  }
  return 0;
}

