# Agent-Style-Secret-Message-Box
Inspired by the movie series "Mission Impossible", I want to create a way to send secret messages to someone in a secure way.

I got the idea to use the EEPROM of an arduino, since it is non-volatile and can store data even when the power is off nad it cant be recovered when overwritten.


This project consits of: arduino nano, 16x2 LCD, 1 push buttons, 1 buzzer, 1 bigger button, toggle switch, battery with TP4056 charging module, and a 3D printed case.

The user can write a secret message to the EEPROM by uploading the first program to the arduino.
The second program reads the secret message from the EEPROM and displays it on the LCD.

The user can then scroll through the message using two buttons.
After reading the message, the user confirms the message by pressing the BIG RED BUTTON.
If the user presses the button, the message is deleted from the EEPROM forever.

The user can upload a new message to the EEPROM by uploading the first program again.
