// +--------------------------------------------------+
// | USB to I2C via Attiny45                          |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V2.0                                             |
// | 05 November 2015                                 |
// +--------------------------------------------------+

#include <iostream>
#include <sstream>
#include "Attiny45.h"

//+------------------+
//| Attiny45 closeBus |
//+------------------+
bool Attiny45::closeBus( void ) throw( std::runtime_error ) {
  usb_close( _handle );
  return true;
}

//+------------------+
//| Attiny45 writeBus |
//+------------------+
int Attiny45::writeBus( unsigned char address, char reg, char buffer[], int size ) throw( std::runtime_error ) {
  
  _buff[0] = reg;
  for( unsigned int i=0; i<size; i++ ) {
    _buff[1+i] = buffer[i];
  }

  if( usb_control_msg( _handle, usbCtrlOut, cmdI2cIo + cmdI2cBegin + cmdI2cEnd, 0, address, _buff, 1+size, 1000 ) < 1 ) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
    return -1;
  }

  if( usbGetStatus() != statusAddressAck ) {
    throw ( std::runtime_error( "Write command status error" ) );
    return -1;
  }

  return( size );
}

//+-----------------+
//| Attiny45 readBus |
//+-----------------+
int Attiny45::readBus( unsigned char address, char reg, char buffer[], int size ) throw( std::runtime_error ) {
  
  if( usb_control_msg( _handle, usbCtrlOut, cmdI2cIo+cmdI2cBegin+((!size)?cmdI2cEnd:(unsigned char)0), 0, address, &reg, 1, 1000 ) < 1 ) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
    return -1;
  }

  if( usbGetStatus() != statusAddressAck ) {
    throw ( std::runtime_error( "Write command status error" ) );
    return -1;
  }

  if( !size ) return 0;

  if( usb_control_msg( _handle, usbCtrlIn, cmdI2cIo+cmdI2cEnd, i2cMRd, address, (char*)buffer, size, 1000 ) < 1 ) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
    return -1;
  }
 
  if( usbGetStatus() != statusAddressAck ) {
    throw ( std::runtime_error( "Read command status error" ) );
    return -1;
  }

  return( size );
}

//+-----------------------+
//| Attiny45 writeBusUnreg |
//+-----------------------+
int Attiny45::writeBusUnreg( unsigned char address, char buffer[], int size ) throw( std::runtime_error ) {
  
  if( usb_control_msg( _handle, usbCtrlOut, cmdI2cIo + cmdI2cBegin + cmdI2cEnd, 0, address, buffer, size, 1000 ) < 1 ) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
    return -1;
  }

  if( usbGetStatus() != statusAddressAck ) {
    throw ( std::runtime_error( "Write command status error" ) );
    return -1;
  }

  return( size );
}

//+----------------------+
//| Attiny45 readBusUnreg |
//+----------------------+
int Attiny45::readBusUnreg( unsigned char address, char buffer[], int size ) throw( std::runtime_error ) {

  if( usb_control_msg( _handle, usbCtrlIn, cmdI2cIo+cmdI2cEnd, i2cMRd, address, (char*)buffer, size, 1000 ) < 1 ) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
    return -1;
  }
 
  if( usbGetStatus() != statusAddressAck ) {
    throw ( std::runtime_error( "Read command status error" ) );
    return -1;
  }

  return( size );
}

//+---------------------+
//| Attiny45 connectBus |
//+---------------------+

void Attiny45::connectBus( usb_dev_handle* handle ) {
  _handle = handle;
  usbSet( cmdSetDelay, 10 );
}

//+-------------------+
//| Attiny45 usbWrite |
//+-------------------+

int Attiny45::usbWrite( int request, int value, int index ) throw( std::runtime_error ) {

  if( usb_control_msg( _handle, usbCtrlOut, request, value, index, NULL, 0, 1000 ) < 0) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
    return -1;
  }
  return 1;
}

//+------------------+
//| Attiny45 usbRead |
//+------------------+

int Attiny45::usbRead( unsigned char cmd, void *data, int len ) throw( std::runtime_error ) {

  int nBytes = usb_control_msg( _handle, usbCtrlIn, cmd, 0, 0, (char*)data, len, 1000 );

  if( nBytes < 0 ) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
    return nBytes;
  }

  return 0;
}

//+-----------------------------------+
//| Attiny45 usbGetI2CFunctionalities |
//+-----------------------------------+

unsigned long Attiny45::usbGetI2CFunctionalities( void ) throw( std::runtime_error ) {
  unsigned long firmVer;
  if( usbRead( cmdGetI2CFunctionalities, &firmVer, sizeof( firmVer ) ) == 0 ) {
    return firmVer;
  }
  else {
    return 0;
  }
}

//+-----------------+
//| Attiny45 usbSet |
//+-----------------+

void Attiny45::usbSet( unsigned char cmd, int value ) {
  if( usb_control_msg( _handle, USB_TYPE_VENDOR, cmd, value, 0, NULL, 0, 1000 ) < 0 ) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
  }
}

//+-----------------------+
//| Attiny45 usbGetStatus |
//+-----------------------+

int Attiny45::usbGetStatus( void ) {
  int i;
  unsigned char status;
  if(( i = usbRead( cmdGetStatus, &status, sizeof( status ))) < 0 ) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
    return i;
  }
  return status;
}

//+----------------------+
//| Attiny45 checkAccess |
//+----------------------+

bool Attiny45::checkAccess( int address ) throw( std::runtime_error ) {
  if( usb_control_msg( _handle, usbCtrlIn, cmdI2cIo + cmdI2cBegin + cmdI2cEnd, 0, address, NULL, 0, 1000 ) < 0 ) {
    std::stringstream ss;
    ss << "USB Error: " << usb_strerror();
    throw( std::runtime_error( ss.str() ) );
    return false;
  }
  if( usbGetStatus() == statusAddressAck ) {
    return true;
  }
  else {
    return false;
  }
}


