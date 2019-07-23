#ifndef SERIAL_H
#define SERIAL_H

void checkForSerial();
void sendMidiToProMicro(byte note, byte velocity);
void customSerialToProMicro(byte header, byte note, byte velocity);

#endif