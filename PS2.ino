/**
 * Functions associated with PS2 communication
 */
//buffer to store key strokes
static volatile uint8_t buffer[BUFFER_SIZE];

//keep track of our place in the buffer
static volatile uint8_t head, tail;

//keep track of PS2 bits sent and received
static volatile uint8_t sendBits, msg, bitCount, setBits;

//the LED setting
uint8_t leds;

// Should we send the LED state to the keyboard and stop receiving for a while
bool sendLeds;

// Has the extended PS2 mode been entered
bool ext;

// Are we in key release
bool brk;

// The number of PS2 key codes we should now ignore e.g. after Pause key is pressed
int skip;

/**
 * Set up the PS2 interface
 */
void setup_PS2() {
  delay(1000);
  pinMode(CLK_PIN, INPUT_PULLUP);
  pinMode(DATA_PIN, INPUT_PULLUP);

  //clear buffer
  for (int i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = 0;
  }

  attachInterrupt(1, ps2interrupt, FALLING);

  head = 0;
  tail = 0;
  sendBits = 0;
}

/**
 * The ISR for the external interrupt
 */
void ps2interrupt(void) {
  static uint8_t bitcount = 0;
  static uint8_t incoming = 0;
  static uint32_t prev_ms = 0;
  uint32_t now_ms;
  uint8_t n, val;

  if (!sendBits) {
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
      add_to_buffer(incoming);
      bitcount = 0;
      incoming = 0;
    }
  } else {
    --sendBits;
    uint8_t b = bitCount - 1;
    if (b == 8) {
      digitalWrite(DATA_PIN, !(setBits & 1));
    } else if (b == 9) {
      pinMode(DATA_PIN, INPUT_PULLUP);
    } else if (b < 8) {
      bool bt = (msg >> b) & 1;
      digitalWrite(DATA_PIN, bt);
      setBits += bt;
    }
    ++bitCount;
  }
}

void add_to_buffer(uint8_t input) {
  uint8_t i = head + 1;
  if (i >= BUFFER_SIZE) {
    i = 0;
  }
  if (i != tail) {
    buffer[i] = input;
    head = i;
  }
  if (DEBUG) {
    Serial.print(F("Raw PS2: "));
    Serial.print(input, HEX);
    Serial.print(" ");
    Serial.println(head);
  }
}

/**
 * Reads the next PS2 key code from the buffer
 */
static inline uint8_t get_scan_code(void) {
  uint8_t c, i;

  i = tail;
  if (i == head)
    return 0;
  i++;
  if (i >= BUFFER_SIZE)
    i = 0;
  c = buffer[i];
  tail = i;
  return c;
}

/**
 * Gets the next PS2 key press from the buffer and decides what to do in response
 */
void process_buffer() {
  uint8_t ps2Key = get_scan_code();
  uint8_t hidKey;
  if (ps2Key) {
    if (skip) {
      --skip;
    } else {
      if (ps2Key == PS2_EXTENDED) {
        ext = true;
      } else if (ps2Key == PS2_RELEASE) {
        brk = true;
      } else if (ps2Key == PS2_LED_ACK) {
        if (sendLeds) {
          sendLeds = false;
          send_ps2_msg(leds);
        }
      } else {
        if (ps2Key == PS2_PAUSE_SEQUENCE) {
          hidKey = HID_PAUSE;
          skip = 7;
          brk = true;
          report_add(hidKey);
          send_key_report();
        } else {
          hidKey =
            ext ? PS2Long_to_HID_keymap[ps2Key] : PS2_to_HID_keymap[ps2Key];
        }

        if (hidKey) {
          boolean send_key = special_functions(hidKey, brk);
          if (send_key) {
            if (is_media(hidKey)) {
              if (!brk) {
                consumer_add(hidKey);
              } else {
                consumer_remove(hidKey);
              }
              send_consumer_report();
            } else {
              if (brk) {
                report_remove(hidKey);
                if (hidKey == HID_NUM_LCK
                    || hidKey == HID_SCR_LCK
                    || hidKey == HID_CAPS) {
                  sendLeds = true;
                  if (hidKey == HID_NUM_LCK) {
                    leds ^= 2;
                  } else if (hidKey == HID_SCR_LCK) {
                    leds ^= 1;
                  } else if (hidKey == HID_CAPS) {
                    leds ^= 4;
                  }
                  send_ps2_msg((byte) PS2_SET_RESET_LEDS);
                }
              } else {
                report_add(hidKey);
              }
              send_key_report();
            }
          }
        }

        brk = false;
        ext = false;
      }
    }
  }
}

