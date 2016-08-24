#ifndef ADT7420_H
#define ADT7420_H
// +--------------------------------------------------+
// | ADT7420 16 bit Digital I2C Temperature Sensor    |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V1.0                                             |
// | 26 February 2014                                 |
// +--------------------------------------------------+

#include <stdexcept>

#ifdef USB_ISS_DEVICE
#  define MYBUS USB_ISS
#  include "USB_ISS.h"
#else
#  define MYBUS Attiny45
#  include "Attiny45.h"
#endif

// +---------------+
// | Class ADT7420 |
// +---------------+
class ADT7420 {

public:

  ADT7420( void ) : _id(0), _config(0) {}
  ADT7420( int id ) : _id(id), _config(0) {}
  ~ADT7420( void ) {}

  bool configure( MYBUS* usb, unsigned char bitmap ) throw( std::runtime_error );

  bool readTemperature( MYBUS* usb, double& temp ) throw( std::runtime_error );

protected:

  enum {
    ADT7420_BaseAddress = 0x48,
    // registers
    ADT7420_TempMSB = 0x00,
    ADT7420_TempLSB = 0x01,
    ADT7420_Status  = 0x02,
    ADT7420_Config  = 0x03,
    ADT7420_ThigMSB = 0x04,
    ADT7420_ThigLSB = 0x05,
    ADT7420_TlowMSB = 0x06,
    ADT7420_TlowLSB = 0x07,
    ADT7420_TcriMSB = 0x08,
    ADT7420_TcriLSB = 0x09,
    ADT7420_ThysSP  = 0x0A,
    ADT7420_ID      = 0x0B,
    ADT7420_SwReset = 0x2F 
  };
  
private:

  int _id; // _id: 0, 1, 2, 3
  unsigned char _config;

};

#endif // ADT7420_H
