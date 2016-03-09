#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"
#include "define.h"

#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
#include <SoftwareSerial.h>
#endif

/**
 * BLE WASD Keyboard V2
 *
 * Daniel Nugent
 * Dorian Rudolf
 * Michael Cox
 *
 * This program uses an adafruit BLE + Micro microcontroller to decode PS2, remap the scan codes to HID, and send over bluetooth GAT
 * Additional parts include the the tricket lipo backpack, WASD code keyboard, microusb male, and 3.7v batteries
 *
 * Ctrl + Shift + esc will reconfigure the keyboard
 *    -Use on first use or if the bluetooth module freezes or changes name
 *
 * Make sure you keyboard can run on 3.3v or make sure you can provide  it with 5v
 *
 */

// Create the bluefruit object, either software serial...uncomment these lines
/*
/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

static char buffer2[BUFFER_SIZE+1];

/**
 * Initial setup
 */
void setup() {
  if (DEBUG) {
    Serial.begin(115200); //debuging
    // If USB connection is not there this can cause failure to start
    while (!Serial)
      ;  // required for Flora & Micro
    delay(500);
  }
  setup_keymaps();
  setup_PS2();
  start_keyboard();
}

/**
 * Main loop - repeats processing the entered PS2 keys
 */
void loop() {
  if (FAKE) {
    uint8_t count = getUserInput(buffer2, 20);
    if (count) {
      Serial.print("Got: ");
      Serial.println(buffer2);
      for (int i = 0; i < count; ++i) {
         uint8_t ps2Key = ASCII_to_PS2_keymap[buffer2[i]];   
         uint8_t modifier = ASCII_to_modifier_keymap[buffer2[i]];  
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
  process_buffer();
}

uint8_t getUserInput(char buffer2[], uint8_t maxSize)
{
  // timeout in 100 milliseconds
  TimeoutTimer timeout(100);

  memset(buffer2, 0, maxSize);
  while( (!Serial.available()) && !timeout.expired() ) { delay(1); }

  if ( timeout.expired() ) return false;

  delay(2);
  uint8_t count=0;
  do
  {
    count += Serial.readBytes(buffer2+count, maxSize);
    delay(2);
  } while( (count < maxSize) && (Serial.available()) );

  return count;
}




