
//IO Pin definitions.
#define sclk 2
#define mosi 3
#define dc   4
#define cs   5
#define rst  6

//Color definitions
#define BLACK           0x0000
#define RED             0xF800
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define GRAY            0xBDF7

//Graphics libraries for the TFT display.
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

//Libraries for the NFC controller.
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include "SoftwareSerial.h"

//Other libraries.
#include <FileIO.h>
#include <avr/wdt.h>
#include <SPI.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"



//Hardware object declarations.;
PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);
FPS_GT511C3 fps(8, 9);

//String array for storing user data parsed from the stored text file.
String displayString[12];

String bstr = "Waiting for a card";
boolean enrolled = true;

void setup() {

  //Initialise TFT
  tft.begin();
  tft.fillRect(0, 0, 128, 128, BLACK);
  delay(100);

  //Display start-up splash screen, followed by the registered user's profile details.
  startupscreen();
  getData();
  profileDetails();

  //Initialise NFC.
  Serial.begin(9600);
  Serial.println("MON NDEF Test");
  nfc.begin();

}

void loop() {
}

//Parse text from the stored text file on the SD card and populate the displayString array with it.
void getData()
{
  Bridge.begin();
  FileSystem.begin();
  String dataString;
  dataString = "";
  int i = 0;
  File dataFile = FileSystem.open("/mnt/sd/profile.txt", FILE_READ);
  char readchar = (char)dataFile.read();
  while (readchar != EOF) {
    while (readchar != '\n') {
      dataString += readchar;
      readchar = (char)dataFile.read();
    }
    displayString[i] = dataString;
    dataString = "";
    readchar = (char)dataFile.read();
    i++;
  }
  dataFile.close();
}

void startupscreen() {
  tft.fillScreen(BLACK);
  tft.setCursor(40, 35);
  tft.setTextSize(3);
  tft.setTextColor(GRAY);
  tft.println("MON");
  delay(3000);
  tft.fillScreen(BLACK);
  tft.setCursor(40, 70);
  tft.setTextSize(3);
  tft.setTextColor(GRAY);
  tft.setTextSize(1);
  tft.println("NFC DEMO");
}

//Displays profile details.
void profileDetails() {
  int startY = 10;
  tft.fillScreen(BLACK);
  tft.setCursor(0, startY);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("Welcome back");
  tft.setCursor(0, startY+15);
  tft.println(displayString[1]);
  tft.setCursor(0, startY+30);
  tft.println("Here is your profile details:");
  delay(2000);

  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  int counter = 2;
  int pos = startY;
  for (counter = 2; counter < 10; counter++)
  {
    tft.setCursor(0, pos);
    if ( !(counter % 2))
    {
      tft.setTextColor(YELLOW);
    }
    else
    {
      tft.setTextColor(WHITE);
    }
    tft.println(displayString[counter]);
    pos += 10;
  }
}

void printLine(String str) {
  tft.setCursor(0, 110);
  tft.setTextColor(BLACK);
  tft.println(bstr);
  tft.setCursor(0, 110);
  tft.setTextColor(RED);
  tft.println(str);
  bstr = str;
}
