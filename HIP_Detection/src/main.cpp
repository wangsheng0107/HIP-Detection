#include <Arduino.h>
#include "Detection_BLE.h"



int ADS1235_SSPin=12;
/* Initialization*/
ADS1235 ADS1235_1(ADS1235_SSPin); //ADS1235 use default SPI interface, need to change class in ADS1235.h to improve
DAC80501 DAC80501_1(Wire,DAC_ADDR); //
DetectionBLE Detection_BLE(ADS1235_1,DAC80501_1);

void setup() {

  Serial.begin(9600);
  while (!Serial);
  if(!Detection_BLE.begin()){
  Serial.println("\nBLE init failed");
  while(1);
}
  Serial.println("\nBLE init done");
  int current_dac_pct = 0;
}
 
void loop() {
  BLE.poll();
  Serial.println("\nLooped");
  delay(5000);
  // switchCharacteristic.setValue(0);
  
}