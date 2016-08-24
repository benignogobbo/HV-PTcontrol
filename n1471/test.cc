// +--------------------------------------------------+
// | N1471 test program                               |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V1.0                                             |
// | 05 March 2014                                    |
// +--------------------------------------------------+

#include <iostream>
#include <iomanip>
#include <bitset>
#include <unistd.h>
#include "N1471.h"

int main() {
  
  const int board = 0;
  const int channel = 0;
  
  try {
    N1471 *n1471 = new N1471;
    
    bool status = n1471->connectBoard( 0 );
    std::cout << status << std::endl;

    for( int i=0; i<4; i++ ) {
      n1471->setIMRange( board, i, "HIGH" );
      uint32_t channelStatus = 0;
      float imax = 0, imin = 0, vmax = 0;
      std::string imrange = "";
      n1471->getStatus( board, i, channelStatus );
      n1471->getMaxV( board, i, vmax );
      n1471->getIMin( board, i, imin );
      n1471->getIMax( board, i, imax );
      n1471->getIMRange( board, i, imrange );
      std::bitset<32> bits( channelStatus );
      //std::cout << "Channel: " << i << ", Status : "
      // 	  << std::hex << channelStatus << std::dec << std::endl;
      std::cout << "Channel: " << i << ", Status  : " << bits << std::endl;
      std::cout << "Channel: " << i << ", VMax    : " << vmax << std::endl;
      std::cout << "Channel: " << i << ", IMin    : " << imin << std::endl;
      std::cout << "Channel: " << i << ", IMax    : " << imax << std::endl;
      std::cout << "Channel: " << i << ", IMRange : " << imrange << std::endl;
    }

    n1471->setVSet( board, channel, 4999.5 );
    n1471->setISet( board, channel, 10.5 );
    n1471->setPw( board, channel, "ON" );
    
    float vset;
    n1471->getVSet( board, channel, vset );
    std::cout << "Channel: 0, VSet: " << vset << std::endl;

    float vmon;
    n1471->getVMon( board, channel, vmon );
    std::cout << "Channel: 0, VMon: " << vmon << std::endl;

    float iset;
    n1471->getISet( board, channel, iset );
    std::cout << "Channel: 0, ISet: " << iset << std::endl;

    float imon;
    n1471->getISet( board, channel, imon );
    std::cout << "Channel: 0, IMon: " << imon << std::endl;

    std::string pw;
    n1471->getPw( board, channel, pw );
    std::cout << "Channel: 0, Pw: " << pw << std::endl;

    std::string pdwn;
    n1471->getPDwn( board, channel, pdwn );
    std::cout << "Channel: 0, PDwn: " << pdwn << std::endl;

    unsigned int alarms;
    n1471->getAlarms( 0, alarms );
    std::cout << "Channel: 0, Alarms: " << alarms << std::endl;

    n1471->setPw( board, channel, "OFF" );
    n1471->getPw( board, channel, pw );
    std::cout << "Channel: 0, Pw: " << pw << std::endl;

    for( int i=0; i<10; i++ ) {
      n1471->getVMon( board, channel, vmon );
      n1471->getIMon( board, channel, imon );
      std::cout << "Channel: " << channel << ", VMon: " << vmon << ", IMon: " << imon << std::endl;
      sleep(1);
    }

    status = n1471->disconnect();
    std::cout << status << std::endl;

  } catch( std::runtime_error &e ) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  return 0;

}
