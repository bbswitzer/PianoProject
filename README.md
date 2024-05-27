# Introduction

<iframe width="560" height="315" src="https://www.youtube.com/embed/S7Bd992k368?si=Cdnbv742X09ewYGh" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>


For a long time I had been inspired by self-playing pianos — it’s something about the way the keys move on their own that make them so amazing. From this I found myself with a desire to create my own version of a self-playing piano at an affordable cost. My family had a rarely-used upright piano and an Arduino Uno that had been sitting in a drawer for years, and for the first time I tinkered with the two to see if I could turn my idea into a reality.

This desire transformed into a 6-month long personal project that led to the video artifact above and the creation of this repo. The first half of the video features a time-lapse of the entire build process put together from videos I took during the project's development. The second half of the video showcases the result of the system. I accomplished every goal for the system by replicating any keystroke, maintaining the manual functionality of the piano, and reducing the cost of installing a self-playing system by 90%.

One of my goals in creating this was to show that it’s possible to bootstrap amazing things for little money, and I believe that I proved this. While a player system from Yamaha or Pianodisc cost upwards of $10,000, I built my own system for under $700. Not only that, but many brands of player pianos require their customers to purchase individual songs to play on the piano, while my system runs on free MIDI files that can be found on the Internet. 

This document serves as a specification for the features and design of the system. Those who are interested in building their own systems or are curious about the design of the system should find this useful. This repo is free to fork for personal use.

Those who are interested in building their own systems should also join the community [Discord server](https://discord.com/invite/reenxNyht5)! The server is nearing 500 members and is filled with other builders in the process of getting their own systems running.

## Features

- Plays any song from any MIDI stream via a Bluetooth or USB connection, accurately reflecting its sound on the piano
- Is recognized as a MIDI device from a Bluetooth or USB connection, enabling the user to stream MIDI files from their device in a plug-and-play fashion
- Plays each note louder or softer depending on the MIDI file's velocity information
- Maintains the manual functionality of the piano, even while the piano is playing; Allows the user to play songs manually without the self-playing features of the system or in a duet with the system playing
- Includes control of the damper (sustain) pedal
- Allows the user to turn the system on/off and access options via a control box on the front of the piano
- Supports additional volume control via a dial on the control box
- Maintains a discreet profile, with the electronics hidden away and only a single power cord used for the entire system

# System Design

## Hardware

## Software

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
