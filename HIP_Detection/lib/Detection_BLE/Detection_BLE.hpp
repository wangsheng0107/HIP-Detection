/***************************************************
  This is a library for the HIP Detection

  Use BLE to send & receive data and command from/to a ble master device
  Control DAC80501 and receive data from ADS1235

  Sheng Wang Dec 2021
 ****************************************************/

#ifndef Detection_BLE_H
#define Detection_BLE_H

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "ADS1235.hpp"
#include "DAC80501.hpp"

const int led_pin = LEDR;
int LaserWritten = 0;


BLEService DetectionServcie("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service, GUID generated by https://www.guidgenerator.com/online-guid-generator.aspx
// create switch characteristic and allow remote device to read and write
BLEByteCharacteristic switchCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
  // BLEDescriptor SwitchDescriptor("2901", "millis");
  // switchCharacteristic.addDescriptor(SwitchDescriptor);
  // Tried to give charactersitc a name, failed. Recognized as 'unknown characteristic'
// create byte characteristic and allow remote device to read and write
BLEIntCharacteristic LaserCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
  // BLEDescriptor LaserDescriptor("2902", "Laser");
  // LaserCharacteristic.addDescriptor(LaserDescriptor);
// create byte characteristic and allow remote device to get notifications
BLEIntCharacteristic PDCharacteristic("19B10013-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
  // BLEDescriptor RDATADescriptor("2903", "RDATA");
  // RDATACharacterstic.addDescriptor(RDATADescriptor);

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print("Characteristic event, written: ");

  if (switchCharacteristic.value()) {
    Serial.println("LED on");
    digitalWrite(led_pin,LOW);
  } else {
    Serial.println("LED off");
    digitalWrite(led_pin,HIGH);
  }
}

void LaserCharacteristicWrittten(BLEDevice central, BLECharacteristic characteristic){
  // central wrote new value to characteristic, update Laser intensity
  Serial.print("Characteristic event, written: ");
  LaserWritten = LaserCharacteristic.value();
  if (LaserWritten > 100){
    LaserWritten = 100;
  }
  else if(LaserWritten < 0){
    LaserWritten = 0;
  }
  else{
    LaserWritten = LaserWritten;
  }
    LaserCharacteristic.writeValue(LaserWritten);
    Serial.print("Laser level received and corrected to: ");
    Serial.println(LaserWritten);

}


class DetectionBLE  {
public: 
  DetectionBLE(ADS1235 & ADC, DAC80501 & DAC): _adc(ADC), _dac(DAC){};
  ~DetectionBLE(){};
public:
int begin(){
  if(!BLE.begin()){ 
    Serial.print("Starting BLE failed");
    while (1);
  }
  delay(10);
  
  delay(10);
  BLE.setLocalName("HIP-Detection");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(DetectionServcie);

  // add the characteristics to the service
  DetectionServcie.addCharacteristic(switchCharacteristic);
  DetectionServcie.addCharacteristic(LaserCharacteristic);
  DetectionServcie.addCharacteristic(PDCharacteristic);

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, &blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, &blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
  switchCharacteristic.setEventHandler(BLEWritten, &switchCharacteristicWritten);
  LaserCharacteristic.setEventHandler(BLEWritten, &LaserCharacteristicWrittten);
  // add the service
  BLE.addService(DetectionServcie);



  // set initial values for switch and laser control
  switchCharacteristic.writeValue(0);
  LaserCharacteristic.writeValue(0);

  Serial.print("DAC initializing...");
  _dac.init();
  _adc.begin(); //init ADS1235
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin,LOW);
   //start advertising
  if(!BLE.advertise()){
    Serial.println("Bluetooth Advertise failed.");
    return 0;
  }
  else{
  Serial.println("Bluetooth device advertising, waiting for connections...");

  return 1;
  }

}

unsigned long read_adc(){
  current_adc_data = _adc.RData();
  return current_adc_data;

};

int read_pct(){
  current_dac_data = _dac.read_reg(DAC_DATA);
  current_dac_pct = current_dac_data/65535 * 100;
  return current_dac_pct;
}
// int update();
 
int write_dac(int ble_dac_tgt){
  return(_dac.write_pct(ble_dac_tgt));
}

int ble_update_dac(){
  _dac_pct = LaserCharacteristic.value();
  _dac.write_pct(_dac_pct);
  Serial.print("DAC percent written: ");
  Serial.println(_dac_pct);
  return 0;
}

 unsigned long ble_update_adc(){
  current_adc_data = _adc.RData();
  if(current_adc_data >= 0X800000){
    current_adc_pct = -(0xFFFFFF-current_adc_data)*100/4194304;
  }
  else if(current_adc_data< 0x800000){
    current_adc_pct = current_adc_data*100/4194304;
  }
  Serial.print("ADC Data read: ");
  Serial.println(current_adc_data);
  PDCharacteristic.writeValue(current_adc_pct);
  Serial.print("PDCharacteristic written: ");
  Serial.println(current_adc_pct);
  return current_adc_data;
}

int ble_update_switch(){
 current_switch = switchCharacteristic.value();
 if (current_switch !=0){
  // digitalWrite(led_pin,HIGH);
  Serial.println("Switch updated : ON"); 
  return 1;
   
 }
 else{
  // digitalWrite(led_pin,LOW);
  Serial.println("Switch updated : OFF");
  return 0;
 }

// return current_switch;
}

int test_led(){
  digitalWrite(led_pin,HIGH);
  delay(500);
  digitalWrite(led_pin,LOW);
  delay(500);
  return 0;
}

private:
 ADS1235 & _adc;
 DAC80501 & _dac;
 int _dac_pct;
 int current_dac_data;
 int ble_dac_rx;
 int current_dac_pct;
 int tgt_dac_pct;
 unsigned long current_adc_data;
 int current_adc_pct;
 int current_switch;

};



#endif