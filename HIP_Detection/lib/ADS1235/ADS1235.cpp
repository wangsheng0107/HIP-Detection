/***************************************************
  This is a library for the TI ADS1235 ADC


  This lib use the SPI interface of the ADC

  Written by Scott Downen
 ****************************************************/

#include <SPI.h>
#include <Arduino.h>
#include <Wire.h>
#include "ADS1235.hpp"
//#include "C:\Workspace\COVID\Photodetection\COVIDTest\COVIDTest.h"
//#include "C:\Workspace\COVID\Photodetection\COVIDTest\board.h"


// ADS1235::ADS1235(int ss_pin) {
//   SSPin = ss_pin;
// }

//const int SSPin = 12; //4 th pin from top right to bottom right

bool ADS1235::begin(void) {

  // // initialize SPI:
  Serial.print("Port Initialized");
  SPI.begin();
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE1));
  // Nano 33 BLE use nordic nRF52840, max 8MHz SPI
  // SPI.setDataMode(SPI_MODE1);
  // SPI.setClockDivider(SPI_CLOCK_DIV16);
  delay(2);
  pinMode (SSPin, OUTPUT);
  delay(2);
  Serial.print("Chip Select assigned to pin\t");
  Serial.println(SSPin);
  delay(2);

  Reset(); // Reset MCU on startup
  //Unlock(); // Unlock registers (this is default, but reduntant check)
  DevID(); // Verify communications and device ID
  Status(); // Verify device status; Reset SPI bus as needed
  Mode0(); // Set up sampling speed and filters
  Mode1(); // Set up start-up delay
  Mode2(); // Set up GPIOs
  Mode3(); // Power down/Status/Error checking, off by default
  SetupADC(); // Set up ADC voltage reference
  Gain(); // Set up software gain
  MUX(); // Select Analog Input to check
  OFFCal(0xfffffe); //Set offset to avoid negative reading
  StartADC(); // Begin ADC reading.
  delay(100); // Give ADC time to settle

  return true;
}

int ADS1235::Write(int input) {
  digitalWrite(SSPin,LOW);
  SPI.transfer(input);
  int echo = SPI.transfer(0x00);
  digitalWrite(SSPin,HIGH);
  delayMicroseconds(50);
  return echo;
}

void ADS1235::Nop(void) {
  Write(0X00);
}

void ADS1235::Reset(void) {
  Write(ADS1235_RESET);
  Serial.println("Device Reset. \t");
}

void ADS1235::StartADC(void) {
  Serial.println("Starting ADC...");
  Write(ADS1235_START);
}

void ADS1235::StopADC(void) {
  Serial.println("Stopping ADC...");
  Write(ADS1235_START);
}

unsigned long ADS1235::RData(void) { 
  int safeExit = 0;
  digitalWrite(SSPin,LOW);
  SPI.transfer(ADS1235_RDATA);
  byte echo = SPI.transfer(0x00); // Used to confirm the communications with register. 0x12 return = valid.
  byte MSB = SPI.transfer(0x00);
  byte MID = SPI.transfer(0x00);
  byte LSB = SPI.transfer(0x00);
  digitalWrite(SSPin, HIGH);
  delayMicroseconds(50);
  unsigned long converted_data = (MSB<<8) |  MID;
  converted_data <<= 8;
  converted_data |= LSB;

  if (echo != ADS1235_RDATA){
    Serial.println("\nSPI did not receive echo. Check connections & registry value");
    Serial.print("Echo data read: ");
    Serial.println(echo);
    return safeExit;
  }

  else{
  Serial.println("Conversion data read.");
  Serial.print("MSB, 0x");
  Serial.println(MSB,HEX);
  Serial.print("MID, 0x");
  Serial.println(MID,HEX);
  Serial.print("LSB, 0x");
  Serial.println(LSB,HEX);
  delay(100);

  return converted_data;
  }
}

void ADS1235::SYOCal(void) {
  Serial.print("Performing system offset calibration...");
  Write(ADS1235_SYOCAL);
  Serial.println("System offset calibration complete.");
}

void ADS1235::GANCal(void) {
  Serial.print("Performing system gain calibration...");
  Write(ADS1235_GANCAL);
  Serial.println("System gain calibration complete.");
}

void ADS1235::SFOCal(void) {
  Serial.print("Performing self offset calibration...");
  Write(ADS1235_SFOCAL);
  Serial.println("System self offset complete.");
}

//16773215 to add 4000 to final read. 2^24 = 16777215
void ADS1235::OFFCal(int offsetvalue){
  Serial.print("Configuring offset calibration... Set to: ");
  Serial.println("offsetvalue");
  byte OFF_MSB = offsetvalue>>16;
  byte OFF_MID = offsetvalue>>8 & 0xFF;
  byte OFF_LSB = offsetvalue & 0xFF;
  Serial.print("Performing self offset calibration...");
  
  digitalWrite(SSPin,LOW);
  byte SFOREG = ADS1235_OFCAL0 + 0X40;
  SPI.transfer(SFOREG);
  SPI.transfer(OFF_MSB);
  digitalWrite(SSPin,HIGH);
  digitalWrite(SSPin,LOW);
  SPI.transfer(SFOREG+1);
  SPI.transfer(OFF_MID);
  digitalWrite(SSPin,HIGH); 
  digitalWrite(SSPin,LOW);
  SPI.transfer(SFOREG+2);
  SPI.transfer(OFF_LSB);
  digitalWrite(SSPin,HIGH); 
  // SPI.transfer(OFF_MID);
  // SPI.transfer(OFF_LSB);
  // digitalWrite(SSPin,HIGH);
  Serial.println("offset config done");
  delay(1000);
  RReg(0x07);
  RReg(0x08);
  RReg(0x09);
};


