//
// main.cpp
// Arduino Led Matrix Code
//
// Created by Mathias de Cacqueray-Valmenier on 30/11/2018.
// Copyright (c) 2018 Mathias de Cacqueray-Valmenier. All rights reserved.
// Repository created by Mathias de Cacqueray-Valmenier, You should read LICENSE

#include <Arduino.h>
#include <letterList.h>

const uint64_t MAXCHARLEN = 512;
const char textDisplay[MAXCHARLEN] = "TRUC DE DINGUE \0";

// DEBUG FLAGS !!!
// #define DEBUG_TEXT 0
// #define DISPLAY_REGISTERS_ROWS 0


const uint8_t NB_MATRIX = 1;
struct tablePinsRow {uint8_t row[NB_ROWS];	uint8_t col[NB_COLS];};
const tablePinsRow matrixDef[NB_MATRIX] = {
	{
		{A0, A1, A2, A3, A4, A5, A6, A7, A8, A9}, 
		{49, 48, A15, A14, A13, A12, A11, A10}
	},
};
byte textDisplayBytes[MAXCHARLEN][NB_ROWS] = {B00000001};
uint64_t textDisplayLen = 0;

uint32_t index = 0;
// array def of trigger hc595, 
struct hc595def {
	uint8_t serPin;	
	uint8_t sckPin;	
	uint8_t rckPin;
	uint8_t lsbPin;
	uint8_t nbOut; // nbOut = number of output used (starting at 0)
};
struct hc259def {
	uint8_t a0Pin;	
	uint8_t a1Pin;	
	uint8_t a2Pin; 
	uint8_t nbOut;  // nbOut = number of output used (a0, a1, a2)
};
const uint8_t NB_OUTPUT_ADDR = 8;
const uint8_t NB_INPUT_ADDR = 3;
const uint8_t matchAdress[NB_OUTPUT_ADDR][NB_INPUT_ADDR] = {
	{LOW, LOW, LOW},
	{LOW, LOW, HIGH},
	{LOW, HIGH, LOW},
	{LOW, HIGH, HIGH},
	{HIGH, LOW, LOW},
	{HIGH, LOW, HIGH},
	{HIGH, HIGH, LOW},
	{HIGH, HIGH, HIGH}
};
// TODO : complete this with the actual pin connected on the arduino
const hc595def hc595cmd = {10, 8, 9, LSBFIRST, NB_COLS};
const int NB_HC259 = 1;
const hc259def hc259cmd[NB_HC259] = {
	{26, 24, 22, 7},
};
const unsigned long period = 300;

unsigned long startMillis;
unsigned long currentMillis;
uint8_t indRow = 0;
uint8_t indCol = 0;
uint8_t indLeters = 0;
uint8_t indLed = 0;
uint8_t indReg = 0;

void select259addr(uint8_t rowInd, hc259def thisH259def) {
	#ifdef DISPLAY_REGISTERS_ROWS
		Serial.print("Select : row: ");
		Serial.print(rowInd);
		Serial.print(" rowID : ");
		Serial.print((matchAdress[rowInd][0] ? "O" : "X"));
		Serial.print((matchAdress[rowInd][1] ? "O" : "X"));
		Serial.print((matchAdress[rowInd][2] ? "O" : "X"));
		Serial.println("");
	#endif // DISPLAY_REGISTERS_ROWS //
	for (indReg = 0; indReg < NB_INPUT_ADDR ; indReg++ ) {
		switch (indReg) {
			case 0: digitalWrite(thisH259def.a0Pin, matchAdress[rowInd][0]); break;
			case 1: digitalWrite(thisH259def.a1Pin, matchAdress[rowInd][1]); break;
			case 2: digitalWrite(thisH259def.a2Pin, matchAdress[rowInd][2]); break;
		}
	}
}

void write595byte(byte data, hc595def thisHC595def) {
	shiftOut(thisHC595def.serPin, thisHC595def.sckPin, thisHC595def.lsbPin, data);
}
void write595bytes(byte data[NB_COLS], hc595def thisHC595def) {
	digitalWrite(thisHC595def.rckPin, LOW);
	// This works only for LSBFIRST
	// https://www.arduino.cc/reference/en/language/functions/advanced-io/shiftout/
	for (size_t index = 0; index < NB_COLS; index++) {
		write595byte(data[index], thisHC595def);
	}
	digitalWrite(thisHC595def.rckPin, HIGH);
}

void displayData(uint8_t dataToWrite[NB_COLS][NB_ROWS])
{ 
	// Turn on each row in series
	for (indRow = 0; indRow < NB_ROWS; indRow++)
	{
		// initiate row with HIGH
		select259addr(indRow, hc259cmd[0]);
		// write whole line
		write595bytes(dataToWrite[indRow], hc595cmd);
		// delay to be able to see it
		delayMicroseconds(100);
	}
}

void moveText() {
	// Serial.println("Test");
	currentMillis = millis();
	if (currentMillis - startMillis > period)
	{
		for (uint8_t indRow = 0; indRow < NB_ROWS; indRow++) {
			byte lastChar = B00000000;
			for (uint32_t indChar = 0; indChar < textDisplayLen; indChar++) {
				byte bit = (textDisplayBytes[indChar][indRow] >> 7);
				textDisplayBytes[indChar][indRow] = textDisplayBytes[indChar][indRow] << 1;
				if (indChar == 0) {
					lastChar = bit;
				} else if (indChar > 0) {
					textDisplayBytes[indChar - 1][indRow] |= bit & B0000001;
				}
				if (indChar == textDisplayLen - 1) {
					textDisplayBytes[indChar][indRow] |= lastChar & B0000001;
				}
			}
		}
		startMillis = millis();
	}
}

void setup() 
{
	// compute size of string to display
	textDisplayLen = 0;
	while (textDisplay[textDisplayLen] != '\0' && textDisplayLen < MAXCHARLEN) {
		textDisplayLen++;
	}
	// assign bytes letter to each char letter
	for (index = 0; index < textDisplayLen; index++) {
		cvtCharByte(textDisplay[index], textDisplayBytes[index]);
	}
	// Open serial port
	Serial.begin(115200);
	memcpy(customArray, textDisplayBytes[0], sizeof(EMPTY));
	startMillis = millis();  //initial start time
}

void displayDebugText(byte textDisplayBytes[MAXCHARLEN][NB_ROWS]) {
	Serial.println("Start Text");
	for (uint8_t indRow = 0; indRow < NB_ROWS; indRow++) {
		for (uint32_t indChar = 0; indChar < NB_BYTE_DISPLAY; indChar++) {
			for (index = 0; index < 8; index++) {
				bool result = textDisplayBytes[indChar][indRow] << index & B1000000;
				if (result) Serial.print("0");
				else Serial.print(" ");
			}
		}
		Serial.println();
	}
	Serial.println("End Text");
}
const unsigned long periodDebug = 1000;
unsigned long startMillisDebug;
unsigned long currentMillisDebug;

void loop() {
	#ifdef DEBUG_TEXT
	currentMillisDebug = millis();
	if (currentMillisDebug - startMillisDebug > periodDebug) {
		displayDebugText(textDisplayBytes);
		startMillisDebug = millis();
	}
	#endif // DEBUG_TEXT //
	moveText();
	displayData(textDisplayBytes);
}