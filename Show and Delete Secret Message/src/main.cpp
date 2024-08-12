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

String secretMessage = ""; // Variable to store the secret message
String words[64]; // Array to store the words of the message
String messageToDisplay[32]; // Array to store the message divided into lines
int currentLine = 0; // Current line to display
int lineCount = 0; // Number of lines in the message

#define I2C_ADDR    0x26 // I2C Address of the module
#define buzzer 5 // Buzzer pin
#define topButton 2 // Top button pin
#define bottomButton 3 // Bottom button pin
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

  delay(1000);

  char c;
  // Read the secret message from the EEPROM
  // Continue reading the message until the END OF TEXT character is reached
  for (unsigned int i = 0; i < 512; i++) {
    c = EEPROM.read(i); // Read a byte from EEPROM
    if (c == '\0') {
      break; // Stop reading when the null character is found
    }
    secretMessage += c; // Append the character to the secretMessage string
  }

  // Print the message on the serial monitor for debugging
  Serial.println("Secret message: " + secretMessage);
  Serial.println();


  // Divide the message into words (max 16 characters per word)
  String word = "";
  for (unsigned int i = 0; i < secretMessage.length(); i++) {
    char currentChar = secretMessage[i];
    if (currentChar == ' ' || currentChar == '\n' || currentChar == '\r') {
      if (word.length() > 0) {
        words[lineCount] = word;
        word = "";
        lineCount++;
      }
    } else {
      word += currentChar;
    }
  }

  // If there is a word left, add it to the array
  if (word.length() > 0) {
    words[lineCount] = word;
    lineCount++;
  }

  // Print the words on the serial monitor for debugging
  for (int i = 0; i < lineCount; i++) {
    Serial.println(words[i]);
  }
  Serial.println();


  // Divide the message into lines (max 16 characters per line)
  String line = "";
  int currentLength = 0;
  for (int i = 0; i < lineCount; i++) {
    if (currentLength + words[i].length() + 1 <= 16) {
      line += words[i] + " ";
      currentLength += words[i].length() + 1;
    } else {
      messageToDisplay[currentLine] = line;
      line = words[i] + " ";
      currentLength = words[i].length() + 1;
      currentLine++;
    }
  }

  // Add the last line to the array
  messageToDisplay[currentLine] = line;

  // Print the lines on the serial monitor for debugging
  for (int i = 0; i <= currentLine; i++) {
    Serial.println(messageToDisplay[i]);
  }
  Serial.println();

  currentLine = 0;
  displayMessage();


}

void loop() {
  // Do nothing
}


void displayMessage() {
  // Display the message on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  // Print the line character by character with a beep
  for (unsigned int i = 0; i < messageToDisplay[currentLine].length(); i++) {
    digitalWrite(buzzer, HIGH);
    lcd.print(messageToDisplay[currentLine][i]);

    digitalWrite(buzzer, LOW);

  }
  
  lcd.setCursor(0, 1);
  // Print the next line character by character with a beep
  for (unsigned int i = 0; i < messageToDisplay[currentLine + 1].length(); i++) {
    digitalWrite(buzzer, HIGH);
    lcd.print(messageToDisplay[currentLine + 1][i]);

    digitalWrite(buzzer, LOW);

  }
  
}

void scrollUp() {
  // Scroll up the message
  Serial.println("Scroll up");
  if (currentLine > 0) {
    currentLine--;
    displayMessage();
  }
}

void scrollDown() {
  // Scroll down the message
  Serial.println("Scroll down");
  if (currentLine < lineCount - 1) {
    currentLine++;
    displayMessage();
  }
}
