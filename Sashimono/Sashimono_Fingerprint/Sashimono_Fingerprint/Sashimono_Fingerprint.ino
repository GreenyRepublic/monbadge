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
#include <SPI.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

//Hardware object declarations.
Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst);
FPS_GT511C3 fps(8, 9);

//Other globals.
String bstr = "Waiting for a card";
boolean enrolled = false;
String displayString[12];

void setup() {

	//Initialise the display.
	tft.begin();
	tft.fillRect(0, 0, 128, 128, BLACK);

	//Initialise the fingerprint scanner.
	fps.Open();
	delay(100);
	fps.SetLED(true);

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

	startupscreen();
	profileDetails();
}

void loop() {

}

//The splash screen displayed at startup.
void startupscreen() {
	tft.setCursor(40, 35);
	tft.setTextSize(3);
	tft.setTextColor(GRAY);
	tft.println("MON");
	delay(3000);
}

//Display the user's profile details.
void profileDetails() {

	//Starting Y position for lines of text written to the display.
	//Adjust to move text up/down.
	int startY = 10;

	//Space between lines of text.
	//Adjust to change line spacing.
	int spacing = 15;

	if (displayString[10] != "")
	{
		enrolled = true;
	}

	if (enrolled == false) {
		tft.fillScreen(BLACK);
		tft.setCursor(0, startY);
		tft.setTextColor(YELLOW);
		tft.setTextSize(1);
		tft.println("Your profile has not");
		tft.setCursor(0, startY + spacing);
		tft.println("enrolled a fingerprint.");
		tft.setCursor(0, startY + (spacing*2));
		tft.println("Please enroll now!");

		while (enrolled == false) {
			FPSenroll();
		}
	}

	if (enrolled == true) {
		bool verified = false;
		tft.fillScreen(BLACK);
		tft.setCursor(0, startY);
		tft.setTextColor(YELLOW);
		tft.setTextSize(1);
		tft.println("Your profile is");
		tft.setCursor(0, startY + spacing);
		tft.println("locked");
		tft.setCursor(0, startY + (spacing*2));
		tft.println("Press your finger to");
		tft.setCursor(0, startY + (spacing * 3));
		tft.println("unlock it.");
		while (!verified)
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
	tft.setCursor(0, startY);
	tft.setTextColor(YELLOW);
	tft.setTextSize(1);
	tft.println("Welcome Back!");
	tft.setCursor(0, startY + (spacing));
	tft.println(displayString[1]);
	tft.setCursor(0, startY + (spacing*2));
	tft.println("Here are your profile details:");
	delay(2000);

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
//Enroll a fingerprint using the scanner.
void FPSenroll() {
	int enrollid = 0;
	bool usedid = true;
	while (usedid == true)
	{
		usedid = fps.CheckEnrolled(enrollid);
		if (usedid == true) enrollid++;
	}
	fps.EnrollStart(enrollid);

	String str1 = "Press finger";
	str1 += enrollid;
	printLine(str1);
	while (fps.IsPressFinger() == false) delay(100);

	bool bret = fps.CaptureFinger(true);
	int iret = 0;
	if (bret != false)
	{
		printLine("Remove finger");
		fps.Enroll1();
		while (fps.IsPressFinger() == true) delay(100);
		printLine("Press same finger (2/3)");
		while (fps.IsPressFinger() == false) delay(100);
		bret = fps.CaptureFinger(true);
		if (bret)
		{
			printLine("Remove finger");
			fps.Enroll2();
			while (fps.IsPressFinger() == true) delay(100);
			printLine("Press same finger (3/3)");
			while (fps.IsPressFinger() == false) delay(100);
			bret = fps.CaptureFinger(true);
			if (bret)
			{
				printLine("Remove finger");
				iret = fps.Enroll3();
				if (!iret)
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
					String str2 = "Enrolment error";
					str2 += iret;
					printLine(str2);
				}
			}
			else
			{
				printLine("Failed third scan.");

			}
		}
		else
		{
			printLine("Failed second scan.");

		}
	}
	else
	{
		printLine("Failed first scan.");

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
