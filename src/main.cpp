//
// main.cpp
// Arduino Led Matrix Code
//
// Created by Mathias de Cacqueray-Valmenier on 30/11/2018.
// Copyright (c) 2018 Mathias de Cacqueray-Valmenier. All rights reserved.
// Repository created by Mathias de Cacqueray-Valmenier, You should read LICENSE


#include <Arduino.h>

const uint8_t NB_MATRIX = 1;
const uint8_t NB_ROWS = 8;
const uint8_t NB_ROWS_PINS = 3;
const uint8_t NB_COLS = 8;
const uint8_t NB_COLS_PINS = 3;

byte A[NB_ROWS] = {B00000000,B00111100,B01100110,B01100110,B01111110,B01100110,B01100110,B01100110};
byte B[NB_ROWS] = {B01111000,B01001000,B01001000,B01110000,B01001000,B01000100,B01000100,B01111100};
byte C[NB_ROWS] = {B00000000,B00011110,B00100000,B01000000,B01000000,B01000000,B00100000,B00011110};
byte D[NB_ROWS] = {B00000000,B00111000,B00100100,B00100010,B00100010,B00100100,B00111000,B00000000};
byte E[NB_ROWS] = {B00000000,B00111100,B00100000,B00111000,B00100000,B00100000,B00111100,B00000000};
byte F[NB_ROWS] = {B00000000,B00111100,B00100000,B00111000,B00100000,B00100000,B00100000,B00000000};
byte G[NB_ROWS] = {B00000000,B00111110,B00100000,B00100000,B00101110,B00100010,B00111110,B00000000};
byte H[NB_ROWS] = {B00000000,B00100100,B00100100,B00111100,B00100100,B00100100,B00100100,B00000000};
byte I[NB_ROWS] = {B00000000,B00111000,B00010000,B00010000,B00010000,B00010000,B00111000,B00000000};
byte J[NB_ROWS] = {B00000000,B00011100,B00001000,B00001000,B00001000,B00101000,B00111000,B00000000};
byte K[NB_ROWS] = {B00000000,B00100100,B00101000,B00110000,B00101000,B00100100,B00100100,B00000000};
byte L[NB_ROWS] = {B00000000,B00100000,B00100000,B00100000,B00100000,B00100000,B00111100,B00000000};
byte M[NB_ROWS] = {B00000000,B00000000,B01000100,B10101010,B10010010,B10000010,B10000010,B00000000};
byte N[NB_ROWS] = {B00000000,B00100010,B00110010,B00101010,B00100110,B00100010,B00000000,B00000000};
byte O[NB_ROWS] = {B00000000,B00111100,B01000010,B01000010,B01000010,B01000010,B00111100,B00000000};
byte P[NB_ROWS] = {B00000000,B00111000,B00100100,B00100100,B00111000,B00100000,B00100000,B00000000};
byte Q[NB_ROWS] = {B00000000,B00111100,B01000010,B01000010,B01000010,B01000110,B00111110,B00000001};
byte R[NB_ROWS] = {B00000000,B00111000,B00100100,B00100100,B00111000,B00100100,B00100100,B00000000};
byte S[NB_ROWS] = {B00000000,B00111100,B00100000,B00111100,B00000100,B00000100,B00111100,B00000000};
byte T[NB_ROWS] = {B00000000,B01111100,B00010000,B00010000,B00010000,B00010000,B00010000,B00000000};
byte U[NB_ROWS] = {B00000000,B01000010,B01000010,B01000010,B01000010,B00100100,B00011000,B00000000};
byte V[NB_ROWS] = {B00000000,B00100010,B00100010,B00100010,B00010100,B00010100,B00001000,B00000000};
byte W[NB_ROWS] = {B00000000,B10000010,B10010010,B01010100,B01010100,B00101000,B00000000,B00000000};
byte X[NB_ROWS] = {B00000000,B01000010,B00100100,B00011000,B00011000,B00100100,B01000010,B00000000};
byte Y[NB_ROWS] = {B00000000,B01000100,B00101000,B00010000,B00010000,B00010000,B00010000,B00000000};
byte Z[NB_ROWS] = {B00000000,B00111100,B00000100,B00001000,B00010000,B00100000,B00111100,B00000000};


struct tablePinsRow {
	uint8_t row[NB_ROWS_PINS];
	uint8_t col[NB_COLS_PINS];
};

const tablePinsRow matrixDef[NB_MATRIX] = {
	{{8, 9, 10}, {11, 12, 13}},
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

void setup() 
{
	// Open serial port
	// Serial.begin(9600);
	
	// Set all used pins to OUTPUT
	for (uint8_t iMat=0; iMat < NB_MATRIX; iMat++) {
		for (uint8_t iRow=0; iRow < NB_ROWS_PINS; iRow++) {
			pinMode(matrixDef[iMat].row[iRow], OUTPUT);
			digitalWrite(matrixDef[iMat].row[iRow], LOW); 
		}
		for (uint8_t iCol=0; iCol < NB_COLS_PINS; iCol++) {
			pinMode(matrixDef[iMat].col[iCol], OUTPUT);
			digitalWrite(matrixDef[iMat].col[iCol], LOW); 
		}	
	}
}


void fixedLetter(uint8_t matrixNb, uint8_t dataToWrite[NB_ROWS])
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
			delayMicroseconds(100);  // uncoment deley for diferent speed of display
		}
		// END row with LOW ==> HOW TO DO IT !!!
		for (uint8_t indLed = 0; indLed < NB_OUTPUT_ADDR ; indLed++ ) {
			for (uint8_t indReg = 0; indReg < NB_INPUT_ADDR ; indReg++ ) {
				digitalWrite(matrixDef[matrixNb].row[indReg], matchAdress[indLed][indReg]);  
			}
		}
	}
}

void loop() {
	delay(5);
	fixedLetter(0, A);
}