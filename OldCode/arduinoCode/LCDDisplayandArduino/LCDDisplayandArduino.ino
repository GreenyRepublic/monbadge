

// You can use any (4 or) 5 pins
#define sclk 2
#define mosi 3
#define dc   4
#define cs   5
#define rst  6

// Color definitions
#define BLACK           0x0000
#define RED             0xF800
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define GRAY            0xBDF7

#include <FileIO.h>
#include <avr/wdt.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>


#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>


#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);

FPS_GT511C3 fps(8, 9);


PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);



String bstr = "Waiting for a card";
boolean enrolled = false;

void setup() {
  tft.begin();

  //uint16_t time = millis();
  tft.fillRect(0, 0, 128, 128, BLACK);
  //time = millis() - time;

  fps.Open();
  delay(100);
  fps.SetLED(true);

  startupscreen();

  profileDetails();


  /*while (enrolled == false) {
    FPSenroll();
    delay(1000);
  }*/

  
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    printLine("Didn't find NFC board");
    while (1); // halt
  }
  nfc.setPassiveActivationRetries(0xFF);

  nfc.SAMConfig();
  printLine("Waiting for a card");

  readNFC();
  

}

void loop() {

}

void startupscreen() {
  tft.fillScreen(BLACK);
  tft.setCursor(40, 70);
  tft.setTextSize(3);
  tft.setTextColor(GRAY);
  tft.println("MON");
  delay(3000);
}

void profileDetails() {
  Bridge.begin();
  FileSystem.begin();
  String dataString;
  String displayString[12];
  dataString = "";
  int i = 0;
  File dataFile = FileSystem.open("/mnt/sd/profile.txt", FILE_READ);
  char readchar = (char)dataFile.read();
  while (readchar != EOF){
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

  if (displayString[10] != "")
  {
    enrolled = true;
  }

  if (enrolled == false){
    tft.fillScreen(BLACK);
    tft.setCursor(0, 40);
    tft.setTextColor(YELLOW);
    tft.setTextSize(1);
    tft.println("Your profile haven't");
    tft.setCursor(0, 55);
    tft.println("enrolled a fingerprint.");
    tft.setCursor(0, 70);
    tft.println("Please enroll now!");

    while (enrolled == false) {
      FPSenroll();

    }
  }

  if (enrolled == true){
    bool verified = false;
    tft.fillScreen(BLACK);
    tft.setCursor(0, 40);
    tft.setTextColor(YELLOW);
    tft.setTextSize(1);
    tft.println("Your profile is");
    tft.setCursor(0, 55);
    tft.println("protected");
    tft.setCursor(0, 70);
    tft.println("Press your finger to");
    tft.setCursor(0, 85);
    tft.println("unlock it.");
    while(verified == false)
    {
      if (fps.IsPressFinger())
      {
        fps.CaptureFinger(false);
        int id = fps.Identify1_N();
        if (id == displayString[11].toInt())
        {
          printLine("Finger found");
          verified = true;
        }
        else
        {
          printLine("Finger not found");
        }
      }
      else
      {
        printLine("Please press finger");
      }
      delay(1000);

    }
  }

  tft.fillScreen(BLACK);
  tft.setCursor(0, 40);
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("Welcome back");
  tft.setCursor(0, 55);
  tft.println(displayString[1]);
  tft.setCursor(0, 70);
  tft.println("Here is your profile details:");
  delay(2000);

  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  int pos = 40;
  int counter = 2;
  for(counter = 2; counter<10; counter++)
  {
    tft.setCursor(0, pos);
    if ( counter % 2 == 0)
    {
      tft.setTextColor(YELLOW);
    }
    else
    {
      tft.setTextColor(WHITE);
    }
    tft.println(displayString[counter]);
    pos = pos + 10;
  }
  /*
  tft.setCursor(0,40);
  tft.setTextColor(YELLOW);
  tft.println(displayString[2]);
  tft.setCursor(0, 50);
  tft.setTextColor(WHITE);
  tft.println(displayString[3]);
  tft.setCursor(0, 60);
  tft.setTextColor(YELLOW);
  tft.println(displayString[4]);
  tft.setCursor(0, 70);
  tft.setTextColor(WHITE);
  tft.println(displayString[5]);
  tft.setCursor(0, 80);
  tft.setTextColor(YELLOW);
  tft.println(displayString[6]);
  tft.setCursor(0, 90);
  tft.setTextColor(WHITE);
  tft.println(displayString[7]);
  tft.setCursor(0, 100);
  tft.setTextColor(YELLOW);
  tft.println(displayString[8]);
  tft.setCursor(0, 110);
  tft.setTextColor(WHITE);
  tft.println(displayString[9]);
  */
  delay(3000);
}

void FPSenroll() {
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid == true) enrollid++;
  }
  fps.EnrollStart(enrollid);

  // enroll
  String str1 = "Press fingerE #";
  str1 += enrollid;
  printLine(str1);
  while (fps.IsPressFinger() == false)
  {
    delay(100);
  }
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    printLine("Remove finger");
    fps.Enroll1();
    while (fps.IsPressFinger() == true) delay(100);
    printLine("Press same finger 2");
    while (fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      printLine("Remove finger");
      fps.Enroll2();
      while (fps.IsPressFinger() == true) delay(100);
      printLine("Press same finger 3");
      while (fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        printLine("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0)
        {
          printLine("Enrolling Successfull");
          enrolled = true;

          FileSystem.begin();
          File dataFile = FileSystem.open("/mnt/sd/profile.txt", FILE_APPEND);
          if (dataFile) {
            dataFile.println("ID:");
            dataFile.println(enrollid);
            dataFile.println("End");
            dataFile.close();
           }
          delay(1000);
          wdt_enable(WDTO_1S);
          wdt_reset();
        }
        else
        {
          String str2 = "Enroll error#";
          str2 += iret;
          printLine(str2);
        }
      }
      else
      {
        printLine("Failed 3rd finger");

      }
    }
    else
    {
      printLine("Failed 2nd finger");

    }
  }
  else
  {
    printLine("Failed 1st finger");

  }
}

void readNFC() {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  if (success) {
    printLine("Found a card!");
    // Wait 1 second before continuing

  }
  else
  {
    // PN532 probably timed out waiting for a card
    printLine("Timed out waiting for a card");
  }
  delay(1000);
  readNFC();
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
