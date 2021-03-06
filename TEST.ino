
#if defined (TEST_SERIAL_INPUT) || defined (TEST_HELLO_WORLD)

/**
 * Test functions that permit typing ASCII into the serial monitor and have it converted to PS2 keycodes and
 * injected into the buffer as if a PS2 keyboard was connected to the bluefruit.
 *
 * This is for offline working when a suitable keyboard is not connected to the bluefruit LE micro
 */
uint8_t ASCII_to_PS2_keymap[255];
uint8_t ASCII_to_modifier_keymap[255];

static char inputBuffer[TEST_BUFFER_SIZE];
static uint8_t count = 0;

uint32_t startup_ms = 0;

/**
 * Reads ascii input from the serial comms and adds it as PS key codes to the PS2 buffer
 */
void test_serial_input() {

  get_test_input(inputBuffer, TEST_BUFFER_SIZE);
  if (count > 0) {

    process_test_input();
    count = 0;
  }
}

/**
 * Adds a ascii input string as a sequence of PS2 keycodes to the buffer
 */
void test_input(String input) {

  input.toCharArray(inputBuffer, TEST_BUFFER_SIZE);
  count = input.length();
  process_test_input();
}

/**
 * Handles the LED message sent back to the keyboard
 */
void test_ps2_msg(uint8_t ps2Msg) {
  if (ps2Msg == PS2_SET_RESET_LEDS) {
    add_to_buffer(PS2_LED_ACK);
  } 
#if defined (DEBUG) 
  debug_ps2_msg(ps2Msg);
#endif
}

/**
 * Returns the amount fo free memory
 */
void free_mem () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  int freeRam = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 

#if defined (DEBUG) 
  Serial.print(F("Free RAM: "));
  Serial.print(freeRam);
  Serial.println();
#endif
}

/**
 * Processes some ascii characters from the input buffer and converts to
 * PS2 keycodes
 */
void process_test_input() {
  String result = inputBuffer;

#if defined (DEBUG) 
    Serial.print(F("Test input: ("));
    Serial.print(count);
    Serial.print(F(") "));
    Serial.println(result);
#endif


  if (result.equals("MEM")) {
    free_mem();
  } else
  if (result.equals("PLAYPAUSE")) {
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_PLAY_PAUSE);
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_PLAY_PAUSE);
  } else if (result.equals("STOP")) {
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_STOP);
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_STOP);
  } else if (result.equals("NEXT")) {
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_NEXT_TRACK);
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_NEXT_TRACK);
  } else if (result.equals("PREV")) {
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_PREV_TRACK);
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_PREV_TRACK);
  } else if (result.equals("VOLUP")) {
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_VOL_UP);
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_VOL_UP);
  } else if (result.equals("VOLDOWN")) {
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_VOL_DWN);
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_VOL_DWN);
  } else if (result.equals("MUTE")) {
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_MUTE);
    add_to_buffer(PS2_EXTENDED);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_MUTE);
  } else if (result.equals("SWITCH")) {
    add_to_buffer(PS2_LEFT_CTRL);
    add_to_buffer(PS2_ESC);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_ESC);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_LEFT_CTRL);
  } else if (result.equals("RESET")) {
    add_to_buffer(PS2_LEFT_CTRL);
    add_to_buffer(PS2_LEFT_SHIFT);
    add_to_buffer(PS2_ESC);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_ESC);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_LEFT_SHIFT);
    add_to_buffer(PS2_RELEASE);
    add_to_buffer(PS2_LEFT_CTRL);
  } else {
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

/**
 * Reads a sequence of ASCII character input over the serial interface
 *
 * This is useful for testing the program via the serial monitor when no PS2 keyboard is available.
 */
void get_test_input(char inputBuffer[], uint8_t maxSize)
{
  // timeout in 100 milliseconds
  TimeoutTimer timeout(100);

  memset(inputBuffer, 0, maxSize);
  while ( (!Serial.available()) && !timeout.expired() ) {
    delay(1);
  }

  if ( timeout.expired() ) return;

  delay(2);
  count = 0;
  do
  {
    count += Serial.readBytes(inputBuffer + count, maxSize);
    delay(2);
  } while ( (count < maxSize) && (Serial.available()) );
}

/**
 * Put this in the process loop and if 10 seconds has passed since startup "hello world"
 * PS2 input will be injected into the buffer.
 *
 * This is useful for testing detached from the serial cable when no PS2 keyboard is available.
 */
void test_hello_world() {
  if (startup_ms == 0) {
    startup_ms = millis();
  }
  uint32_t now_ms = millis();
  if ((now_ms - startup_ms) > 10000) {
    test_input("hello world ");
    startup_ms  = now_ms;
  }
}

/**
 * Allows the red LED near to the USB port to be flashed.
 *
 * This was useful for diagnosing where the code had got to when debugging why
 * things were not working when detached from serial monitor.
 */
void blink(uint8_t count) {
  for (int i = 0; i < count; ++i) {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);              // wait for a second
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);              // wait for a second
  }
}


/**
 * Sets up the keyboard mapping from PS2 to HID
 */
void setup_test_keymaps() {
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

#endif




