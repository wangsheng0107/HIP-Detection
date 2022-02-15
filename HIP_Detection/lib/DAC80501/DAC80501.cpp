/***************************************************
  This is a library for the TI DAC80501


  This lib use the i2c interface of the DAC.

  Written by Quan Dong
 ****************************************************/

#include "DAC80501.hpp"
#include <stdlib.h>

const uint16_t MID_SCALE_DAC_CNT = 0x8000;
const float    MID_SCALE_DAC_VOLT = 1.25;
const float    FS_LED_VOLT        = 2.5; // full scale

/**************************************************************************/
/*!
    @brief Create the interface object using hardware I2C
    @param address to use along with the default I2C device
*/
/**************************************************************************/
  // DAC80501::DAC80501(TwoWire &i2c, byte i2c_addr) : _i2c(i2c), _i2c_address( i2c_addr ){
  //   _ref  = DAC_PRF_REF;
  // };



/**************************************************************************/
/*!
    @brief Reset and initial DAC80501
    @param 
    @return The DAC output reg
*/
/**************************************************************************/
int DAC80501::reset() {
  // Reset device.
  _i2c.begin();
  write_reg(DAC_TRIGGER, DAC_SFT_RST);
  // return init();
  return 0;
}

/**************************************************************************/
/*!
    @brief Initialize DAC80501
    @param 
    @return The DAC output reg
*/
/**************************************************************************/
int DAC80501::init() {
  // Initialize I2C
  _i2c.begin(); //Begin Wire(I2C)
  Serial.println("I2C started");
  reset();// reset the DAC to default config
  Serial.println("DAC reset");
  
  //Read Device ID
  _ID=read_reg(DAC_DEVID);

  //Disable sync mode, so that the DAC outputs immediately.
  write_reg(DAC_SYNC, 0);
  _sync=0;
  
  //Turn the output to 0
  write_reg(DAC_DATA,0x8000);
  
  //Enable reference divide and output buf gain.
  //The output will be disabled if ref-div is not enabled, need circuit fix
  //Buff gain enabled to compensate the ref-div
  setGain(1,1);

  //Read DAC data reg
  uint16_t dac_data=read_reg(DAC_DATA);

  return dac_data;
}

/**************************************************************************/
/*!
    @brief check the status
    @return The raw unsigned 8-bit FAULT status register
*/
/**************************************************************************/
int DAC80501::status(void) {
  //Read DAC sync reg
  _sync=read_reg(DAC_SYNC) & 0x01;
  
  //Read DAC config reg
  uint16_t dac_config=read_reg(DAC_CONFIG);
  
  _ref_dwn = dac_config>>8 & 0x01;
  _dac_dwn = dac_config & 0x01;

  
  //Read DAC gain reg
  uint16_t dac_gain=read_reg(DAC_GAIN);
  _ref_div = 1 << ( dac_gain >> 8 & 0x01 );
  _gain= 1 << ( dac_gain & 0x01 );
  
  uint16_t DevStatus = _sync   <<4 | 
                       _ref_dwn<<3 | _dac_dwn<<2 |
                       _div    <<1 | _gain;
  return DevStatus;
}

int DAC80501::read_reg(uint8_t address) {
  Wire.beginTransmission(_addr);
  Wire.write(address);  // reg address
  Wire.endTransmission();
  
  // read 2 int, from address addr
  Wire.requestFrom(_addr, (int) 2);
  int d_in=0;
  while(Wire.available()) {
    d_in <<= 8;
    d_in |= Wire.read();
    // //Serial..println(d_in,HEX);
  }
  return d_in;
}

int DAC80501::write_reg(uint8_t address, uint16_t data) {
  Wire.beginTransmission(_addr);
  Wire.write(address);  // reg address
  Wire.write((data>>8) &0xff);
  Wire.write(data & 0xff);
  Wire.endTransmission();

  return 0;
}

int DAC80501::write_data(uint16_t data) {
  return write_reg(DAC_DATA,data);
}

int DAC80501::write_pct(int percentage){
    if (percentage > 100){
    percentage = 100;
    }
    else if (percentage < 0){
    percentage = 0;
    }
    int DAC_CNT = round(MID_SCALE_DAC_CNT / MID_SCALE_DAC_VOLT * FS_LED_VOLT /100 * percentage);
    _pct = DAC_CNT;
    write_reg(DAC_DATA,_pct);
    return _pct;

}

int DAC80501::setSync(uint16_t en) {
  _sync = en;
  uint16_t _sync_to_set = 0x0001 & en;
  return write_reg(DAC_SYNC,_sync_to_set);
}


// Set REF-DIV and BUFF-GAIN 
// REF-DIV = 1 to divide the reference voltage by 2
// BUFF-Gain = 1 to set DAC output gain to 2
int DAC80501::setGain(uint16_t ref_div_tgt, uint16_t gain_tgt) {
  uint16_t gain = (ref_div_tgt<<8) | (0x0001 & gain_tgt);
  write_reg(DAC_GAIN,gain);
  _gain = gain_tgt;
  _ref_div = ref_div_tgt;
  return 0;

}

// If DAC power down, the output will be connected to GND through a internal 1kohm resistor
int DAC80501::pwdwn(uint16_t ref_pw_tgt, uint16_t dac_pw_tgt){
  uint16_t pwd = (ref_pw_tgt<<8) | (0x0001 & dac_pw_tgt);
  write_reg(DAC_CONFIG,pwd);
  return 0;
}



int DAC80501::trigger(void) {
  return write_reg(DAC_TRIGGER,0x0010);
}