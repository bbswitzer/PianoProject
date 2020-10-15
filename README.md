# Piano Project
Arduino Code for a DIY Player Piano.

See more information about it here: brandonswitzer.squarespace.com/player-piano

Role of each of the Arduinos:

  ESP32:
  
    - Receives and decodes bluetooth message
    
    - Schedules notes
    
    - Handles sustain
    
    - Handles setting changes
    
  Pro Micro:
  
    - Receives data from ESP32 and/or USB
    
    - Activates notes using shift registers
    
  Control Box (Pro Micro):
  
    - Interface for changing settings
    
    - Receives data from USB and sends it to ESP32
    
    - Reset button
    
Required Arduino Libraries:
  - ShiftPWM (Pro Micro)
  - MIDIUSB  (Pro Micro & Control Box)
  - LiquidCrystal_I2C (Control Box)
  - Arduino Core for ESP32 (https://github.com/espressif/arduino-esp32)
