// +--------------------------------------------------+
// | N1471 wrapper library                            |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V1.1                                             |
// | 18 November 2015                                 |
// +--------------------------------------------------+

// --- to be removed --/
#include <bitset>
// --------------------/
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include "N1471.h"

bool N1471::_n1471DeviceLocked = false;

N1471::N1471( void ) {
  for( int i=0; i<4; i++ ) { _vok[i] = false; _iok[i] = false; }
}

bool N1471::connectBoard( int board ) throw( std::runtime_error ) {

  char buff[256];
  std::string interface = "\'ATTRS{interface}==\"USB <-> Serial\"\'";
  std::string comm1 = "udevadm info -a -n ";
  std::string comm2 = " | grep ";

  // Look for device:
  std::vector<std::string> files;
  DIR *dp = opendir( "/dev" );
  struct dirent *dirp;
  while( (dirp = readdir(dp))  != NULL ) {
    files.push_back( std::string( dirp->d_name ) );
  }
  std::string s = "ttyUSB";
  std::vector<std::string> devices;

  for( unsigned int i=0; i<files.size(); i++ ) {
    if( files[i].substr(0,6) == s ) {
      devices.push_back( "/dev/" + files[i] );
    }
  }
  
  std::vector<std::string> CAENDevices;
  for( unsigned int i=0; i<devices.size(); i++ ) {
    std::string command = comm1 + devices[i] + comm2 + interface;
    FILE* f = popen( command.c_str(), "r" );
    if( f ) {
      while( !feof( f ) ) {
        if( fgets( buff, 256, f ) != NULL ) {
          CAENDevices.push_back( devices[i] );
        }
      }
      pclose( f );
    }
  }

  if( CAENDevices.size() == 1 ) {

    _fd = open( CAENDevices[0].c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK );
    if( _fd < 0 ) {
      std::stringstream s;
      s << strerror( errno );
      throw ( std::runtime_error( s.str() ) );
      return false;
    }  

    tcgetattr( _fd, &_oldtio );
    _newtio = _oldtio;

    cfsetispeed( &_newtio, (speed_t)B9600 );
    cfsetospeed( &_newtio, (speed_t)B9600 );
    cfmakeraw(   &_newtio );

    tcsetattr( _fd, TCSANOW, &_newtio );  
    tcflush( _fd, TCIOFLUSH ); 

    // At this poit we should be connected and able to chat with module, let's test...
    std::stringstream command;
    command << "$BD:"
	    << std::setw(2) << std::setfill( '0' ) << board
	    << ",CMD:MON,PAR:BDNAME\r\n";
    std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);    
    int status = write( _fd, command.str().c_str(), command.str().size() );
    if( status != command.str().size() ) {
      throw ( std::runtime_error( "Error in writing to module" ) );
      return false;
    }
    std::string answer = serialRead();
    if( checkAnswer( answer ) == false ) {
      return false;
    }
    answer = answer.substr( 7 );
    std::cout << "\nModule type: " << answer << std::endl;

    command.str(""); command.clear();
    command << "$BD:"
	    << std::setw(2) << std::setfill( '0' ) << board
	    << ",CMD:MON,PAR:BDFREL\r\n";
    std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);    
    status = write( _fd, command.str().c_str(), command.str().size() );
    if( status != command.str().size() ) {
      throw ( std::runtime_error( "Error in writing to module" ) );
      return false;
    }
    answer = serialRead();
    if( checkAnswer( answer ) == false ) {
      return false;
    }
    answer = answer.substr( 7 );
    std::cout << "Firmware Release: " << answer << std::endl;

    command.str(""); command.clear();
    command << "$BD:"
	    << std::setw(2) << std::setfill( '0' ) << board
	    << ",CMD:MON,PAR:BDSNUM\r\n";
    std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);    
    status = write( _fd, command.str().c_str(), command.str().size() );
    if( status != command.str().size() ) {
      throw ( std::runtime_error( "Error in writing to module" ) );
      return false;
    }
    answer = serialRead();
    if( checkAnswer( answer ) == false ) {
      return false;
    }
    answer = answer.substr( 7 );
    std::cout << "Serial Number: " << answer << std::endl;

    return true;
  }
  
  return false;
  
}

