
<iframe width="560" height="315" src="https://www.youtube.com/embed/S7Bd992k368?si=Cdnbv742X09ewYGh" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

# Introduction


For a long time I have been inspired by self-playing pianos — it’s something about the way the keys move on their own that make them so amazing. From this I found myself with a desire to create my own version of a self-playing piano at an affordable cost. My family had a rarely-used upright piano and an Arduino Uno that had been sitting in a drawer for years, and for the first time I tinkered with the two to see if I could turn my idea into a reality.

This desire transformed into a 6-month long personal project that led to the video artifact above and the creation of this repo. The first half of the video features a time-lapse of the entire build process put together from videos I took during the project's development. The second half of the video showcases the result of the system. I accomplished every goal for the system by replicating any keystroke, maintaining the manual functionality of the piano, and reducing the cost of installing a self-playing system by 90%.

The goal of this document is to stand as a technical specification for the features and design of the system. It will start with a list of the system's features, followed by an overview of the system's hardware, followed by the system's software design and algorithms. Those who are interested in building their own systems or are simply curious about the design of the system should find this useful. This repo is free to fork for personal use.

Those who are interested in building their own systems should also join the community [Discord server](https://discord.com/invite/reenxNyht5)! The server is nearing 500 members and is filled with other builders in the process of getting their own systems up and running.

# Features

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

### Keys

Each piano key must be independently pressed by a physical actuator. Therefore, there are 88 key actuators in total. Each actuator must:
- Move vertically, corresponding to the vertical pressing of a key
- Be strong enough to push its key up and down at a reasonable speed
- Be power-efficient enough to enable 10+ keys (corresponding to 10 fingers) to be played at once with a reasonably-sized power supply
- Not get dangerously hot on a reasonable duty cycle of 10 seconds
- Be small enough so that all 88 of them can fit into a few vertical rows to maintain a compact profile

10 is chosen as the threshold for the minimum number of keys that should be supported to be played at once because most songs are written to be played with 10 fingers on a piano. Still, some MIDI files express their sound with 20 or even 30 keys at once, so it is preferred that the system enables as many keys as possible to be played at the same time. 

Additionally, 10 seconds is chosen as a reasonable duty cycle for the actuator because it takes about 10 seconds for a loudly-played key’s sound to completely diminish. After 10 seconds, the actuator should automatically turn off in the software if not signaled to by the MIDI file.

With these requirements and some testing, I settled on the JF-1039B solenoid, which can be bought in bulk on Alibaba. The solenoid is rated at 12v 2a, which enables the system to play up to 40 keys at once with two 200w power supplies. It has a force of 30 N at its rated voltage, which can push each key with reasonable power and speed. It takes 40 seconds to get unreasonably hot at its rated voltage, well above the 10 second requirement. It is also small enough for 88 them to fit into two rows side-by-side, enabling them to be fit into a small space. 

The solenoids are mounted into two vertical rows on a thin mounting board. The mounting board is then mounted under the piano near the strings. This situates the solenoids on the opposite side of the lever from where a piano player pushes the keys, allowing the solenoids to push up from under the piano rather than down to activate the keys. The vertical rows also maintain a thin profile for the solenoids, allowing the entire mounting board to be hidden behind the removable board on the piano that usually covers the strings. A hole under each key is drilled in the piano's base board and a metal rod extension is mounted on each solenoid, allowing each solenoid to access its key from under the piano. A side view of a key with its solenoid is shown below. 

### Damper

### Control

Each key's solenoid must be controlled by an individual signal via some microcontroller. Microcontrollers enable high-level logic to be integrated into the electronics via code. Additionally, to support volume control, these signals must support pulse-width modulation (PWM), which controls the solenoid’s power and therefore how hard it hits its key. Most microcontrollers are rated between 3.3-5v, so microcontroller signals are stepped up to 12v via a transistor (model).

This also raises the challenge of output pin limitations. Almost no microcontrollers have enough PWM output pins to map to each of the 88 keys on the piano. While it is possible to assign a separate microcontroller to control sets of 11 keys or so and communicate among them (see alternative designs below), I decided to expand the output pins of a single microcontroller using TX-34 shift registers. These shift registers expand 3 microcontroller pins into an arbitrarily large number of output signals, including PWM support for each signal. This allows all solenoid actuation for keys to be controlled by a single central microcontroller, although multiple microcontrollers are still used in the system.

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
