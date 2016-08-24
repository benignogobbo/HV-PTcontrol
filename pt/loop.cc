// +--------------------------------------------------+
// | libAttiny test program                           |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V2.0                                             |
// | 05 November 2015                                 |
// +--------------------------------------------------+

#include <iostream>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include "init.h"

int main() {

  try {

    init();

    while( true ) {

      for( unsigned int i=0; i<_attinys.size(); i++ ) {
      
	double temp1 = 0, temp2 = 0, pres1 = 0;
	_ms56s[i]->getTemperature( _attinys[i], temp1 );
	_ms56s[i]->getPressure( _attinys[i], pres1 );
	_adts[i]->readTemperature( _attinys[i], temp2 );
	time_t now = time(0);
	struct tm tstruct = *localtime( &now );
	char buf[80];
	strftime( buf, sizeof(buf), "%Y/%m/%d %X", &tstruct );
	std::cout << buf 
		  << " - MS56xx s/n: " << _ms56s[i]->getSerialNumber()
		  << ", P: " << pres1 << ", T: " << temp1
	  	  << " - ADT7429 T: " << temp2 << std::endl;
	
      }
      sleep( 10 );  // Set here the seconds between measurements...
    }
  } catch( std::runtime_error &e ) {
    std::cout << e.what() << std::endl;
    return -1;
  }
  
  return 0;

}
