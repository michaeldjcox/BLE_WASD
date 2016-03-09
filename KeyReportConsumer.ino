// Stores the parts of the USB HID key command
KeyReport consumerReport;
uint8_t mediaHidKey;

/**
 * Adds the specified consumer key to the consumer key report
 */
void consumer_add(uint8_t hidKey) {
  mediaHidKey = hidKey;
  consumerReport.modifiers = 0x00;
  consumerReport.reserved = 0x02;
  switch (hidKey) {
    case (HID_PLAY_PAUSE):
      consumerReport.keys[0] = 0x40;
      consumerReport.keys[1] = 0x00;
      break;
    case (HID_STOP):
      consumerReport.keys[0] = 0x00;
      consumerReport.keys[1] = 0x10;
      break;
    case (HID_NEXT_TRACK):
      consumerReport.keys[0] = 0x00;
      consumerReport.keys[1] = 0x02;
      break;
    case (HID_PREV_TRACK):
      consumerReport.keys[0] = 0x00;
      consumerReport.keys[1] = 0x04;
      break;
    case (HID_VOL_UP):
      consumerReport.keys[0] = 0x10;
      consumerReport.keys[1] = 0x00;
      break;
    case (HID_VOL_DWN):
      consumerReport.keys[0] = 0x20;
      consumerReport.keys[1] = 0x00;
      break;
    case (HID_MUTE):
      // Don't know this one...
      consumerReport.keys[0] = 0x00;
      consumerReport.keys[1] = 0x00;
      break;
  }
  consumerReport.keys[2] = 0x00;
  consumerReport.keys[3] = 0x00;
  consumerReport.keys[4] = 0x00;
  consumerReport.keys[5] = 0x00;
}

/**
 * Removes the specified consumer key to the consumer key report
 */
void consumer_remove(uint8_t hidKey) {
  mediaHidKey = 0;
  consumerReport.modifiers = 0x00;
  consumerReport.reserved = 0x02;
  consumerReport.keys[0] = 0x00;
  consumerReport.keys[1] = 0x00;
  consumerReport.keys[2] = 0x00;
  consumerReport.keys[3] = 0x00;
  consumerReport.keys[4] = 0x00;
  consumerReport.keys[5] = 0x00;
}

/**
 * Clears all currently set modifiers - not sure if this is really needed
 */
void clear_consumer_report() {
  consumer_remove(0);
}

/**
 * Gets the key report
 */
KeyReport getConsumerReport() {
  return consumerReport;
}

/**
 * Gets the associated media HID key code
 */
uint8_t getMediaHidKey() {
  return mediaHidKey;
}



