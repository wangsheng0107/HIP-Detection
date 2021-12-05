/***************************************************
  This is a library for the HIP Detection

  Use BLE to send & receive data and command from/to a ble master device
  Control DAC80501 and receive data from ADS1235

  Sheng Wang Dec 2021
 ****************************************************/

#ifndef Detection_BLE_H
#define Detection_BLE_H


#include <ArduinoBLE.h>
#include "ADS1235.h"
#include "DAC80501.h"



class DetectionBLE  {
public: 
  DetectionBLE(ADS1235 & ADC, DAC80501 & DAC): _adc(ADC), _dac(DAC){};
  ~DetectionBLE(){};
public:
int begin();
// int read();
// int update();
 

private:
 ADS1235 & _adc;
 DAC80501 & _dac;

};

#endif


