/*
 * This is the second program to be uploaded to the arduino.
  * This program reads the secret message from the EEPROM and displays it on the LCD.
  * While the message is printing the buzzer beeps for each character.
  * The user can see the secret message on the LCD and scroll through the message using two buttons.
  * After reading the message, the user confirms the message by pressing the BIG RED BUTTON.
  * If the user presses the button, the message is deleted from the EEPROM forever.
  * 
 */


#include <Arduino.h>
#include <Wire.h>
#include "LiquidCrystal_I2C.h"
#include <EEPROM.h>

String secretMessage;
String messageToDisplay[32]; // Array to store the message divided into lines
int currentLine = 0; // Current line to display
int lineCount = 0; // Number of lines in the message

#define I2C_ADDR    0x26 // I2C Address of the module
#define buzzer 5 // Buzzer pin
#define topButton 3 // Top button pin
#define bottomButton 4 // Bottom button pin
#define redButton 4 // Red button pin

LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

void displayMessage();
void scrollUp();
void scrollDown();

void setup() {

  Serial.begin(115200);
  Serial.println("Secret message reader started");
  
  // Initialize the lcd, buzzer and buttons
  Wire.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  pinMode(topButton, INPUT_PULLUP);
  pinMode(bottomButton, INPUT_PULLUP);
  pinMode(redButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(topButton), scrollUp, FALLING);
  attachInterrupt(digitalPinToInterrupt(bottomButton), scrollDown, FALLING);

  EEPROM.begin();

  // Beep the buzzer 5 times 
  for (int i = 0; i < 5; i++) {
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }

  // Read the secret message from the EEPROM
  // Continue reading the message until the END OF MESSAGE character is found
  for (unsigned int i = 0; i < 512; i++) {
    char c = char(EEPROM.read(i));
    if (c == '\0') {
      break;
    }
    secretMessage += c;
  }

  // Print the message on the serial monitor for debugging

  Serial.println("Secret message: " + secretMessage);

  // Divide the message into parts (max 16 characters per line and words are not split)
  // loop through the message and check for spaces and when the line is full, store it in the array
  int charCount = 0;
  for (unsigned int i = 0; i < secretMessage.length(); i++) {
    if (secretMessage[i] == ' ') {
      if (charCount > 15) {
        lineCount++;
        charCount = 0;
      }
    }
    messageToDisplay[lineCount] += secretMessage[i];
    charCount++;
  }

  displayMessage();


}

void loop() {
  // Do nothing
}


void displayMessage() {
  // Display the message on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(messageToDisplay[currentLine]);
  lcd.setCursor(0, 1);
  lcd.print(messageToDisplay[currentLine + 1]);
}

void scrollUp() {
  // Scroll up the message
  if (currentLine > 0) {
    currentLine--;
    displayMessage();
  }
}

void scrollDown() {
  // Scroll down the message
  if (currentLine < lineCount - 1) {
    currentLine++;
    displayMessage();
  }
}
