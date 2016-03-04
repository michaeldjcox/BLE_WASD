/*
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
  static uint8_t bitcount=0;
  static uint8_t incoming=0;
  static uint32_t prev_ms=0;
  uint32_t now_ms;
  uint8_t n, val;
  
  if (!sendBits){
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
      uint8_t i = head + 1;
      if (i >= BUFFER_SIZE) i = 0;
      if (i != tail) {
        buffer[i] = incoming;
        head = i;
      }
    if(DEBUG){
      Serial.print("Raw PS2: ");
      Serial.println(incoming,HEX);
    }
      bitcount = 0;
      incoming = 0;
    }
  } else {
    --sendBits;
    uint8_t b = bitCount - 1;
    if (b == 8){
      digitalWrite(DATA_PIN, !(setBits & 1));
    } else if (b == 9) {
      pinMode(DATA_PIN, INPUT_PULLUP);
    } else if (b < 8) {
      bool bt = (msg >> b) & 1;
      digitalWrite(DATA_PIN,  bt);
      setBits += bt;
    }
    ++bitCount; 
  }
}

/**
 * Reads the next PS2 key code from the buffer
 */
static inline uint8_t get_scan_code(void) {
  uint8_t c, i;

  i = tail;
  if (i == head) return 0;
  i++;
  if (i >= BUFFER_SIZE) i = 0;
  c = buffer[i];
  tail = i;
  return c;
}

/**
 * Gets the next PS2 key press from the buffer and decides what to do in response
 */
void process_buffer() {
    uint8_t k = get_scan_code();
    uint8_t k2;
    if (k) {
        if (skip) {
            --skip;
        } else {
            if (k == PS2_EXTENDED) {
                ext = true;
            } else if (k == PS2_RELEASE) {
                brk = true;
            } else if (k == PS2_LED_ACK) {
                if (sendLeds) {
                    sendLeds = false;
                    send_msg(leds);
                }
            } else {
                if (k == PS2_PAUSE_SEQUENCE) {
                    k2 = HID_PAUSE;
                    skip = 7;
                    brk = true;
                    report_add(k2);
                    send_report();
                } else {
                    k2 = ext ? PS2Long_to_HID_keymap[k] : PS2_to_HID_keymap[k];
                }

                if (k2) {                  
                    boolean send_key = special_functions(k2, brk);
                    if (send_key) {
                        if (brk){
                            report_remove(k2);
                            if (k2 == HID_NUM_LCK || k2 == HID_SCR_LCK || k2 == HID_CAPS){
                                sendLeds = true;
                                if (k2 == HID_NUM_LCK) {
                                    leds ^= 2;
                                } else if (k2 == HID_SCR_LCK) {
                                    leds ^= 1;
                                } else if (k2 == HID_CAPS) {
                                    leds ^= 4;
                                }
                                send_msg((byte) HID_SET_RESET_LEDS);
                            }
                        } else {
                            report_add(k2);
                        }
                        send_report();
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
void send_msg(uint8_t ps2Msg) {
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
 * Sets up the keyboard mapping from PS2 to HID
 */
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


