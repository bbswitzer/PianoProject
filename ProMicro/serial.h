#ifndef SERIAL_H
#define SERIAL_H

void checkForSerial();
void sendSerialToMain(byte header, byte setting, byte value);
void sendSerialToUSB(String* message, int lengthOfMessage);

#endif