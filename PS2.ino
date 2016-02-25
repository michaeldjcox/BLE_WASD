
void setup_PS2() {
  delay(1000);
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DATA_PIN, INPUT_PULLUP);

  //clear buffer
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = 0;
  }

  attachInterrupt(1, ps2interrupt, FALLING);
}

// The ISR for the external interrupt
void ps2interrupt(void)
{
  static uint8_t bitcount = 0;
  static uint8_t incoming = 0;
  static uint32_t prev_ms = 0;
  uint32_t now_ms;
  uint8_t n, val;

  val = digitalRead(DATA_PIN);
  now_ms = millis();
  if (now_ms - prev_ms > 250) {
    bitcount = 0;
    incoming = 0;
  }
  prev_ms = now_ms;
  n = bitcount - 1;
  if (n <= 7) {
    incoming |= (val << n);
  }
  bitcount++;
  if (bitcount == 11) {

    buffer[write_index] = incoming;
    write_index = (write_index + 1) % BUFFER_SIZE;
    if(DEBUG){
      Serial.print("Raw PS2: ");
      Serial.println(incoming,HEX);
    }
    bitcount = 0;
    incoming = 0;
  }
}



//read the raw PS2 data. This is ugly but so are all the other methods online. Is there even a clean way to read PS2????
void process_buffer() {
  uint8_t new_index;
  if (read_buffer(0) != 0) {
    //regular key press
    if (read_buffer(0) != PS2_RELEASE && read_buffer(0) != PS2_EXTENDED && read_buffer(0) != PS2_PAUSE_SEQUENCE) {
      if (is_modifier(read_buffer(0), 0)) {
        set_modifier(read_buffer(0), 0);
        send_key(read_buffer(0), 0, 1);
      }
      else {
        send_key(read_buffer(0), 0, 0);
      }
      new_index = read_index + 1;
    }

    //regular key release, check for modifier
    else if (read_buffer(0) == PS2_RELEASE) {
      while (!read_buffer(1)); //wait for next byte
      if (is_modifier(read_buffer(1), 0)) {
        unset_modifier(read_buffer(1), 0);
      }
      release_key();
      new_index = read_index + 2;
    }

    //long key
    else if (read_buffer(0) == PS2_EXTENDED) {
      while (!read_buffer(1)); //wait for next byte
      //long key press
      if (read_buffer(1) != PS2_RELEASE) {
        if (is_modifier(read_buffer(1), 1)) {
          set_modifier(read_buffer(1), 1);
          send_key(read_buffer(1), 1, 1);
        }
        else if (is_media(read_buffer(1))) {
          send_media(read_buffer(1));
        }
        else {
          send_key(read_buffer(1), 1, 0);
        }


        if (read_buffer(1) == PS2_PRNT_SCR) { //obnoxiously long print screen key
          while (!read_buffer(3)); //wait for the remaining 2 bytes before clearing buffer
          new_index = read_index + 4;
        }
        else {
          new_index = read_index + 2;
        }
      }

      //long key release
      else {
        while (!read_buffer(2)); //wait for next byte
        if (is_modifier(read_buffer(2), 1)) {
          unset_modifier(read_buffer(2), 1);
        }

        release_key();

        if (read_buffer(1) == PS2_PRNT_SCR) { //obnoxiously long print screen key
          while (!read_buffer(5)); //wait for the remaining 3 bytes before clearing buffer
          new_index = read_index + 5;
        }
        else {

          new_index = read_index + 3;
        }
      }
    }

    //extraaaaaa long, pain in the ass, pause key
    else if (read_buffer(0) == PS2_PAUSE_SEQUENCE) {
      send_key(PS2_PAUSE_KEY_FAKE, 1, 0);
      while (!read_buffer(7)); //wait for the remaining 7 bytes before clearing buffer, WTF
      new_index = read_index + 8;
    }

    //clear read bytes
    for (int i = 0; i < new_index - read_index; i++) {
      uint8_t x = (read_index + i) % BUFFER_SIZE;
      buffer[x] = 0;
    }

    read_index = new_index % BUFFER_SIZE;
  }
}



uint8_t read_buffer(uint8_t i) {
  return buffer[(read_index + i) % BUFFER_SIZE];
}

