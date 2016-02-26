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


/**
 * Initial setup
 */
void setup() {
  if(DEBUG){
    Serial.begin(115200); //debuging
    while (!Serial);  // required for Flora & Micro
    delay(500);
  }
  setup_keymaps();
  setup_PS2();
  start_BLE(0); //use 1 to configure the bluetooth module(or press ctrl + shift + esc)
}

/**
 * Main loop - repeats processing the entered PS2 keys
 */
void loop() {
  process_buffer();
}




