/***************************************************
  This is a library for the TI ADS1235


  This lib use the SPI interface of the DAC.

  Written by Scott Downen
  Modded by 
 ****************************************************/
 
#ifndef ADS1235_h
#define ADS1235_h



//#include <Arduino.h>
//#include <SPI.h>

//#define ADS1235_CS_PIN     40  // define the chipselect
//#define ADS1235_RDY_STATE  MISO   // pin to watch for data ready state

#define ADC0_ADC1_COMP  0x34  //AIN0 and AIN1 = MUX 0x34 (See table 41). 00110100

/* Set up messaging and stabilization delays */
/* Note: These are intended to be used with millis(); to replace delays */
#define INTERVAL_1MS   1
#define INTERVAL_5MS   5
#define INTERVAL_1S    1000
#define CURRENT_TIME   0

/* ADS1235 Command Map */
#define ADS1235_NOP              0x00 // No operation. Use the NOP command to validate the CRC response byte and error detection
#define ADS1235_RESET            0x06 // The RESET command resets ADC operation and resets the registers to default values
#define ADS1235_START            0x08 // This command starts ADC conversions
#define ADS1235_STOP             0x0A // This command stops ADC conversions
#define ADS1235_RDATA            0x12 // This command reads conversion data. Data can be read at any time during conversion phase
#define ADS1235_SYOCAL           0x16 // This command is used for system offset calibration
#define ADS1235_GANCAL           0x17 // This command is for gain calibration.
#define ADS1235_SFOCAL           0x19 // This command is used for self offset calibration
#define ADS1235_RREG             0x20 // Use the RREG command to read register data. Add register address to 20h to construct byte
#define ADS1235_WREG             0x40 // Use the WREG command to read register data. Add register address to 40h to construct byte
#define ADS1235_LOCK             0xF2 // The LOCK command locks-out write access to the registers including the calibration registers
#define ADS1235_UNLOCK           0xF5 // The UNLOCK command allows register write access, including access to the contents of the calibration registers

/* ADS1235 Register Map; Key: R = Read only, RW = Read/Write */
#define ADS1235_ID               0x00 // Device Identification (7:4 R-DEV_ID, R; 3:1 R-REV_ID, R)
#define ADS1235_STATUS           0x01 // Device Status (7 R-Lock, 6 RW-CRC Error, 5 R-PGA Low alarm, 4 R-PGA High alarm, 3 R-Ref low alarm, 2 R-DRDY, 1 R-CLK, 0 RW-RST)
#define ADS1235_MODE0            0x02 // Mode 0 (7 Always 0, 6:3 RW-Data Rate, 2:0 RW-Digital Filter)
#define ADS1235_MODE1            0x03 // Mode 1 (7 Always 0, 6:5 RW-Chop/Bridge Excitation Modes, 4 RW-ADC Conversion Mode, 3:0 RW-Conversion Start Delay)
#define ADS1235_MODE2            0x04 // Mode 2 (RW-Internal GPIO Pin Connections)
#define ADS1235_MODE3            0x05 // Mode 3 (All RW; 7 Power Down, 6 Status, 5 CRC Verification, 4 SPI Auto Reset, 3 GPIO3, 2 GPIO2, 1 GPIO1, 0 GPIO0)
#define ADS1235_REF              0x06 // Reference Configuration (All RW; 7:4 always 0, 3:2 Ref Pos Input, 1:0 Ref Neg Input)
#define ADS1235_OFCAL0           0x07 // 24-bit offset word. The ADC subtracts the offset value from the conversion result before the full-scale operation.
#define ADS1235_OFCAL1           0x08 // 24-bit offset word. The ADC subtracts the offset value from the conversion result before the full-scale operation.
#define ADS1235_OFCAL2           0x09 // 24-bit offset word. The ADC subtracts the offset value from the conversion result before the full-scale operation.
#define ADS1235_FSCAL0           0x0A // 24-bit calibration word. The ADC divides the register value by 400000h then multiplies the result with the conversion data
#define ADS1235_FSCAL1           0x0B // 24-bit calibration word. The ADC divides the register value by 400000h then multiplies the result with the conversion data
#define ADS1235_FSCAL2           0x0C // 24-bit calibration word. The ADC divides the register value by 400000h then multiplies the result with the conversion data
#define ADS1235_PGA              0x10 // Programmable gain (All RW; 7 PGA bypass, 6:3 always 0, 2:0 gain selection)
#define ADS1235_INPMUX           0x11 // Input multiplexer (All RW; 7:4 Pos Input multiplexer, 3:0 Neg input multiplexer)





class ADS1235
{
public:
  ADS1235(int ss_pin):SSPin(ss_pin){}; //need to add this function
  ~ADS1235(){};
  
  bool begin(void);
  int Write(int input);
  void Nop(void);
  void Reset(void);
  
  void StartADC(void);
  void StopADC(void);
  unsigned long RData(void);

  void SYOCal(void);
  void GANCal(void);
  void SFOCal(void);

  void OFFCal(int offsetvalue);

  int RReg(int rrh);
  void WReg(int rrh, int value);
  
  void Lock(void);
  void Unlock(void);

  void DevID(void);
  void Status(void);

  void Mode0(void);
  void Mode1(void);
  void Mode2(void);
  void Mode3(void);

  void SetupADC(void);
  void Gain(void);
  void MUX(void);




private:
 int SSPin; 
};

#endif