int ADS1235::RReg(int rrh) {
  byte regID = rrh + ADS1235_RREG;
  digitalWrite(SSPin,LOW);
  SPI.transfer(regID);
  SPI.transfer(0x00);
  byte regValue = SPI.transfer(0x00);
  digitalWrite(SSPin,HIGH);
  Serial.print("Reg value at 0x");
  Serial.print(rrh, HEX);
  Serial.print(": ");          
  Serial.println(regValue, HEX);
  return regValue;
}

void ADS1235::WReg(int rrh, int value) {
  byte regID = rrh + ADS1235_WREG;
  digitalWrite(SSPin,LOW);
  Serial.print("Writing to register 0x");
  Serial.print(regID, HEX);
  Serial.println("...");
  SPI.transfer(regID);
  SPI.transfer(value);
  digitalWrite(SSPin,HIGH);
  delay(1000);
}

void ADS1235::Lock(void){
  Serial.print("Locking registers...");
  digitalWrite(SSPin,LOW);
  Write(ADS1235_LOCK);
  digitalWrite(SSPin,HIGH);
  delay(1000);
  Serial.println("Registers locked.");
}

void ADS1235::Unlock(void){
  Serial.print("Unlocking registers...");
  digitalWrite(SSPin,LOW);
  Write(ADS1235_UNLOCK);
  digitalWrite(SSPin,HIGH);
  delay(5);
  Serial.println("Registers unlocked.");
}

void ADS1235::DevID(void) {
  Serial.print("Device ID: ");
  digitalWrite(SSPin,LOW);
  RReg(ADS1235_ID);
  digitalWrite(SSPin,HIGH);
  Serial.print("\n");
  delay(5);
}

void ADS1235::Status(void){
  byte DevStatus = RReg(ADS1235_STATUS);
  Serial.print("Dev Status: 0x");
  Serial.println(DevStatus, HEX);
  Serial.print("\n");

  if ((DevStatus = 0x1)){
    WReg(0x01, 0x00);
    Serial.println("Device status reset \n");
    delay(50);
    byte DevStatus = RReg(ADS1235_STATUS);
    Serial.print("Dev Status: 0x");
    Serial.println(DevStatus, HEX);
    Serial.print("\n");
  }
  
  delay(5);
}

void ADS1235::Mode0(void){
  /* Mode 0 sets up 60 samples/second and FIR filter=default*/
  Serial.print("Initializing Mode 0...");
 //oxB for 5sps sin4 for lowest noise at Gain = 1
  WReg(ADS1235_MODE0, 0xB);
  Serial.println("Conversion mode 0 set up.");
  delay(5);
}

void ADS1235::Mode1(void) {
  // Set up Mode 1
  /*Default 50us delay on conversion start, normal continuous mode*/
  Serial.print("Initializing Mode 1...");
  WReg(ADS1235_MODE1, 0x1);
  Serial.println("Conversion mode 1 set up.");
  delay(5);
}

void ADS1235::Mode2(void){
  // Set up Mode 2
  /*GPIO not connected internally, only using AIN*/ 
  Serial.print("Initializing Mode 2...");
  WReg(ADS1235_MODE2, 0x0);
  Serial.println("Conversion mode 2 set up.");
  delay(5);
}

void ADS1235::Mode3(void) {
  // Set up Mode 3
  /*No SW power down, no status byte, no crc data verification, no SPI auto-reset*/ 
  Serial.print("Initializing Mode 3...");
  WReg(ADS1235_MODE3, 0x0);
  Serial.println("Conversion mode 3 set up.");
  delay(5);
}

void ADS1235::SetupADC(void) {
  // Set up ADC reference
  /*AVDD and AVSS are default pos/neg inputs, 0b00000101*/
  Serial.print("Setting up ADC references...");
  WReg(ADS1235_REF,0x0A);
  Serial.println("ADC reference set up.");
  delay(5);
}

void ADS1235::Gain(void) {
  // Set up Gain register
  /*0x1 = 1, 0x6 = 64, 0x7 = 128*/
  /*0x0 = 1, 0x6 = 64, 0x7 = 128*///Sheng
  //0x80 PGA bypass
  Serial.print("Setting Gain...");
  WReg(ADS1235_PGA,0x80);
  Serial.println("Gain set.");
  delay(5);
}

void ADS1235::MUX(void) {
  //AIN0 and AIN1 = MUX 0x34 (table 41). Set INPMUX to 0x34.
  Serial.print("Selecting AIN0 as pos, AIN1 as neg ref...");
  WReg(ADS1235_INPMUX,ADC0_ADC1_COMP);
  Serial.println("Pins selected.");
  delay(5);
}
