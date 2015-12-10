//  _ ___ _______     ___ ___ ___  ___ _   _ ___ _____ ___ 
// / |_  )__ /   \   / __|_ _| _ \/ __| | | |_ _|_   _/ __| 
// | |/ / |_ \ |) | | (__ | ||   / (__| |_| || |  | | \__ \ 
// |_/___|___/___/   \___|___|_|_\\___|\___/|___| |_| |___/ 
// 
// LCD Display and Arduino
// 
// Made by Hino Hau
// License: Public Domain
// Downloaded from: https://123d.circuits.io/circuits/1355136-lcd-display-and-arduino

#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int button = 2 ;
int led = 5 ;
int value ;

void setup() {
  pinMode (button , INPUT );
  pinMode (led , OUTPUT );
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Welcome to MON");
  delay(5000);
}

void loop() {
 
  value = digitalRead (button);
  int buttonPressed = 0
  
  lcd.clear();
  char s1[ ] = "Name:";
  char s2[ ] = "Kwok Hin Hau";
  char s3[ ] = "Status:";
  char s4[ ] = "Available";
  char s5[ ] = "ID:";
  char s6[ ] = "463473584";
  char s7[ ] = "Age:";
  char s8[ ] = "20";
  
  lcd.setCursor(0, 0);
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
  delay(5000);
  if (value == HIGH)
    if (buttonPressed == 0)
    buttonPressed = 1;
    else
    buttonPressed = 0;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(s3);
  lcd.setCursor(0, 1);
  lcd.print(s4);
  delay(5000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(s5);
  lcd.setCursor(0, 1);
  lcd.print(s6);
  delay(5000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(s7);
  lcd.setCursor(0, 1);
  lcd.print(s8);
  delay(5000);
  
  
  
 
  } 
