#include <Arduino.h>
#include "Detection_BLE.h"

/* Initialization*/
ADS1235 ADC1;
DAC80501 DAC80501_1(Wire,DAC_ADDR);
Detection_BLE detect(ADC1,DAC80501_1);

void setup() {
  Serial.begin(9600);

  
  
  // DAC80501_1.init();
  // DAC1.status();
  Serial.println("\nI2C Scanner");
}

 
void loop() {
  // put your main code here, to run repeatedly:
}