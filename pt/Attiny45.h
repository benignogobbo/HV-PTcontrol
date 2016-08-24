#ifndef Attiny45_H
#define Attiny45_H
// +--------------------------------------------------+
// | USB to I2C via Attiny45                          |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V2.0                                             |
// | 05 November 2015                                 |
// +--------------------------------------------------+

#include <stdexcept>
#include <usb.h>

// +---------------+
// | Class Attiny45 |
// +---------------+
class Attiny45 {

 public:

  Attiny45( void ) { _handle = NULL; }
  ~Attiny45( void ) {}

  void connectBus( usb_dev_handle* handle );

  bool checkAccess( int address ) throw( std::runtime_error );

  int writeBus( unsigned char address, char register, char buffer[], int size ) throw( std::runtime_error );

  int readBus( unsigned char address, char register, char buffer[], int size ) throw( std::runtime_error );

  int writeBusUnreg( unsigned char address, char buffer[], int size ) throw( std::runtime_error );

  int readBusUnreg( unsigned char address, char buffer[], int size ) throw( std::runtime_error );

  bool closeBus( void ) throw( std::runtime_error );

  unsigned long usbGetI2CFunctionalities( void ) throw( std::runtime_error );

 private:

  usb_dev_handle    *_handle;

  char    _buff[128];

  static const unsigned short AttinyVendorId  = 0x0403;
  static const unsigned short AttinyProductId = 0xc631;

  static const unsigned char cmdEcho                   = 0;
  static const unsigned char cmdGetI2CFunctionalities  = 1;
  static const unsigned char cmdSetDelay               = 2;
  static const unsigned char cmdGetStatus              = 3;
  static const unsigned char cmdI2cIo                  = 4;
  static const unsigned char cmdI2cBegin               = 1;
  static const unsigned char cmdI2cEnd                 = 2;

  static const int           usbCtrlIn  = (USB_TYPE_CLASS | USB_ENDPOINT_IN );
  static const int           usbCtrlOut = (USB_TYPE_CLASS );

  static const int statusIdle        = 0;
  static const int statusAddressAck  = 1;
  static const int statusAddressNak  = 2;

  static const unsigned char i2cMRd = 0x01;

  int usbWrite( int request, int value, int index ) throw( std::runtime_error );

  int usbRead( unsigned char cmd, void *data, int len ) throw( std::runtime_error );

  void usbSet( unsigned char cmd, int value );

  int usbGetStatus( void ); 

};

#endif // Attiny45_H
