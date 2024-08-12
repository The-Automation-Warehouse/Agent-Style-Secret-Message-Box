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
String words[50]; // Array to store the words of the message
String messageToDisplay[25]; // Array to store the message divided into lines
int currentLine = 0; // Current line to display
int lineCount = 0; // Number of lines in the message
int wordCount = 0; // Number of words in the message
bool displayMessageFlag = false; // Flag to indicate if the message should be displayed
bool scrollUpFlag = true; // Flag to indicate if the message should be scrolled up
bool scrollDownFlag = true; // Flag to indicate if the message should be scrolled down
bool lastLine = false; // Flag to indicate if the last line is being displayed
// Last resort bodge to get the interrupts to work
bool doInterrupts = false;

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

  Serial.println("Reading secret message from EEPROM");

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
        words[wordCount] = word;
        word = "";
        wordCount++;
      }
    } else {
      word += currentChar;
    }
  }

  // If there is a word left, add it to the array
  if (word.length() > 0) {
    words[wordCount] = word;
    wordCount++;
  }

  // Print the words on the serial monitor for debugging
  for (int i = 0; i < wordCount; i++) {
    Serial.println(words[i]);
  }
  Serial.println();

  Serial.println("Words count: " + String(wordCount));
  Serial.println();


  // Divide the message into lines (max 16 characters per line)
  String line = "";
  int currentLength = 0;
  for (int i = 0; i < wordCount; i++) {
    if (currentLength + words[i].length() + 1 <= 16) {
      line += words[i] + " ";
      currentLength += words[i].length() + 1;
    } else {
      messageToDisplay[currentLine] = line;
      line = words[i] + " ";
      currentLength = words[i].length() + 1;
      currentLine++;
      lineCount++;
    }
  }

  // Add the last line to the array
  messageToDisplay[currentLine] = line;
  lineCount++;

  // Print the lines on the serial monitor for debugging
  for (int i = 0; i <= currentLine; i++) {
    Serial.println(messageToDisplay[i]);
  }
  Serial.println();

  Serial.println("Lines count: " + String(lineCount));
  Serial.println();

  currentLine = 0;

  displayMessage();


}

void loop() {

  if (displayMessageFlag) {
    displayMessage();
    displayMessageFlag = false;
  }

  if (lastLine) {
    // If the last line is reached, wait for the user to press the red button
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hold red button  ");
    lcd.setCursor(0, 1);
    lcd.print("to confim");

    // Wait for the user to hold the red button
    // Count down from 5 on the entire screen
    bool deleteFlag = false;

    while (deleteFlag == false) {
      if (digitalRead(topButton) == LOW) {
        break;
      }
      while (digitalRead(redButton) == LOW) {
        for (int i = 5; i > 0; i--) {
          // Fill the lcd with the number
          lcd.clear();
          lcd.setCursor(0, 0);
          for (int j = 0; j < 16; j++) {
            lcd.print(i);
          }
          lcd.setCursor(0, 1);
          for (int j = 0; j < 16; j++) {
            lcd.print(i);
          }
          if (digitalRead(redButton) == HIGH) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Hold red button  ");
            lcd.setCursor(0, 1);
            lcd.print("to confim");
            break;
          }
          delay(1000);
        }
        if (digitalRead(redButton) == LOW) {
          deleteFlag = true;
          break;
        }
      }
    }

    if (deleteFlag == true) {
      // Delete the message from the EEPROM

      lcd.clear();

      // Beep the buzzer 2 times
      for (int i = 0; i < 2; i++) {
        digitalWrite(buzzer, HIGH);
        delay(300);
        digitalWrite(buzzer, LOW);
        delay(300);
      }

      for (unsigned int i = 0; i < 512; i++) {
        EEPROM.write(i, 0);
      }

      Serial.println("Secret message deleted from EEPROM");

      lcd.setCursor(0, 0);
      lcd.print("Message deleted");

      deleteFlag = false;

      while (true) {}
    }

    currentLine = 0;
    lastLine = false;
    scrollDownFlag = true;
    scrollUpFlag = true;
    displayMessageFlag = true;
  }
}


void displayMessage() {
  // Display the message on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  // Print the line character by character with a beep
  for (unsigned int i = 0; i < messageToDisplay[currentLine].length(); i++) {
    if (scrollUpFlag) {
      digitalWrite(buzzer, HIGH);
      lcd.print(messageToDisplay[currentLine][i]);
      delay(20);
      digitalWrite(buzzer, LOW);
      delay(20);
    } else {
      lcd.print(messageToDisplay[currentLine][i]);
    }

  }
  
  lcd.setCursor(0, 1);
  // Print the next line character by character with a beep
  for (unsigned int i = 0; i < messageToDisplay[currentLine + 1].length(); i++) {
    if (scrollDownFlag) {
      digitalWrite(buzzer, HIGH);
      lcd.print(messageToDisplay[currentLine + 1][i]);
      delay(20);
      digitalWrite(buzzer, LOW);
      delay(20);
    } else {
      lcd.print(messageToDisplay[currentLine + 1][i]);
    }
  }
  scrollDownFlag = false;
  scrollUpFlag = false;
  
  delay(100);
  doInterrupts = true;
}

void scrollUp() {

  if (doInterrupts) {
    doInterrupts = false;
    Serial.println("Scroll up");
    // Scroll up the message
    if (currentLine > 0) {
      currentLine--;
      scrollUpFlag = true;
      displayMessageFlag = true;
    }
  }

  
}

void scrollDown() {
  
  if (doInterrupts) {
    doInterrupts = false;
    Serial.println("Scroll down");
    // Scroll down the message
    if (currentLine < lineCount - 2) {
      currentLine++;
      scrollDownFlag = true;
      displayMessageFlag = true;
    } else {
      lastLine = true;
    }
  }
}
