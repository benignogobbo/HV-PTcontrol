// +--------------------------------------------------+
// | Adt7420 16 bit Digital I2C Temperature Sensor    |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V1.0                                             |
// | 26 February 2014                                 |
// +--------------------------------------------------+

#include "ADT7420.h"

// +-------------------+
// | ADT7420 configure |
// +-------------------+
bool ADT7420::configure( MYBUS* usb, unsigned char bitmap ) throw( std::runtime_error ) {
  // 1:0  fault queue: 00 - 1 fault, 01 - 2 faults, 10 - 3 faults, 11 - 4 faults
  // 2    CT polarity: 0 - active low, 1 - active high
  // 3    INT polarity: 0 - active low, 1 - active high
  // 4    INT/CT mode:  0 - interrupt mode, 1 - comparator mode
  // 6:5  Operation mode: 00 - continuous, 01 - one shot, 10 - 1 SPS mode, 11 - shutdown
  // 7    Resolution: 0 - 13-bit resolution: sign bit+ 12 bits, 1 - 16-bit resolution: sign bit + 15 bits

  char address = ADT7420_BaseAddress + _id;
  char reg = ADT7420_Config;
  char buff[1]; buff[0] = bitmap;
  _config = bitmap;
  int status = usb->writeBus( address, reg, buff, 1 );
  if( status != 1 ) {
    throw ( std::runtime_error( "ADT7420 device configuration write error" ) );
    return false;
  }
  return true;
}

// +-------------------------+
// | ADT7420 readTemperature |
// +-------------------------+
bool ADT7420::readTemperature( MYBUS* usb, double& temp ) throw( std::runtime_error ) {

  temp = 0;
  char address = ADT7420_BaseAddress + _id;
  char reg = ADT7420_TempMSB;
  char buff[8];
  int status = usb->readBus( address, reg, buff, 2 );
  if( status != 2 ) {
    throw ( std::runtime_error( "ADT7420 device temperature read error" ) );
    return false;
  }

  // calculate Temperature.
  int t = ( (unsigned char)buff[0]<<8 ) + (unsigned char)buff[1];
  if( ( _config >> 7 ) == 0 ) {
    if( t & (1<<13) ) 
      temp = ( (double)t - 8192. ) / 16.;
    else
      temp = (double)t / 16.;
  }
  else {
    if( t & (1<<16) ) 
      temp = ( (double)t - 65536. ) / 128.;
    else
      temp = (double)t / 128.;
  }

  return true;
}

