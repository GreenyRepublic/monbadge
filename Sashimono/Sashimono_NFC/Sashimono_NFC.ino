//The software for the MON badge Arduino prototype.
//Currently incorporates the display and fingerprint scanner, as well as reading user data from a file stored on a microSD card.
//Written by Kwok Hau and Benjamin Clark.
//22/04/2016

//Header pin defitions.
#define sclk 2
#define mosi 3
#define dc   4
#define cs   5
#define rst  6

//Color definitions.
#define BLACK           0x0000
#define RED             0xF800
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define GRAY            0xBDF7

//Library imports.
#include <FileIO.h>
#include <avr/wdt.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include "SoftwareSerial.h"
#include <SPI.h>

//NFC Imports.
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

//Hardware object declarations.
Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

//Other globals.
String bstr = "Waiting for a card";
boolean enrolled = true;
String displayString[12];

void setup() {

	Serial.begin(9600);
	delay(5000);

	//Initialise the display.
	tft.begin();
	tft.fillRect(0, 0, 128, 128, BLACK);
	startupscreen();

	//NFC board setup.
	//nfc.setPassiveActivationRetries(0xFF);
	//nfc.SAMConfig();

	//Extract text from the stored user data file and save it to an array.
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

void loop() {
	profileDetails();
	//scanCard();
}

//The splash screen displayed at startup.
void startupscreen() {
	tft.setCursor(40, 35);
	tft.setTextSize(3);
	tft.setTextColor(GRAY);
	tft.println("MON");
	delay(3000);
}

//Dispaly the initial welcome message.
void profileIntro()
{
	//Starting Y position for lines of text written to the display.
	//Adjust to move text up/down.
	int startY = 10;

	//Space between lines of text.
	//Adjust to change line spacing.
	int spacing = 15;

	tft.fillScreen(BLACK);
	tft.setCursor(0, startY);
	tft.setTextColor(YELLOW);
	tft.setTextSize(1);
	tft.println("Welcome Back!");
	tft.setCursor(0, startY + (spacing));
	tft.println(displayString[1]);
	tft.setCursor(0, startY + (spacing * 2));
	tft.println("Here are your profile details:");
	delay(2000);

}


//Display the user's profile details.
void profileDetails() {
	//Starting Y position for lines of text written to the display.
	//Adjust to move text up/down.
	int startY = 10;

	//Space between lines of text.
	//Adjust to change line spacing.
	int spacing = 15;

	tft.fillScreen(BLACK);
	tft.setTextSize(1);
	int pos = startY;
	for (int counter = 2; counter < 10; counter++)
	{
		switch (counter % 2)
		{
		case 0:
			tft.setTextColor(YELLOW);
			break;
		default:
			tft.setTextColor(WHITE);
			break;
		}
		tft.println(displayString[counter]);
		pos += 10;
	}
	delay(3000);
}

void scanCard()
{
	boolean success;
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
	uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

											  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
											  // 'uid' will be populated with the UID, and uidLength will indicate
											  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
	success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

	if (success) {
		Serial.println("Found a card!");
		Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
		Serial.print("UID Value: ");
		for (uint8_t i = 0; i < uidLength; i++)
		{
			Serial.print(" 0x"); Serial.print(uid[i], HEX);
		}
		Serial.println("");
		// Wait 1 second before continuing
		delay(1000);
	}
	else
	{
		// PN532 probably timed out waiting for a card
		Serial.println("Timed out waiting for a card");
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