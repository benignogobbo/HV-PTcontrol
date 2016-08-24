// +--------------------------------------------------+
// | initialization                                   |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V1.0                                             |
// | 05 November 2015                                 |
// +--------------------------------------------------+

#include <cstdlib>
#include <usb.h>
#include "init.h"

std::vector<Attiny45*> _attinys;
std::vector<ADT7420*>  _adts;
std::vector<MS56xx*>   _ms56s;

void init( void ) {

  usb_init();
  usb_find_busses();
  usb_find_devices();

  struct usb_bus *bus;
  struct usb_device *device;
  struct usb_dev_handle *handle;

  const unsigned short AttinyVendorId  = 0x0403;
  const unsigned short AttinyProductId = 0xc631;

  bool found = false;
  for( bus = usb_get_busses(); bus; bus = bus->next ) {
    for( device = bus->devices; device; device = device->next ) {
      if(( device->descriptor.idVendor == AttinyVendorId ) && ( device->descriptor.idProduct == AttinyProductId )) {
	if( ! ( handle = usb_open( device ) ) ) {
	  throw ( std::runtime_error( "Cannot oped USB device" ) );
	  exit(1);
	}
	else {
	  Attiny45* attiny = new Attiny45;
	  attiny->connectBus( handle );
	  std::cout << "Found Attiny device. I2C Functionalities: " << std::hex << attiny->usbGetI2CFunctionalities() << std::dec << "<br>" << std::endl;
	  _attinys.push_back( attiny );
	  ADT7420* adt = new ADT7420( 0 );
	  adt->configure( attiny, (1<<7) );
	  _adts.push_back( adt );
	  MS56xx* ms = new MS56xx( 0 );
	  ms->init( attiny );
	  _ms56s.push_back( ms );
	  found = true;
	}
      }
    }
  }
  if( !found ) {
    std::cout << "No attiny45 devices found" << std::endl;
    exit(1);
  }
  
}
