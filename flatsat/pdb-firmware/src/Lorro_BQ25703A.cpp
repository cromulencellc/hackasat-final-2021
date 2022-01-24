/**************************************************************************/
/*!
  @file     Arduino_BQ25703A.cpp
  @author   Lorro


  Library for basic interfacing with BQ25703A battery management IC from TI
*/
/**************************************************************************/
#include "Arduino.h"

#include <Wire.h>

#include "Lorro_BQ25703A.h"

// Prints a binary number with leading zeros (Automatic Handling)
#define PRINTBIN(Num) for (uint32_t t = (1UL<< ((sizeof(Num)*8)-1)); t; t >>= 1) Serial2.write(Num  & t ? '1' : '0');

//Create instance of data structure
Lorro_BQ25703A::Regt BQ25703Areg;

Lorro_BQ25703A::Lorro_BQ25703A(){

  Wire.begin();

}

//I2C functions below here
//------------------------------------------------------------------------

static boolean Lorro_BQ25703A::readDataReg( const byte regAddress, byte *dataVal, const uint8_t arrLen ){

  Wire.beginTransmission( BQ25703Aaddr );
  Wire.write( regAddress );
  byte ack = Wire.endTransmission();
  if( ack == 0 ){
    Wire.requestFrom( ( int )BQ25703Aaddr , ( int )( arrLen + 1 ) );
    if( Wire.available() > 0 ){
      for( uint8_t i = 0; i < arrLen; i++ ){
        dataVal[ i ] = Wire.receive();
      }
    }
    return true;
  }else{
    return false; //if I2C comm fails
  }
}

static boolean Lorro_BQ25703A::writeDataReg( const byte regAddress, byte dataVal0, byte dataVal1 ){

  Wire.beginTransmission( BQ25703Aaddr );
  Wire.write( regAddress );
  Wire.write( dataVal0 );
  Wire.write( dataVal1 );
  byte ack = Wire.endTransmission();
  if( ack == 0 ){
    return true;
  }else{
    return false; //if I2C comm fails
  }

}

// JRL - Pull into BQ25703A library
// void readBQ25703ARegs() {
// 	uint8_t data = 0;
// 	char buff[50];

// 	int size = sizeof(BQ25703A.BQ25703ARegArray) / sizeof(BQ25703A.BQ25703ARegArray[0]);

// 	for (int j=0;j<size;j++) {
		
// 		Wire.beginTransmission(Lorro_BQ25703A::BQ25703Aaddr);
// 		Wire.write(BQ25703A.BQ25703ARegArray[j]);
// 		Wire.endTransmission();
// 		Wire.requestFrom(Lorro_BQ25703A::BQ25703Aaddr,(uint8_t)1);
	
// 		data = Wire.read();
	
// 		sprintf(buff, "Register %02X = %02X", BQ25703A.BQ25703ARegArray[j],data);
// 		Serial2.println(buff);

// 		delay(15);
		
// 	}

// }
