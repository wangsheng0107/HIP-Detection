#include "Detection_BLE.h"





int DetectionBLE::begin(){

  if(!BLE.begin()){ 
    Serial.print("Starting BLE failed");
    while (1);
  }


  BLE.setLocalName("HIP-Detection");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(DetectionServcie);

  // add the characteristics to the service
  DetectionServcie.addCharacteristic(switchCharacteristic);
  DetectionServcie.addCharacteristic(LaserCharacteristic);
  DetectionServcie.addCharacteristic(RDATACharacterstic);

  // add the service
  BLE.addService(DetectionServcie);

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
  switchCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);

  // set initial values for switch and laser control
  switchCharacteristic.writeValue(0);
  LaserCharacteristic.writeValue(0);

  // start advertising
  if(!BLE.advertise()){
    Serial.println("Bluetooth Advertise failed.");
    return 0;
  }
  else{
  Serial.println("Bluetooth device advertising, waiting for connections...");

  return 1;
  }
}


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
    digitalWrite(13, HIGH);
  } else {
    Serial.println("LED off");
    digitalWrite(13, LOW);
  }
}