/**
 * Sends a message up to the keyboard in order to set LEDs
 */
void send_ps2_msg(uint8_t ps2Msg) {
  noInterrupts();
  pinMode(CLK_PIN, OUTPUT);
  digitalWrite(CLK_PIN, LOW);
  delayMicroseconds(60);
  pinMode(CLK_PIN, INPUT_PULLUP);
  msg = ps2Msg;
  bitCount = 0;
  sendBits = 12;
  setBits = 0;
  pinMode(DATA_PIN, OUTPUT);
  digitalWrite(DATA_PIN, LOW);
  interrupts();
}

/**
 * Tests if a HID key code corresponds to a media key
 */
bool is_media(uint8_t hidKey) {

  switch (hidKey) {
    case (HID_PLAY_PAUSE):
    case (HID_STOP):
    case (HID_NEXT_TRACK):
    case (HID_PREV_TRACK):
    case (HID_VOL_UP):
    case (HID_VOL_DWN):
    case (HID_MUTE):
      return true;
      break;
  }
  return false;
}

/**
 * Sets up the keyboard mapping from PS2 to HID
 */
void setup_keymaps() {
  PS2_to_HID_keymap[PS2_A] = HID_A;
  PS2_to_HID_keymap[PS2_B] = HID_B;
  PS2_to_HID_keymap[PS2_C] = HID_C;
  PS2_to_HID_keymap[PS2_D] = HID_D;
  PS2_to_HID_keymap[PS2_E] = HID_E;
  PS2_to_HID_keymap[PS2_F] = HID_F;
  PS2_to_HID_keymap[PS2_G] = HID_G;
  PS2_to_HID_keymap[PS2_H] = HID_H;
  PS2_to_HID_keymap[PS2_I] = HID_I;
  PS2_to_HID_keymap[PS2_J] = HID_J;
  PS2_to_HID_keymap[PS2_K] = HID_K;
  PS2_to_HID_keymap[PS2_L] = HID_L;
  PS2_to_HID_keymap[PS2_M] = HID_M;
  PS2_to_HID_keymap[PS2_N] = HID_N;
  PS2_to_HID_keymap[PS2_O] = HID_O;
  PS2_to_HID_keymap[PS2_P] = HID_P;
  PS2_to_HID_keymap[PS2_Q] = HID_Q;
  PS2_to_HID_keymap[PS2_R] = HID_R;
  PS2_to_HID_keymap[PS2_S] = HID_S;
  PS2_to_HID_keymap[PS2_T] = HID_T;
  PS2_to_HID_keymap[PS2_U] = HID_U;
  PS2_to_HID_keymap[PS2_V] = HID_V;
  PS2_to_HID_keymap[PS2_W] = HID_W;
  PS2_to_HID_keymap[PS2_X] = HID_X;
  PS2_to_HID_keymap[PS2_Y] = HID_Y;
  PS2_to_HID_keymap[PS2_Z] = HID_Z;
  PS2_to_HID_keymap[PS2_0] = HID_0;
  PS2_to_HID_keymap[PS2_1] = HID_1;
  PS2_to_HID_keymap[PS2_2] = HID_2;
  PS2_to_HID_keymap[PS2_3] = HID_3;
  PS2_to_HID_keymap[PS2_4] = HID_4;
  PS2_to_HID_keymap[PS2_5] = HID_5;
  PS2_to_HID_keymap[PS2_6] = HID_6;
  PS2_to_HID_keymap[PS2_7] = HID_7;
  PS2_to_HID_keymap[PS2_8] = HID_8;
  PS2_to_HID_keymap[PS2_9] = HID_9;
  PS2_to_HID_keymap[PS2_ACCENT] = HID_ACCENT;
  PS2_to_HID_keymap[PS2_HYPH] = HID_HYPH;
  PS2_to_HID_keymap[PS2_EQUAL] = HID_EQUAL;
  PS2_to_HID_keymap[PS2_BACK_SLASH] = HID_BACK_SLASH;
  PS2_to_HID_keymap[PS2_BACK_SPACE] = HID_BACK_SPACE;
  PS2_to_HID_keymap[PS2_SPACE] = HID_SPACE;
  PS2_to_HID_keymap[PS2_TAB] = HID_TAB;
  PS2_to_HID_keymap[PS2_CAPS] = HID_CAPS;
  PS2_to_HID_keymap[PS2_LEFT_SHIFT] = HID_LEFT_SHIFT;
  PS2_to_HID_keymap[PS2_LEFT_CTRL] = HID_LEFT_CTRL;
  PS2_to_HID_keymap[PS2_LEFT_ALT] = HID_LEFT_ALT;
  PS2_to_HID_keymap[PS2_RIGHT_SHIFT] = HID_RIGHT_SHIFT;
  PS2_to_HID_keymap[PS2_ENTER] = HID_ENTER;
  PS2_to_HID_keymap[PS2_ESC] = HID_ESC;
  PS2_to_HID_keymap[PS2_F1] = HID_F1;
  PS2_to_HID_keymap[PS2_F2] = HID_F2;
  PS2_to_HID_keymap[PS2_F3] = HID_F3;
  PS2_to_HID_keymap[PS2_F4] = HID_F4;
  PS2_to_HID_keymap[PS2_F5] = HID_F5;
  PS2_to_HID_keymap[PS2_F6] = HID_F6;
  PS2_to_HID_keymap[PS2_F7] = HID_F7;
  PS2_to_HID_keymap[PS2_F8] = HID_F8;
  PS2_to_HID_keymap[PS2_F9] = HID_F9;
  PS2_to_HID_keymap[PS2_F10] = HID_F10;
  PS2_to_HID_keymap[PS2_F11] = HID_F11;
  PS2_to_HID_keymap[PS2_F12] = HID_F12;
  PS2_to_HID_keymap[PS2_SCROLL] = HID_SCR_LCK;
  PS2_to_HID_keymap[PS2_LEFT_BRACKET] = HID_LEFT_BRACKET;
  PS2_to_HID_keymap[PS2_NUM] = HID_NUM_LCK;
  PS2_to_HID_keymap[PS2_KP_STAR] = HID_KP_STAR;
  PS2_to_HID_keymap[PS2_KP_HYPH] = HID_KP_HYPH;
  PS2_to_HID_keymap[PS2_KP_PLUS] = HID_KP_PLUS;
  PS2_to_HID_keymap[PS2_KP_PERIOD] = HID_KP_PERIOD;
  PS2_to_HID_keymap[PS2_KP_0] = HID_KP_0;
  PS2_to_HID_keymap[PS2_KP_1] = HID_KP_1;
  PS2_to_HID_keymap[PS2_KP_2] = HID_KP_2;
  PS2_to_HID_keymap[PS2_KP_3] = HID_KP_3;
  PS2_to_HID_keymap[PS2_KP_4] = HID_KP_4;
  PS2_to_HID_keymap[PS2_KP_5] = HID_KP_5;
  PS2_to_HID_keymap[PS2_KP_6] = HID_KP_6;
  PS2_to_HID_keymap[PS2_KP_7] = HID_KP_7;
  PS2_to_HID_keymap[PS2_KP_8] = HID_KP_8;
  PS2_to_HID_keymap[PS2_KP_9] = HID_KP_9;
  PS2_to_HID_keymap[PS2_RIGHT_BRACKET] = HID_RIGHT_BRACKET;
  PS2_to_HID_keymap[PS2_SEMI_COLON] = HID_SEMI_COLON;
  PS2_to_HID_keymap[PS2_APOSTROPHY] = HID_APOSTROPHY;
  PS2_to_HID_keymap[PS2_COMMA] = HID_COMMA;
  PS2_to_HID_keymap[PS2_PERIOD] = HID_PERIOD;
  PS2_to_HID_keymap[PS2_FWD_SLASH] = HID_FWD_SLASH;
  PS2_to_HID_keymap[PS2_BACK_SLASH_NON_US] = HID_BACK_SLASH_NON_US;

  PS2Long_to_HID_keymap[PS2_PRNT_SCR] = HID_PRNT_SCR;
  PS2Long_to_HID_keymap[PS2_RIGHT_ALT] = HID_RIGHT_ALT;
  PS2Long_to_HID_keymap[PS2_RIGHT_CTRL] = HID_RIGHT_CTRL;
  PS2Long_to_HID_keymap[PS2_PREV_TRACK] = HID_PREV_TRACK;
  PS2Long_to_HID_keymap[PS2_LEFT_GUI] = HID_LEFT_GUI;
  PS2Long_to_HID_keymap[PS2_VOL_DWN] = HID_VOL_DWN;
  PS2Long_to_HID_keymap[PS2_MUTE] = HID_MUTE;
  PS2Long_to_HID_keymap[PS2_RIGHT_GUI] = HID_RIGHT_GUI;
  PS2Long_to_HID_keymap[PS2_APPLICATION] = HID_APPLICATION;
  PS2Long_to_HID_keymap[PS2_VOL_UP] = HID_VOL_UP;
  PS2Long_to_HID_keymap[PS2_PLAY_PAUSE] = HID_PLAY_PAUSE;
  PS2Long_to_HID_keymap[PS2_STOP] = HID_STOP;
  PS2Long_to_HID_keymap[PS2_KP_FWD_SLASH] = HID_KP_FWD_SLASH;
  PS2Long_to_HID_keymap[PS2_NEXT_TRACK] = HID_NEXT_TRACK;
  PS2Long_to_HID_keymap[PS2_KP_ENTER] = HID_KP_ENTER;
  PS2Long_to_HID_keymap[PS2_END] = HID_END;
  PS2Long_to_HID_keymap[PS2_LEFT_ARROW] = HID_LEFT_ARROW;
  PS2Long_to_HID_keymap[PS2_HOME] = HID_HOME;
  PS2Long_to_HID_keymap[PS2_PAUSE_KEY_FAKE] = HID_PAUSE;
  PS2Long_to_HID_keymap[PS2_INSERT] = HID_INSERT;
  PS2Long_to_HID_keymap[PS2_DELETE] = HID_DEL;
  PS2Long_to_HID_keymap[PS2_DOWN_ARROW] = HID_DOWN_ARROW;
  PS2Long_to_HID_keymap[PS2_RIGHT_ARROW] = HID_RIGHT_ARROW;
  PS2Long_to_HID_keymap[PS2_UP_ARROW] = HID_UP_ARROW;
  PS2Long_to_HID_keymap[PS2_PG_DWN] = HID_PG_DWN;
  PS2Long_to_HID_keymap[PS2_PG_UP] = HID_PG_UP;


  ASCII_to_PS2_keymap['a'] = PS2_A;
  ASCII_to_PS2_keymap['b'] = PS2_B;
  ASCII_to_PS2_keymap['c'] = PS2_C;
  ASCII_to_PS2_keymap['d'] = PS2_D;
  ASCII_to_PS2_keymap['e'] = PS2_E;
  ASCII_to_PS2_keymap['f'] = PS2_F;
  ASCII_to_PS2_keymap['g'] = PS2_G;
  ASCII_to_PS2_keymap['h'] = PS2_H;
  ASCII_to_PS2_keymap['i'] = PS2_I;
  ASCII_to_PS2_keymap['j'] = PS2_J;
  ASCII_to_PS2_keymap['k'] = PS2_K;
  ASCII_to_PS2_keymap['l'] = PS2_L;
  ASCII_to_PS2_keymap['m'] = PS2_M;
  ASCII_to_PS2_keymap['n'] = PS2_N;
  ASCII_to_PS2_keymap['o'] = PS2_O;
  ASCII_to_PS2_keymap['p'] = PS2_P;
  ASCII_to_PS2_keymap['q'] = PS2_Q;
  ASCII_to_PS2_keymap['r'] = PS2_R;
  ASCII_to_PS2_keymap['s'] = PS2_S;
  ASCII_to_PS2_keymap['t'] = PS2_T;
  ASCII_to_PS2_keymap['u'] = PS2_U;
  ASCII_to_PS2_keymap['v'] = PS2_V;
  ASCII_to_PS2_keymap['w'] = PS2_W;
  ASCII_to_PS2_keymap['x'] = PS2_X;
  ASCII_to_PS2_keymap['y'] = PS2_Y;
  ASCII_to_PS2_keymap['z'] = PS2_Z;

  ASCII_to_PS2_keymap['A'] = PS2_A;
  ASCII_to_PS2_keymap['B'] = PS2_B;
  ASCII_to_PS2_keymap['C'] = PS2_C;
  ASCII_to_PS2_keymap['D'] = PS2_D;
  ASCII_to_PS2_keymap['E'] = PS2_E;
  ASCII_to_PS2_keymap['F'] = PS2_F;
  ASCII_to_PS2_keymap['G'] = PS2_G;
  ASCII_to_PS2_keymap['H'] = PS2_H;
  ASCII_to_PS2_keymap['I'] = PS2_I;
  ASCII_to_PS2_keymap['J'] = PS2_J;
  ASCII_to_PS2_keymap['K'] = PS2_K;
  ASCII_to_PS2_keymap['L'] = PS2_L;
  ASCII_to_PS2_keymap['M'] = PS2_M;
  ASCII_to_PS2_keymap['N'] = PS2_N;
  ASCII_to_PS2_keymap['O'] = PS2_O;
  ASCII_to_PS2_keymap['P'] = PS2_P;
  ASCII_to_PS2_keymap['Q'] = PS2_Q;
  ASCII_to_PS2_keymap['R'] = PS2_R;
  ASCII_to_PS2_keymap['S'] = PS2_S;
  ASCII_to_PS2_keymap['T'] = PS2_T;
  ASCII_to_PS2_keymap['U'] = PS2_U;
  ASCII_to_PS2_keymap['V'] = PS2_V;
  ASCII_to_PS2_keymap['W'] = PS2_W;
  ASCII_to_PS2_keymap['X'] = PS2_X;
  ASCII_to_PS2_keymap['Y'] = PS2_Y;
  ASCII_to_PS2_keymap['Z'] = PS2_Z;

  ASCII_to_modifier_keymap['A'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['B'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['C'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['D'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['E'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['F'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['G'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['H'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['I'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['J'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['K'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['L'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['M'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['N'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['O'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['P'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['Q'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['R'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['S'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['T'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['U'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['V'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['W'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['X'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['Y'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['Z'] = PS2_LEFT_SHIFT;

  ASCII_to_PS2_keymap['0'] = PS2_0;
  ASCII_to_PS2_keymap['1'] = PS2_1;
  ASCII_to_PS2_keymap['2'] = PS2_2;
  ASCII_to_PS2_keymap['3'] = PS2_3;
  ASCII_to_PS2_keymap['4'] = PS2_4;
  ASCII_to_PS2_keymap['5'] = PS2_5;
  ASCII_to_PS2_keymap['6'] = PS2_6;
  ASCII_to_PS2_keymap['7'] = PS2_7;
  ASCII_to_PS2_keymap['8'] = PS2_8;
  ASCII_to_PS2_keymap['9'] = PS2_9;

  ASCII_to_PS2_keymap[')'] = PS2_0;
  ASCII_to_PS2_keymap['!'] = PS2_1;
  ASCII_to_PS2_keymap['"'] = PS2_2;
  ASCII_to_PS2_keymap['£'] = PS2_3;
  ASCII_to_PS2_keymap['$'] = PS2_4;
  ASCII_to_PS2_keymap['%'] = PS2_5;
  ASCII_to_PS2_keymap['^'] = PS2_6;
  ASCII_to_PS2_keymap['&'] = PS2_7;
  ASCII_to_PS2_keymap['*'] = PS2_8;
  ASCII_to_PS2_keymap['('] = PS2_9;

  ASCII_to_modifier_keymap[')'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['!'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['"'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['£'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['$'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['%'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['^'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['&'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['*'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['('] = PS2_LEFT_SHIFT;


  ASCII_to_PS2_keymap['`'] = PS2_ACCENT;
  ASCII_to_PS2_keymap['-'] = PS2_HYPH;
  ASCII_to_PS2_keymap['='] = PS2_EQUAL;

  ASCII_to_PS2_keymap[';'] = PS2_SEMI_COLON;
  ASCII_to_PS2_keymap['\''] = PS2_APOSTROPHY;
  ASCII_to_PS2_keymap['#'] = 0; // PS2_HASH;

  ASCII_to_PS2_keymap['\\'] = PS2_BACK_SLASH_NON_US;

  ASCII_to_PS2_keymap[','] = PS2_COMMA;
  ASCII_to_PS2_keymap['.'] = PS2_PERIOD;
  ASCII_to_PS2_keymap['/'] = PS2_FWD_SLASH;

  ASCII_to_PS2_keymap['¬'] = PS2_ACCENT;
  ASCII_to_PS2_keymap['_'] = PS2_HYPH;
  ASCII_to_PS2_keymap['+'] = PS2_EQUAL;

  ASCII_to_PS2_keymap[':'] = PS2_SEMI_COLON;
  ASCII_to_PS2_keymap['@'] = PS2_APOSTROPHY;
  ASCII_to_PS2_keymap['~'] = 0; // PS2_HASH;

  ASCII_to_PS2_keymap['|'] = PS2_BACK_SLASH_NON_US;

  ASCII_to_PS2_keymap['<'] = PS2_COMMA;
  ASCII_to_PS2_keymap['>'] = PS2_PERIOD;
  ASCII_to_PS2_keymap['?'] = PS2_FWD_SLASH;

  ASCII_to_modifier_keymap['¬'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['_'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['+'] = PS2_LEFT_SHIFT;

  ASCII_to_modifier_keymap[':'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['@'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['~'] = 0; // PS2_HASH;

  ASCII_to_modifier_keymap['|'] = PS2_LEFT_SHIFT;

  ASCII_to_modifier_keymap['<'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['>'] = PS2_LEFT_SHIFT;
  ASCII_to_modifier_keymap['?'] = PS2_LEFT_SHIFT;

  ASCII_to_PS2_keymap[' '] = PS2_SPACE;
}

