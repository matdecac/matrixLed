//
// main.cpp
// Arduino Led Matrix Code
//
// Created by Mathias de Cacqueray-Valmenier on 30/11/2018.
// Copyright (c) 2018 Mathias de Cacqueray-Valmenier. All rights reserved.
// Repository created by Mathias de Cacqueray-Valmenier, You should read LICENSE

#include <Arduino.h>
#include <letterList.h>

const uint8_t NB_MATRIX = 1;


struct tablePinsRow {uint8_t row[NB_ROWS];	uint8_t col[NB_COLS];};
const tablePinsRow matrixDef[NB_MATRIX] = {
	{
		{A0, A1, A2, A3, A4, A5, A6, A7, A8, A9}, 
		{49, 48, A15, A14, A13, A12, A11, A10}
	},
};

// array def of trigger hc595, 
struct hc595def {
	uint8_t serPin;	
	uint8_t sckPin;	
	uint8_t lsbPin; 
	uint8_t rckPin; 
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
const hc595def hc595cmd[16] = {
	{0, 0, 0, 0, 4}, {0, 0, 0, 0, 8}, {0, 0, 0, 0, 8}, // petit cote n1
	{0, 0, 0, 0, 7}, {0, 0, 0, 0, 8}, {0, 0, 0, 0, 8}, {0, 0, 0, 0, 8}, {0, 0, 0, 0, 8}, // grand cote n1
	{0, 0, 0, 0, 4}, {0, 0, 0, 0, 8}, {0, 0, 0, 0, 8}, // petit cote n2
	{0, 0, 0, 0, 7}, {0, 0, 0, 0, 8}, {0, 0, 0, 0, 8}, {0, 0, 0, 0, 8}, {0, 0, 0, 0, 8}, // grand cote n2
};
const hc259def hc259cmd[16] = {
	{0, 0, 0, 8}, {0, 0, 0, 4}, // petit cote n1
	{0, 0, 0, 8}, {0, 0, 0, 4}, // grand cote n1
	{0, 0, 0, 8}, {0, 0, 0, 4}, // petit cote n2
	{0, 0, 0, 8}, {0, 0, 0, 4}, // grand cote n2
};

void write595(byte data, hc595def thisHC595def) {
	digitalWrite(thisHC595def.rckPin, LOW);
	shiftOut(thisHC595def.serPin, thisHC595def.sckPin, thisHC595def.lsbPin, data);
	digitalWrite(thisHC595def.rckPin, HIGH);
}

void fixedLetter2(uint8_t matrixNb, uint8_t dataToWrite[NB_ROWS])
{ 
	// Turn on each row in series
	for (byte indRow = 0; indRow < NB_ROWS; indRow++)
	{
		// initiate row with HIGH
		for (uint8_t indLed = 0; indLed < NB_OUTPUT_ADDR ; indLed++ ) {
			for (uint8_t indReg = 0; indReg < NB_INPUT_ADDR ; indReg++ ) {
				digitalWrite(matrixDef[matrixNb].row[indReg], matchAdress[indLed][indReg]);  
			}
		}
		for (uint8_t indBit = 0; indBit < NB_ROWS; indBit++)
		{
			// only write if bit 1 asked
			if ((dataToWrite[indRow] >> indBit) & 0x01) {
				// loop on all accessible output
				for (uint8_t indLed = 0; indLed < NB_OUTPUT_ADDR ; indLed++ ) {
					// loop on register path
					for (uint8_t indReg = 0; indReg < NB_INPUT_ADDR ; indReg++ ) {
						digitalWrite(matrixDef[matrixNb].col[indReg], matchAdress[indLed][indReg]);  
					}
				}
			}
			delayMicroseconds(100);  // uncoment delay for diferent speed of display
		}
		// END row with LOW ==> HOW TO DO IT !!!
		for (uint8_t indLed = 0; indLed < NB_OUTPUT_ADDR ; indLed++ ) {
			for (uint8_t indReg = 0; indReg < NB_INPUT_ADDR ; indReg++ ) {
				digitalWrite(matrixDef[matrixNb].row[indReg], matchAdress[indLed][indReg]);  
			}
		}
	}
}


void resetMatrix(uint8_t iMat, bool isInit) {
	// Set all used pins to OUTPUT
	for (uint8_t iMat=0; iMat < NB_MATRIX; iMat++) {
		for (uint8_t iRow=0; iRow < NB_ROWS; iRow++) {
			if (isInit) pinMode(matrixDef[iMat].row[iRow], OUTPUT);
			digitalWrite(matrixDef[iMat].row[iRow], LOW); 
		}
		for (uint8_t iCol=0; iCol < NB_COLS; iCol++) {
			if (isInit) pinMode(matrixDef[iMat].col[iCol], OUTPUT);
			digitalWrite(matrixDef[iMat].col[iCol], LOW); 
		}	
	}
}

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 300;



void fixedLetter(uint8_t matrixNb, const uint8_t dataToWrite[NB_ROWS])
{ 
	// Turn on each row in series
	for (byte indRow = 0; indRow < NB_ROWS; indRow++)
	{
		// initiate row with HIGH
		digitalWrite(
			matrixDef[matrixNb].row[indRow], LOW
		);
		// loop on all accessible output
		for (uint8_t indCol = 0; indCol < NB_COLS ; indCol++ ) {
			// only write if bit 1 asked
			// loop on register path
			digitalWrite(
				matrixDef[matrixNb].col[indCol],
				(dataToWrite[indRow] >> indCol) & 0x01
			);
		}
		delayMicroseconds(100);
		digitalWrite(
			matrixDef[matrixNb].row[indRow], HIGH
		);
		// loop on all accessible output
		for (uint8_t indCol = 0; indCol < NB_COLS ; indCol++ ) {
			// only write if bit 1 asked
			// loop on register path
			digitalWrite(
				matrixDef[matrixNb].col[indCol], LOW
			);
		}
	}
}
uint8_t indRow = 0;
uint8_t indCol = 0;
uint8_t indLeters = 0;

void testLoop() {
	// Serial.println("Test");
	currentMillis = millis();
	if (currentMillis - startMillis > period)
	{
		// resetMatrix(0, false);
		if (indCol >= NB_COLS) {
			indRow += 1;
			indCol = 0;
		}
		else indCol += 1;
		if (indRow >= NB_ROWS) {
			indRow = 0;	
		}
		memcpy(customArray, EMPTY, sizeof(EMPTY));	
		customArray[indRow] |= 1UL << indCol;
		startMillis = millis();
	}
	fixedLetter(0, customArray);
}
void testLeters() {
	// Serial.println("Test");
	currentMillis = millis();
	if (currentMillis - startMillis > period)
	{
		// resetMatrix(0, false);
		if (indLeters >= NB_LETERS) {
			indLeters = 0;
		} else {
			indLeters++;
		}
		startMillis = millis();
	}
	fixedLetter(0, ALL_LETERS[indLeters]);
}
void movingLeter() {
	// Serial.println("Test");
	currentMillis = millis();
	if (currentMillis - startMillis > period)
	{
		// resetMatrix(0, false);
		if (indCol >= NB_COLS) {
			indCol = 0;
		} else {
			indCol += 1;
		}
		for (byte indRow = 0; indRow < NB_ROWS; indRow++) {
			uint8_t bit = (customArray[indRow] >> 7);
			customArray[indRow] = customArray[indRow] << 1;
			customArray[indRow] |= bit & B0000001;
		}
		startMillis = millis();
	}
	fixedLetter(0, customArray);
}

const uint64_t MAXCHARLEN = 512;
const char textDisplay[MAXCHARLEN] = "TRUC DE DINGUE\0";
byte textDisplayBytes[MAXCHARLEN][NB_ROWS] = {B00000001};
uint64_t textDisplayLen = 0;
byte matrixDisplay[NB_MATRIX][NB_ROWS] = {B00000001};


void displayText() {
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
	// assign byte to each letter
	for (uint32_t ind; ind < textDisplayLen; ind++) {
		cvtCharByte(textDisplay[ind], textDisplayBytes[ind]);
	}
	// Open serial port
	// Serial.begin(9600);
	// textDisplay
	// Set all used pins to OUTPUT
	for (uint8_t iMat=0; iMat < NB_MATRIX; iMat++) {
		resetMatrix(iMat, true);
	}
	memcpy(customArray, textDisplayBytes[0], sizeof(EMPTY));
	startMillis = millis();  //initial start time
}
void loop() {
	// fixedLetter(0, textDisplayBytes[0]);
	// movingLeter();
	displayText();
	fixedLetter(0, textDisplayBytes[0]);
}