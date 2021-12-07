// #include "Detection_BLE.h"




//  int DetectionBLE::begin(){

//     if(!BLE.begin()){ 
//       Serial.print("Starting BLE failed");
//       while (1);
//     }


//     BLE.setLocalName("HIP-Detection");
//     // set the UUID for the service this peripheral advertises:
//     BLE.setAdvertisedService(DetectionServcie);

//     // add the characteristics to the service
//     DetectionServcie.addCharacteristic(switchCharacteristic);
//     DetectionServcie.addCharacteristic(LaserCharacteristic);
//     DetectionServcie.addCharacteristic(RDATACharacterstic);
//     // assign event handlers for connected, disconnected to peripheral
//     BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
//     BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
//     // add the service
//     BLE.addService(DetectionServcie);

//     // set initial values for switch and laser control
//     switchCharacteristic.writeValue(0);
//     LaserCharacteristic.writeValue(0);

//     // start advertising
//     if(!BLE.advertise()){
//       Serial.println("Bluetooth Advertise failed.");
//       return 0;
//     }
//     else{
//     Serial.println("Bluetooth device advertising, waiting for connections...");

//     return 1;
//     }
//   }