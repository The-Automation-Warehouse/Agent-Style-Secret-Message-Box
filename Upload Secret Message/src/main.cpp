/*
 * This program stores the secret message to arduino EEPROM.
  * The user uploads the first program to the arduino.
  * The first program stores the secret message to the EEPROM.
  * After storing the message the screen displays "Secret message stored in EEPROM, please upload second program".
  * And the box beeps using the buzzer.
  * 
 * After storing the message,the user uploads the second program to the arduino.
  * The second program reads the message from the EEPROM and displays it on the LCD.
  * The user can see the secret message on the LCD.
  * After reading the message, the user confimrs the message by pressing the BIG RED BUTTON.
  * If the user presses the button, the message is deleted from the EEPROM.
  * 
 */

#include <Arduino.h>
#include "LiquidCrystal_I2C.h"
#include <Wire.h>
#include <EEPROM.h>

// ↓↓ Set the message to be stored in the EEPROM here ↓↓
String secretMessage = "Hello, World!, This is a secret message stored in the EEPROM";

#define I2C_ADDR    0x26 // I2C Address of the module
#define buzzer 5 // Buzzer pin

LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

void setup() {
  // Initialize the lcd and buzzer
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  // Initialize EEPROM
  EEPROM.begin();

  // Store the secret message to the EEPROM with the END OF MESSAGE character at the end and wipe the rest of the EEPROM
  // Wipe the EEPROM
  for (unsigned int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }

  // Store the message
  for (unsigned int i = 0; i < secretMessage.length(); i++) {
    EEPROM.write(i, secretMessage[i]);
  }
  
  // Store the end of message character
  EEPROM.write(secretMessage.length(), '\0');

  // Beep the buzzer 5 times
  for (int i = 0; i < 5; i++) {
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }

  // Print the message on the serial monitor for debugging by reading the message from the EEPROM
  Serial.begin(115200);
  Serial.println("Secret message stored in EEPROM, please upload the second program");

  // Read the secret message from the EEPROM into a new string
  String readMessage = "";
  for (unsigned int i = 0; i < 512; i++) {
    char c = char(EEPROM.read(i));
    if (c == '\0') {
      break;
    }
    readMessage += c;
  }

  // Print the retrieved message
  Serial.println("Secret message: " + readMessage);
}

void loop() {
  // Scroll the message on the LCD (each line at a time, 16 characters)
  lcd.setCursor(0, 0);
  lcd.print("Secret message  ");
  lcd.setCursor(0, 1);
  lcd.print("stored in EEPROM");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("Please upload   ");
  lcd.setCursor(0, 1);
  lcd.print("second program  ");
  delay(2000);
}

