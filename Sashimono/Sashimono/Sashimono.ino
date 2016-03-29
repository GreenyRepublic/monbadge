/*
 Name:		Sashimono
 Created:	3/3/2016 1:44:42 AM
 Author(s):	Benjamin Clark, Hino Hau.
 Description: The basic operating software for the MON smart identity badge.
 Version 0.0.1
*/

//NFC Module Libraries
#include <PN532.h>
#include <PN532_debug.h>
#include <PN532Interface.h>
#include <PN532_SPI.h>
#include <PN532_I2C.h>
#include <PN532_HSU.h>
#include <snep.h>
#include <mac_link.h>
#include <llcp.h>
#include <emulatetag.h>
#include <NfcTag.h>
#include <NfcDriver.h>
#include <NfcAdapter.h>
#include <NdefRecord.h>
#include <NdefMessage.h>
#include <Ndef.h>
#include <MifareUltralight.h>
#include <MifareClassic.h>
#include <Due.h>

//Other Libraries
#include <Wire.h>
#include <FPS_GT511C3.h>		//Fingerprint Scanner Library
#include <gfxfont.h>			//GFX Fonts Library
#include <Adafruit_GFX.h>		//GFX Library
#include <Adafruit_SSD1351.h>	//Display Library

//Header Pins Definitions
#define SCLK 2
#define MOSI 3
#define DC   4
#define CS   5
#define RST  6

//Color Definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF
#define GRAY            0xBDF7
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

// Option 1: use any pins but a little slower
Adafruit_SSD1351 tft = Adafruit_SSD1351(CS, DC, MOSI, SCLK, RST);

// Option 2: must use the hardware SPI pins 
// (for UNO thats SCLK = 13 and SID = 11) and pin 10 must be 
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, rst);

float Pi = 3.1415926;
int cursory = 0;

void fillpixelbypixel(uint16_t color)
{
	for (uint8_t x = 0; x < tft.width(); x++)
	{
		for (uint8_t y = 0; y < tft.height(); y++)
		{
			tft.drawPixel(x, y, color);
		}
	}
	delay(100);
}

//Initial power-on and reset button code.
void setup()
{
	//Show MON Splash Screen.
	Serial.begin(115200);
	tft.begin();

	// You can optionally rotate the display by running the line below.
	// Note that a value of 0 means no rotation, 1 means 90 clockwise,
	// 2 means 180 degrees clockwise, and 3 means 270 degrees clockwise.
	//tft.setRotation(1);
	// NOTE: The test pattern at the start will NOT be rotated!  The code
	// for rendering the test pattern talks directly to the display and
	// ignores any rotation.

	uint16_t time = millis();
	tft.fillRect(0, 0, 128, 128, BLACK);
	time = millis() - time;

	Serial.println(time, DEC);
	delay(500);

	startupscreen();

	profileDetails();

	NFCstartup();

	readNFC();
	delay(500);

	Serial.println("done");
	delay(1000);
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
	tft.fillScreen(BLACK);
	tft.setCursor(0, 40);
	tft.setTextColor(YELLOW);
	tft.setTextSize(1);
	tft.println("Welcome back");
	tft.setCursor(0, 55);
	tft.println("hinohau");
	tft.setCursor(0, 70);
	tft.println("Here is your profile details:");
	delay(2000);

	tft.fillScreen(BLACK);
	tft.setTextSize(1);
	tft.setCursor(0, 40);
	tft.setTextColor(YELLOW);
	tft.println("Name:");
	tft.setCursor(0, 50);
	tft.setTextColor(WHITE);
	tft.println("Kwok Hin Hau");
	tft.setCursor(0, 60);
	tft.setTextColor(YELLOW);
	tft.println("Status:");
	tft.setCursor(0, 70);
	tft.setTextColor(WHITE);
	tft.println("Available");
	tft.setCursor(0, 80);
	tft.setTextColor(YELLOW);
	tft.println("ID:");
	tft.setCursor(0, 90);
	tft.setTextColor(WHITE);
	tft.println("463473584");
	tft.setCursor(0, 100);
	tft.setTextColor(YELLOW);
	tft.println("Age:");
	tft.setCursor(0, 110);
	tft.setTextColor(WHITE);
	tft.println("20");
	delay(3000);
}

void NFCstartup() {
	Serial.println("Hello!");
	nfc.begin();
	uint32_t versiondata = nfc.getFirmwareVersion();
	if (!versiondata) {
		Serial.print("Didn't find PN53x board");
		while (1); // halt
	}

	// Got ok data, print it out!
	Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
	Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
	Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

	// Set the max number of retry attempts to read from a card
	// This prevents us from waiting forever for a card, which is
	// the default behaviour of the PN532.
	nfc.setPassiveActivationRetries(0xFF);

	// configure board to read RFID tags

	tft.setCursor(15, 110);
	tft.setTextColor(RED);
	tft.println("Waiting for a card");



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
		Serial.println("Found a card!");
		tft.setCursor(15, 110);
		tft.setTextColor(BLACK);
		tft.println("Waiting for a card");
		tft.setCursor(15, 110);
		tft.setTextColor(RED);
		tft.println("Found a card!");
		Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
		Serial.print("UID Value: ");
		for (uint8_t i = 0; i < uidLength; i++)
		{
			Serial.print(" 0x"); Serial.print(uid[i], HEX);
		}
		Serial.println("");
		// Wait 1 second before continuing
		exit;
	}
	else
	{
		// PN532 probably timed out waiting for a card
		Serial.println("Timed out waiting for a card");
		readNFC();
	}
}

}

// Main loop.
void loop() {
  
}