bool N1471::setFloat( int board, int channel, std::string variable, float min, float max, float value, float format ) throw( std::runtime_error ) {
  
  waitForUnlock();
  
  if( board < 0 || board > 99  ) {
    std::stringstream s;
    s << "'board' must be between 0 and 99"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  
  if( channel < 0 || channel > 3  ) {
    std::stringstream s;
    s << "'channel' must be 0, 1, 2 or 3"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  
  if( value < min || value > max  ) {
    std::stringstream s;
    s << variable << " value out of limits: [" << min << ", " << max << "]"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  
  
  std::stringstream command;
  int p, w;
  double wd;
  p = int( ceil( modf( format, &wd ) * 10. ) );
  w = int( wd );
  command << "$BD:"
	  << std::setw(2) << std::setfill( '0' ) << board
	  << ",CMD:SET,CH:" << channel
	  << ",PAR:" << variable
	  << ",VAL:"
	  << std::fixed 
	  << std::setw( w+p+1 )
	  << std::setprecision( p )
	  << value 
	  << "\r\n";
  std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);
  int status = write( _fd, command.str().c_str(), command.str().size() );
  if( status != command.str().size() ) {
    throw ( std::runtime_error( "Error in writing to module" ) );
    unlock();
    return false;
  }
    
  std::string answer = serialRead();
  if( checkAnswer( answer ) == false ) {
    unlock();
    return false;
  }
  unlock();
  return true;
}

bool N1471::setOnOff( int board, int channel, std::string variable, std::string on, std::string off, std::string value ) throw( std::runtime_error ) {

  waitForUnlock();

  if( board < 0 || board > 99  ) {
    std::stringstream s;
    s << "'board' must be between 0 and 99"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  
  if( channel < 0 || channel > 3  ) {
    std::stringstream s;
    s << "'channel' must be 0, 1, 2 or 3"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  

  if( value != on && value != off ) {
    std::stringstream s;
    s << value << " must be '" << on << "' or '" << off << "'";
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }

  std::stringstream command;
  command << "$BD:"
	  << std::setw(2) << std::setfill( '0' ) << board
	  << ",CMD:SET,CH:" << channel
	  << ",PAR:" << variable
	  << ",VAL:"
	  << value
	  << "\r\n";
  std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);
  int status = write( _fd, command.str().c_str(), command.str().size() );
  if( status != command.str().size() ) {
    throw ( std::runtime_error( "Error in writing to module" ) );
    unlock();
    return false;
  }
    
  std::string answer = serialRead();
  if( checkAnswer( answer ) == false ) {
    unlock();
    return false;
  }
  unlock();
  return true;
}

bool N1471::setPw( int board, int channel, std::string ON_OFF ) throw( std::runtime_error ) {

  waitForUnlock();

  if( board < 0 || board > 99  ) {
    std::stringstream s;
    s << "'board' must be between 0 and 99"; 
    throw ( std::runtime_error( s.str() ) );    
    unlock();
    return false;
  }  
  if( channel < 0 || channel > 3  ) {
    std::stringstream s;
    s << "'channel' must be 0, 1, 2 or 3"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  
  if( ON_OFF != "ON" && ON_OFF != "OFF" ) {
    std::stringstream s;
    s << ON_OFF << " must be 'ON' or 'OFF'";
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }

 std::stringstream command;
 command << "$BD:"
	 << std::setw(2) << std::setfill( '0' ) << board
	 << ",CMD:SET,CH:" << channel
	 << ",PAR:" << ON_OFF
	 << "\r\n";
 std::cout << command.str() << std::endl;
 std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);
 int status = write( _fd, command.str().c_str(), command.str().size() );
 if( status != command.str().size() ) {
   throw ( std::runtime_error( "Error in writing to module" ) );
   unlock();
   return false;
  }
    
  std::string answer = serialRead();
  std::cout << answer << std::endl;
  if( checkAnswer( answer ) == false ) {
    unlock();
    return false;
  }
  unlock();
  return true;
}

bool N1471::getStatus( int board, int channel, uint32_t &status )  throw( std::runtime_error ) {

  waitForUnlock();

  if( board < 0 || board > 99  ) {
    std::stringstream s;
    s << "'board' must be between 0 and 99"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  
  if( channel < 0 || channel > 3  ) {
    std::stringstream s;
    s << "'channel' must be 0, 1, 2 or 3"; 
    throw ( std::runtime_error( s.str() ) );    
    unlock();
    return false;
  }  
  std::stringstream command;

  command << "$BD:"
	  << std::setw(2) << std::setfill( '0' ) << board
	  << ",CMD:MON,CH:" << channel
	  << ",PAR:STAT\r\n"; 
  std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);
  int locstatus = write( _fd, command.str().c_str(), command.str().size() );
  if( locstatus != command.str().size() ) {
    throw ( std::runtime_error( "Error in writing to module" ) );
    unlock();
    return false;
  }
  std::string answer = serialRead();
  if( checkAnswer( answer ) == false ) {
    unlock();
    return false;
  }
  answer = answer.substr( 7 );
  status = atof( answer.c_str() );
  unlock();
  return true;
}

bool N1471::getFloat( int board, int channel, std::string variable, float &value )  throw( std::runtime_error ) {

  waitForUnlock();

  if( board < 0 || board > 99  ) {
    std::stringstream s;
    s << "'board' must be between 0 and 99"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  
  if( channel < 0 || channel > 3  ) {
    std::stringstream s;
    s << "'channel' must be 0, 1, 2 or 3"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  

  std::stringstream command;
  command << "$BD:"
	  << std::setw(2) << std::setfill( '0' ) << board
	  << ",CMD:MON,CH:" << channel
	  << ",PAR:" << variable
	  << "\r\n";
  std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);
  int status = write( _fd, command.str().c_str(), command.str().size() );
  if( status != command.str().size() ) {
    throw ( std::runtime_error( "Error in writing to module" ) );
    unlock();
    return false;
  }
    
  std::string answer = serialRead();
  if( checkAnswer( answer ) == false ) {
    unlock();
    return false;
  }
  answer = answer.substr( 7 );
  value = atof( answer.c_str() );
  unlock();
  return true;
}

bool N1471::getOnOff( int board, int channel, std::string variable, std::string on, std::string off, std::string &value )  throw( std::runtime_error ) {

  waitForUnlock();

  if( board < 0 || board > 99  ) {
    std::stringstream s;
    s << "'board' must be between 0 and 99"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  
  if( channel < 0 || channel > 3  ) {
    std::stringstream s;
    s << "'channel' must be 0, 1, 2 or 3"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  

  std::stringstream command;
  command << "$BD:"
	  << std::setw(2) << std::setfill( '0' ) << board
	  << ",CMD:MON,CH:" << channel
	  << ",PAR:" << variable
	  << "\r\n";
  std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);
  int status = write( _fd, command.str().c_str(), command.str().size() );
  if( status != command.str().size() ) {
    throw ( std::runtime_error( "Error in writing to module" ) );
    unlock();
    return false;
  }

  std::string answer = serialRead();
  if( checkAnswer( answer ) == false ) {
    unlock();
    return false;
  }
  answer = answer.substr( 7 );
  value = answer;
  unlock();
  return true;
}

bool N1471::getPw( int board, int channel, std::string &pw )  throw( std::runtime_error ) {

  if( board < 0 || board > 99  ) {
    std::stringstream s;
    s << "'board' must be between 0 and 99"; 
    throw ( std::runtime_error( s.str() ) );
    return false;
  }  
  if( channel < 0 || channel > 3  ) {
    std::stringstream s;
    s << "'channel' must be 0, 1, 2 or 3"; 
    throw ( std::runtime_error( s.str() ) );
    return false;
  }  
  uint32_t status;
  if( getStatus( board, channel, status ) ) {
    if( ( status & 0x1 ) == 1 ) {
      pw = "ON";
    }
    else {
      pw = "OFF";
    }
    return true;
  }
  else {
    std::string s = "Error in getting module status";
    throw ( std::runtime_error( s ) );
    pw = "";
    return false;
  } 
}

bool N1471::getAlarms( int board, unsigned int &alarms ) throw( std::runtime_error ) {

  waitForUnlock();

  if( board < 0 || board > 99  ) {
    std::stringstream s;
    s << "'board' must be between 0 and 99"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  

  std::stringstream command;
  command << "$BD:"
	  << std::setw(2) << std::setfill( '0' ) << board
	  << ",CMD:MON,PAR:BDALARM\r\n";
  std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);    
  int status = write( _fd, command.str().c_str(), command.str().size() );
  if( status != command.str().size() ) {
    throw ( std::runtime_error( "Error in writing to module" ) );
    unlock();
    return false;
  }
  std::string answer = serialRead();
  if( checkAnswer( answer ) == false ) {
    unlock();
    return false;
  }
  answer = answer.substr( 7 );
  alarms = atof( answer.c_str() );
    unlock();
  return true;
}

bool N1471::clearAlarms( int board ) throw( std::runtime_error ) {

  waitForUnlock();

  if( board < 0 || board > 99  ) {
    std::stringstream s;
    s << "'board' must be between 0 and 99"; 
    throw ( std::runtime_error( s.str() ) );
    unlock();
    return false;
  }  
  std::stringstream command;
  command << "$BD:"
	  << std::setw(2) << std::setfill( '0' ) << board
	  << ",CMD:SET,PAR:BDCLR\r\n";
  std::cout.precision(6); std::cout.unsetf( std::ios_base::floatfield);    
  int status = write( _fd, command.str().c_str(), command.str().size() );
  if( status != command.str().size() ) {
    throw ( std::runtime_error( "Error in writing to module" ) );
    unlock();
    return false;
  }
  std::string answer = serialRead();
  if( checkAnswer( answer ) == false ) {
    unlock();
    return false;
  }
  unlock();
  return true;
}  

bool N1471::setVSet( int board, int channel, float voltage ) throw( std::runtime_error ) {
  return setFloat( board, channel, "VSET", 0, 5500, voltage, 4.1 );
}

bool N1471::setISet( int board, int channel, float current ) throw( std::runtime_error ) {
  return setFloat( board, channel, "ISET", 0, 10000, current, 4.2 );
}

bool N1471::setRUp( int board, int channel, float vps ) throw( std::runtime_error ) {
  return setFloat( board, channel, "RUP", 1, 500, vps, 3.0 );
}

bool N1471::setRDwn( int board, int channel, float vps ) throw( std::runtime_error ) {
  return setFloat( board, channel, "RDW", 1, 500, vps, 3.0 );
}

bool N1471::setTrip( int board, int channel, float seconds ) throw( std::runtime_error ) {
  return setFloat( board, channel, "TRIP", 0, 1000, seconds, 4.1 );
}

bool N1471::setMaxV( int board, int channel, float voltage ) throw( std::runtime_error ) {
  return setFloat( board, channel, "MAXV", 0, 5600, voltage, 4.0 );
}

bool N1471::setPDwn( int board, int channel, std::string RAMP_KILL ) throw( std::runtime_error ) {
  return setOnOff( board, channel, "PDWN", "RAMP", "KILL", RAMP_KILL );
}

bool N1471::setIMRange( int board, int channel, std::string LOW_HIGH ) throw( std::runtime_error ) {
  return setOnOff( board, channel, "IMRANGE", "LOW", "HIGH",  LOW_HIGH );
}
 
bool N1471::getVSet( int board, int channel, float &vset )  throw( std::runtime_error ) {
  return getFloat( board, channel, "VSET", vset );
}

bool N1471::getVMon( int board, int channel, float &vmon )  throw( std::runtime_error ) {
  return getFloat( board, channel, "VMON", vmon );
}

bool N1471::getISet( int board, int channel, float &iset )  throw( std::runtime_error ) {
  return getFloat( board, channel, "ISET", iset );
}

bool N1471::getIMax( int board, int channel, float &imax )  throw( std::runtime_error ) {
  return getFloat( board, channel, "IMAX", imax );
}

bool N1471::getIMin( int board, int channel, float &imin )  throw( std::runtime_error ) {
  return getFloat( board, channel, "IMIN", imin );
}

bool N1471::getIMon( int board, int channel, float &imon )  throw( std::runtime_error ) {
  return getFloat( board, channel, "IMON", imon );
}

bool N1471::getIMRange( int board, int channel, std::string &imrange )  throw( std::runtime_error ) {
  return getOnOff( board, channel, "IMRANGE", "LOW", "HIGH", imrange );
}

bool N1471::getMaxV( int board, int channel, float &maxv )  throw( std::runtime_error ) {
  return getFloat( board, channel, "MAXV", maxv );
}

bool N1471::getRUp( int board, int channel, float &rup )  throw( std::runtime_error ) {
  return getFloat( board, channel, "RUP", rup );
}

bool N1471::getRDwn( int board, int channel, float &rdwn )  throw( std::runtime_error ) {
  return getFloat( board, channel, "RDW", rdwn );
}

bool N1471::getPolarity( int board, int channel, std::string &polarity )  throw( std::runtime_error ) {
  return getOnOff( board, channel, "POL", "-", "+", polarity );
}

bool N1471::getPDwn( int board, int channel, std::string &pdwn )  throw( std::runtime_error ) {
  return getOnOff( board, channel, "PDWN", "RAMP", "KILL", pdwn );
}

bool N1471::getTrip( int board, int channel, float &trip )  throw( std::runtime_error ) {
  return getFloat( board, channel, "TRIP", trip );
}

bool N1471::disconnect( void ) throw( std::runtime_error ) {

  tcsetattr( _fd, TCSANOW, &_oldtio ); /* restore old port settings */
  tcflush( _fd, TCIOFLUSH );
  if( close( _fd ) < 0 ) {
    std::stringstream s;
    s << strerror( errno ); 
    throw ( std::runtime_error( s.str() ) );
    return false;
  } 
  return true;
}

std::string N1471::serialRead( void ) {
  fd_set read_fds, write_fds, except_fds;
  FD_ZERO( &read_fds );
  FD_ZERO( &write_fds );
  FD_ZERO( &except_fds );
  FD_SET( _fd, &read_fds );
  struct timeval timeout;

  timeout.tv_sec = 0;        // timeout: 1 s.
  timeout.tv_usec = 250000;

  std::string s = "";
  char buff[512];
  int status = 0;
  int rv = 0;
  do {
    if( ( rv = select( _fd+1, &read_fds, &write_fds, &except_fds, &timeout )) ==1 ) {
      status = read( _fd, buff, 1 );
      if( status == 1 ) {
        s += buff[0];
      }
    } 
  } while( status > 0 && rv == 1 );
  return s;
}

bool N1471::checkAnswer( std::string &answer ) {
  if( answer.substr( 0, 3 ) != "#BD" ) {
    std::stringstream s;
    s << "Error from module: " << answer;
    throw ( std::runtime_error( s.str() ) );
    return false;
  }
  answer = answer.substr(11);
  if( answer.substr(0,2) != "OK" ) {
    std::stringstream s;
    s << "Error from module: " << answer;
    throw ( std::runtime_error( s.str() ) );
    return false;
  }
  return true;
}

void N1471::waitForUnlock( void ) {
  while( _n1471DeviceLocked == true ) {
    usleep( 50000 );
  }
  _n1471DeviceLocked = true;
}

