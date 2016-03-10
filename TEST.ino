static char inputBuffer[BUFFER_SIZE+1];

void test_input() {
    if (FAKE) {
    uint8_t count = get_test_input(inputBuffer, 20);
    if (count) {
      Serial.print("Test input: ");
      Serial.println(inputBuffer);

      String result = inputBuffer;

      if (result.equals("PLAY")) {
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_PLAY_PAUSE);
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_PLAY_PAUSE);
      } else 
      if (result.equals("STOP")) {
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_STOP);
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_STOP);
      } else 
      if (result.equals("NEXT")) {
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_NEXT_TRACK);
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_NEXT_TRACK);
      } else 
      if (result.equals("PREV")) {
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_PREV_TRACK);
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_PREV_TRACK);
      } else 
      if (result.equals("VOLUP")) {
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_VOL_UP);
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_VOL_UP);
      } else 
      if (result.equals("VOLDOWN")) {
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_VOL_DWN);
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_VOL_DWN);
      } else 
      if (result.equals("MUTE")) {
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_MUTE);
        add_to_buffer(PS2_EXTENDED);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_MUTE);
      } else 
      if (result.equals("SWITCH")) {
        add_to_buffer(PS2_LEFT_CTRL);
        add_to_buffer(PS2_ESC);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_ESC);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_LEFT_CTRL);
      } else 
      if (result.equals("RESET")) {
        add_to_buffer(PS2_LEFT_CTRL);
        add_to_buffer(PS2_LEFT_SHIFT);
        add_to_buffer(PS2_ESC);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_ESC);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_LEFT_SHIFT);
        add_to_buffer(PS2_RELEASE);
        add_to_buffer(PS2_LEFT_CTRL);        
      } else       
         for (int i = 0; i < count; ++i) {
         uint8_t ps2Key = ASCII_to_PS2_keymap[inputBuffer[i]];   
         uint8_t modifier = ASCII_to_modifier_keymap[inputBuffer[i]];  
         if (modifier) {
            add_to_buffer(modifier);
         }
         if (ps2Key) {
            add_to_buffer(ps2Key);
            add_to_buffer(PS2_RELEASE);
            add_to_buffer(ps2Key);
         }
         if (modifier) {
            add_to_buffer(PS2_RELEASE);
            add_to_buffer(modifier);
         }
      }
    }
  }
}

uint8_t get_test_input(char inputBuffer[], uint8_t maxSize)
{
  // timeout in 100 milliseconds
  TimeoutTimer timeout(100);

  memset(inputBuffer, 0, maxSize);
  while( (!Serial.available()) && !timeout.expired() ) { delay(1); }

  if ( timeout.expired() ) return false;

  delay(2);
  uint8_t count=0;
  do
  {
    count += Serial.readBytes(inputBuffer+count, maxSize);
    delay(2);
  } while( (count < maxSize) && (Serial.available()) );

  return count;
}




