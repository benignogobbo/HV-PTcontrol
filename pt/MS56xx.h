#ifndef MS56XX_H
#define MS56XX_H
// +--------------------------------------------------+
// | MS56xx Barometric Pressure Sensor                |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V1.1                                             |
// | 02 July 2014                                     |
// +--------------------------------------------------+

#include <stdexcept>
#include <sys/types.h>

#ifdef USB_ISS_DEVICE
#  define MYBUS USB_ISS
#  include "USB_ISS.h"
#else
#  define MYBUS Attiny45
#  include "Attiny45.h"
#endif

// +--------------+
// | class MS56xx |
// +--------------+
class MS56xx {

public:

  MS56xx( void ) : _id(1), _serialNumber(0) {} 
  MS56xx( int id ) : _id(1-id), _serialNumber(0) {}   // On chip, Id is set by complemento of CSV pin. 
  ~MS56xx( void ) {}

  bool init( MYBUS* usb ) throw( std::runtime_error );
  bool readRawPressure( MYBUS* usb, int& ipress ) throw( std::runtime_error );
  bool readRawTemperature( MYBUS* usb, int& itemp ) throw( std::runtime_error );
  bool getPressure( MYBUS* usb, double& press ) throw( std::runtime_error );
  bool getTemperature( MYBUS* usb, double& temp ) throw( std::runtime_error );
  bool getAltitude( MYBUS* usb, double& alt ) throw( std::runtime_error );
  inline int getCoefficient( int i ) { if( i>=0 && i<=7 ) return _c[i]; else return 0; }
  inline int getSerialNumber( void ) { return _serialNumber; }
  
protected:

  enum {
    MS56xx_BaseAddress = 0x76,
    MS56xx_Reset       = 0x1e,
    MS56xx_ADC_Read    = 0x00,
    MS56xx_ADC_Conv    = 0x40,
    MS56xx_ADC_D1      = 0x00,
    MS56xx_ADC_D2      = 0x10,
    MS56xx_OSR_256     = 0x00,
    MS56xx_OSR_512     = 0x02,
    MS56xx_OSR_1024    = 0x04,
    MS56xx_OSR_2048    = 0x06,
    MS56xx_OSR_4096    = 0x08,
    MS56xx_PROM_Read   = 0xa0
  };

private:
 
  int _id; // _id: 0, 1
  int64_t _c[8];
  int16_t _serialNumber;
  bool readPROM( MYBUS* usb ) throw( std::runtime_error );
  bool readADC( MYBUS* usb, char command, int& adc ) throw( std::runtime_error );
  unsigned char evaluateCRC4( void ) throw( std::runtime_error );
  bool writeCommand( MYBUS* usb, char command, int ms = 0 ) throw( std::runtime_error );
};

#endif // MS56XX_H