void setup_keymaps(){
        PS2_to_HID_keymap[PS2_A]                    = HID_A;
        PS2_to_HID_keymap[PS2_B]                    = HID_B;
        PS2_to_HID_keymap[PS2_C]                    = HID_C;
        PS2_to_HID_keymap[PS2_D]                    = HID_D;
        PS2_to_HID_keymap[PS2_E]                    = HID_E;
        PS2_to_HID_keymap[PS2_F]                    = HID_F;
        PS2_to_HID_keymap[PS2_G]                    = HID_G;
        PS2_to_HID_keymap[PS2_H]                    = HID_H;
        PS2_to_HID_keymap[PS2_I]                    = HID_I;
        PS2_to_HID_keymap[PS2_J]                    = HID_J;
        PS2_to_HID_keymap[PS2_K]                    = HID_K;
        PS2_to_HID_keymap[PS2_L]                    = HID_L;
        PS2_to_HID_keymap[PS2_M]                    = HID_M;
        PS2_to_HID_keymap[PS2_N]                    = HID_N;
        PS2_to_HID_keymap[PS2_O]                    = HID_O;
        PS2_to_HID_keymap[PS2_P]                    = HID_P;
        PS2_to_HID_keymap[PS2_Q]                    = HID_Q;
        PS2_to_HID_keymap[PS2_R]                    = HID_R;
        PS2_to_HID_keymap[PS2_S]                    = HID_S;
        PS2_to_HID_keymap[PS2_T]                    = HID_T;
        PS2_to_HID_keymap[PS2_U]                    = HID_U;
        PS2_to_HID_keymap[PS2_V]                    = HID_V;
        PS2_to_HID_keymap[PS2_W]                    = HID_W;
        PS2_to_HID_keymap[PS2_X]                    = HID_X;
        PS2_to_HID_keymap[PS2_Y]                    = HID_Y;
        PS2_to_HID_keymap[PS2_Z]                    = HID_Z;
        PS2_to_HID_keymap[PS2_0]                    = HID_0;
        PS2_to_HID_keymap[PS2_1]                    = HID_1;
        PS2_to_HID_keymap[PS2_2]                    = HID_2;
        PS2_to_HID_keymap[PS2_3]                    = HID_3;
        PS2_to_HID_keymap[PS2_4]                    = HID_4;
        PS2_to_HID_keymap[PS2_5]                    = HID_5;
        PS2_to_HID_keymap[PS2_6]                    = HID_6;
        PS2_to_HID_keymap[PS2_7]                    = HID_7;
        PS2_to_HID_keymap[PS2_8]                    = HID_8;
        PS2_to_HID_keymap[PS2_9]                    = HID_9;
        PS2_to_HID_keymap[PS2_ACCENT]               = HID_ACCENT;
        PS2_to_HID_keymap[PS2_HYPH]                 = HID_HYPH;
        PS2_to_HID_keymap[PS2_EQUAL]                = HID_EQUAL;
        PS2_to_HID_keymap[PS2_BACK_SLASH]           = HID_BACK_SLASH;
        PS2_to_HID_keymap[PS2_BACK_SPACE]           = HID_BACK_SPACE;
        PS2_to_HID_keymap[PS2_SPACE]                = HID_SPACE;
        PS2_to_HID_keymap[PS2_TAB]                  = HID_TAB;
        PS2_to_HID_keymap[PS2_CAPS]                 = HID_CAPS; 
        PS2_to_HID_keymap[PS2_LEFT_SHIFT]           = HID_LEFT_SHIFT;
        PS2_to_HID_keymap[PS2_LEFT_CTRL]            = HID_LEFT_CTRL;
        PS2_to_HID_keymap[PS2_LEFT_ALT]             = HID_LEFT_ALT;
        PS2_to_HID_keymap[PS2_RIGHT_SHIFT]          = HID_RIGHT_SHIFT;
        PS2_to_HID_keymap[PS2_ENTER]                = HID_ENTER;
        PS2_to_HID_keymap[PS2_ESC]                  = HID_ESC;
        PS2_to_HID_keymap[PS2_F1]                   = HID_F1;
        PS2_to_HID_keymap[PS2_F2]                   = HID_F2;
        PS2_to_HID_keymap[PS2_F3]                   = HID_F3;
        PS2_to_HID_keymap[PS2_F4]                   = HID_F4;
        PS2_to_HID_keymap[PS2_F5]                   = HID_F5;
        PS2_to_HID_keymap[PS2_F6]                   = HID_F6;
        PS2_to_HID_keymap[PS2_F7]                   = HID_F7;
        PS2_to_HID_keymap[PS2_F8]                   = HID_F8;
        PS2_to_HID_keymap[PS2_F9]                   = HID_F9;
        PS2_to_HID_keymap[PS2_F10]                  = HID_F10;
        PS2_to_HID_keymap[PS2_F11]                  = HID_F11;
        PS2_to_HID_keymap[PS2_F12]                  = HID_F12;
        PS2_to_HID_keymap[PS2_SCROLL]               = HID_SCR_LCK;
        PS2_to_HID_keymap[PS2_LEFT_BRACKET]         = HID_LEFT_BRACKET;
        PS2_to_HID_keymap[PS2_NUM]                  = HID_NUM_LCK;
        PS2_to_HID_keymap[PS2_KP_STAR]              = HID_KP_STAR;
        PS2_to_HID_keymap[PS2_KP_HYPH]              = HID_KP_HYPH;
        PS2_to_HID_keymap[PS2_KP_PLUS]              = HID_KP_PLUS;
        PS2_to_HID_keymap[PS2_KP_PERIOD]            = HID_KP_PERIOD;
        PS2_to_HID_keymap[PS2_KP_0]                 = HID_KP_0;
        PS2_to_HID_keymap[PS2_KP_1]                 = HID_KP_1;
        PS2_to_HID_keymap[PS2_KP_2]                 = HID_KP_2;
        PS2_to_HID_keymap[PS2_KP_3]                 = HID_KP_3;
        PS2_to_HID_keymap[PS2_KP_4]                 = HID_KP_4;
        PS2_to_HID_keymap[PS2_KP_5]                 = HID_KP_5;
        PS2_to_HID_keymap[PS2_KP_6]                 = HID_KP_6;
        PS2_to_HID_keymap[PS2_KP_7]                 = HID_KP_7;
        PS2_to_HID_keymap[PS2_KP_8]                 = HID_KP_8;
        PS2_to_HID_keymap[PS2_KP_9]                 = HID_KP_9;
        PS2_to_HID_keymap[PS2_RIGHT_BRACKET]        = HID_RIGHT_BRACKET;
        PS2_to_HID_keymap[PS2_SEMI_COLON]           = HID_SEMI_COLON;
        PS2_to_HID_keymap[PS2_APOSTROPHY]           = HID_APOSTROPHY;
        PS2_to_HID_keymap[PS2_COMMA]                = HID_COMMA;
        PS2_to_HID_keymap[PS2_PERIOD]               = HID_PERIOD;
        PS2_to_HID_keymap[PS2_FWD_SLASH]            = HID_FWD_SLASH;
        PS2_to_HID_keymap[PS2_BACK_SLASH_NON_US]    = HID_BACK_SLASH_NON_US;

        PS2Long_to_HID_keymap[PS2_PRNT_SCR]         = HID_PRNT_SCR;
        PS2Long_to_HID_keymap[PS2_RIGHT_ALT]        = HID_RIGHT_ALT;
        PS2Long_to_HID_keymap[PS2_RIGHT_CTRL]       = HID_RIGHT_CTRL;
        PS2Long_to_HID_keymap[PS2_PREV_TRACK]       = HID_PREV_TRACK;
        PS2Long_to_HID_keymap[PS2_LEFT_GUI]         = HID_LEFT_GUI;
        PS2Long_to_HID_keymap[PS2_VOL_DWN]          = HID_VOL_DWN;
        PS2Long_to_HID_keymap[PS2_MUTE]             = HID_MUTE;
        PS2Long_to_HID_keymap[PS2_RIGHT_GUI]        = HID_RIGHT_GUI;
        PS2Long_to_HID_keymap[PS2_APPLICATION]      = HID_APPLICATION;
        PS2Long_to_HID_keymap[PS2_VOL_UP]           = HID_VOL_UP;
        PS2Long_to_HID_keymap[PS2_PLAY_PAUSE]       = HID_PLAY_PAUSE;
        PS2Long_to_HID_keymap[PS2_STOP]             = HID_STOP;
        PS2Long_to_HID_keymap[PS2_KP_FWD_SLASH]     = HID_KP_FWD_SLASH;
        PS2Long_to_HID_keymap[PS2_NEXT_TRACK]       = HID_NEXT_TRACK;
        PS2Long_to_HID_keymap[PS2_KP_ENTER]         = HID_KP_ENTER;
        PS2Long_to_HID_keymap[PS2_END]              = HID_END;
        PS2Long_to_HID_keymap[PS2_LEFT_ARROW]       = HID_LEFT_ARROW;
        PS2Long_to_HID_keymap[PS2_HOME]             = HID_HOME;
        PS2Long_to_HID_keymap[PS2_PAUSE_KEY_FAKE]   = HID_PAUSE;
        PS2Long_to_HID_keymap[PS2_INSERT]           = HID_INSERT;
        PS2Long_to_HID_keymap[PS2_DELETE]           = HID_DEL;
        PS2Long_to_HID_keymap[PS2_DOWN_ARROW]       = HID_DOWN_ARROW;
        PS2Long_to_HID_keymap[PS2_RIGHT_ARROW]      = HID_RIGHT_ARROW;
        PS2Long_to_HID_keymap[PS2_UP_ARROW]         = HID_UP_ARROW;
        PS2Long_to_HID_keymap[PS2_PG_DWN]           = HID_PG_DWN;
        PS2Long_to_HID_keymap[PS2_PG_UP]            = HID_PG_UP;
    }


