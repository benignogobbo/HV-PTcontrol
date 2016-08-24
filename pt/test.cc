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
#include "init.h"

int main() {

  try {

    init();
    
    for( unsigned int i=0; i<_attinys.size(); i++ ) {

      std::cout << "---------- " << std::setw(2) << i << " ----------" << std::endl
		<< "MS56xx  Serial Num.:  " << _ms56s[i]->getSerialNumber() << std::endl;
      
      std::cout << "MS56xx PROM: ";
      for( int j=0; j<8; j++ ) {
	std::cout << _ms56s[i]->getCoefficient(j) << " ";
      }
      std::cout << std::endl;

      double temp = 0;
      _ms56s[i]->getTemperature( _attinys[i], temp );
      std::cout << "MS56xx  Temperature:  " << std::fixed << std::setprecision(2) << temp << std::endl;
      double pres = 0;
      _ms56s[i]->getPressure( _attinys[i], pres );
      std::cout << "MS56xx     Pressure: " << pres/100. << std::endl;

      _adts[i]->readTemperature( _attinys[i], temp );
      std::cout << "ADT7420 Temperature:  " << std::fixed << std::setprecision(2) << temp << std::endl;

    }
  } catch( std::runtime_error &e ) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  return 0;

}
