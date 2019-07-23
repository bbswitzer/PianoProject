# PianoProject
Arduino Code for a DIY Player Piano.

See more information about it here: brandonswitzer.squarespace.com/player-piano

Role of each of the Arduinos:

  ESP32:
  
    - Receive and decode bluetooth message
    
    - Schedules notes
    
    - Handles sustain
    
    - Handles setting changes
    
  Pro Micro:
  
    - Receives data from ESP32 and/or USB
    
    - Activates notes using shift registers
    
  Control Box:
  
    - Interface for changing settings
    
    - Receives data from USB and sends it to ESP32
    
    - Reset button
