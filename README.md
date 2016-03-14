# BLE_WASD
Bluetooth enabled WASD keyboard
See https://hackaday.io/project/7646-ble-wasdmechanical-keyboard for build instructions!

This fork contains improvements from original to

a) fix some bugs
b) add two way PS2 comms so that capslock/numlock/scrolllock LEDS work
c) provide an optional "wired" mode. The thought here is to be able to use as a wired keyboard without unplugging the bluefruit from the keyboard and possibly benefit from some improved reduction in latecy when we are wired. The ability to send the media keys from the CODE keyboard also has to be preserved here and this requires some addition to the arduino library code

WORK IN PROGRESS
