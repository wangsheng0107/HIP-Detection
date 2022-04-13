#include <Arduino.h>
#include "Detection_BLE.hpp"


int test_led = 13;
int ADS1235_SSPin=D10;
/* Initialization*/
ADS1235 ADS1235_1(ADS1235_SSPin); //ADS1235 use default SPI interface, need to change class in ADS1235.h to improve
DAC80501 DAC80501_1(Wire,DAC_ADDR); //
DetectionBLE Detection_BLE(ADS1235_1,DAC80501_1);

void setup() {

  Serial.begin(9600);
  while (!Serial);
  // pinMode(led_pin,OUTPUT);
  if(!Detection_BLE.begin()){
  Serial.println("\nBLE init failed");
  while(1);
}
  Serial.println("\nBLE init done");
}
 
void loop() {
  BLE.poll();
  // Serial.println("\nLooped");
  // delay(5000);
  // switchCharacteristic.setValue(0);
  Detection_BLE.ble_update_dac();
  Detection_BLE.ble_update_adc();
  // Detection_BLE.ble_update_switch();
  Serial.print("DAC_DATA: ");
  Serial.println(DAC80501_1.read_reg(DAC_DATA));
  Serial.print("DAC_Status: ");
  Serial.println(DAC80501_1.read_reg(DAC_STATUS));
  Serial.println("//////////////////////////////");
  delay(3000);
  // digitalWrite(led_pin,HIGH);
  // delay(500);
  // digitalWrite(led_pin,LOW);
  // delay(500);
  // Detection_BLE.test_led();

}