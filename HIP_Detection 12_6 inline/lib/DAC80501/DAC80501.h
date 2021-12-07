/***************************************************
  This is a library for the TI DAC80501

  This lib use the i2c interface of the DAC.

  NOTE:  ref-div register must be set to 1 to enable output, 
          when powered by 3.3V. Set buff-gain register to 1 
          to compensate ref-div.

  NOTE:  DAC80501M used, initial DAC-DATA register value 0x8000
          Switch to DAC80501Z if necessary.


  Sheng Wang 11/26/2021
 ****************************************************/

#ifndef DAC80501_H
#define DAC80501_H

#include <Wire.h>



//DAC80501 Address
const int DAC_ADDR   = 0x48; 

//Reg map
const int  DAC_DEVID    = 0x01;   //Device ID, should read 0x0215
const int  DAC_SYNC     = 0x02;   //bit[0] DAC_SYNC_EN
const int  DAC_CONFIG   = 0x03;   //[8] REF_PWDWN, [0] DAC_PWDWN
const int  DAC_GAIN     = 0x04;   //[8] REF-DIV,   [0] BUF-GAIN
const int  DAC_TRIGGER  = 0x05;   //[4] LDAC,    [3:0] SOFT-RESET
const int  DAC_STATUS   = 0x07;   //[0] REF_ALARM
const int  DAC_DATA     = 0x08;   //[15:0] DAC-DATA

//Command
const uint16_t DAC_SFT_RST  = 0x000A; //SOFT-RESET cmd

//Prefered values
const float    DAC_PRF_REF  = 2.5;    //Internal reference 2.5V
const uint16_t DAC_PRF_DIV  = 1;      //Set ref div to 1/2
const uint16_t DAC_PRF_GAIN = 1;      //Set output gain to 2
//

/*! Interface class for the DAC80501 */
class DAC80501 {
public:
  DAC80501(TwoWire &i2c, byte i2c_addr) : _i2c(i2c), _i2c_address( i2c_addr ){
    _ref  = DAC_PRF_REF;
  };
  ~DAC80501(){};
  
  int  reset();
  int status();
  int  init();
  int read_reg(uint8_t addr);
  int write_reg(uint8_t addr, uint16_t data);
  int setSync(uint16_t sync);
  int setGain(uint16_t ref_dig_tgt, uint16_t gain_tgt);
  int trigger();
  int write_data(uint16_t data);
  int write_pct(int percent);
  // int setVolt(double volt);
  int pwdwn(uint16_t ref_pw_tgt, uint16_t dac_pw_tgt);

private:

  double _ref;
  int _ID;
  int _sync;
  int _ref_div;
  int _gain;
  int _ref_dwn;
  int _dac_dwn;
  int _addr;
  int _div;
  int _pct;
private:
  TwoWire & _i2c;
  const byte _i2c_address;
  
};

#endif
