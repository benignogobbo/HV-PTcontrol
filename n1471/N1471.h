#ifndef N1471_H
#define N1471_H

// +--------------------------------------------------+
// | N1471 wrapper library                            |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V1.1                                             |
// | 18 November 2015                                 |
// +--------------------------------------------------+

#include <stdexcept>
#include <termios.h>
#include <stdint.h>

// +-------------+
// | Class N1471 |
// +-------------+
class N1471 {

 public:

  N1471( void );
  ~N1471( void ) {}

  bool connectBoard( int board ) throw( std::runtime_error );

  bool setVSet( int board, int channel, float voltage ) throw( std::runtime_error );

  bool setISet( int board, int channel, float current ) throw( std::runtime_error );

  bool setRUp( int board, int channel, float vps ) throw( std::runtime_error );

  bool setRDwn( int board, int channel, float vps ) throw( std::runtime_error );

  bool setTrip( int board, int channel, float seconds ) throw( std::runtime_error );

  bool setMaxV( int board, int channel, float voltage ) throw( std::runtime_error );

  bool setPDwn( int board, int channel, std::string RAMP_KILL ) throw( std::runtime_error );

  bool setIMRange( int board, int channel, std::string LOW_HIGH ) throw( std::runtime_error );

  bool setPw( int board, int channel, std::string ON_OFF ) throw( std::runtime_error );

  bool getStatus( int board, int channel, uint32_t &status )  throw( std::runtime_error );

  bool getVSet( int board, int channel, float &vset )  throw( std::runtime_error );

  bool getVMon( int board, int channel, float &vmon )  throw( std::runtime_error );

  bool getISet( int board, int channel, float &iset )  throw( std::runtime_error );

  bool getIMax( int board, int channel, float &imax )  throw( std::runtime_error );

  bool getIMin( int board, int channel, float &imin )  throw( std::runtime_error );

  bool getIMon( int board, int channel, float &imon )  throw( std::runtime_error );

  bool getIMRange( int board, int channel, std::string &imrange )  throw( std::runtime_error );

  bool getMaxV( int board, int channel, float &maxv )  throw( std::runtime_error );

  bool getRUp( int board, int channel, float &rup )  throw( std::runtime_error );

  bool getRDwn( int board, int channel, float &rdwn )  throw( std::runtime_error );

  bool getTrip( int board, int channel, float &trip )  throw( std::runtime_error );

  bool getPDwn( int board, int channel, std::string &pdwn )  throw( std::runtime_error );

  bool getPolarity( int board, int channel, std::string &polarity )  throw( std::runtime_error );

  bool getPw( int board, int channel, std::string &pw )  throw( std::runtime_error );

  bool getAlarms( int board, unsigned int &alarms ) throw( std::runtime_error );

  bool clearAlarms( int board ) throw( std::runtime_error );

  bool disconnect( void ) throw( std::runtime_error );

  void waitForUnlock( void );

  inline void unlock() { _n1471DeviceLocked = false; }

  // to be removed
  bool test( void );

 private:

  static bool _n1471DeviceLocked;
  char    _buff[512];
  int     _fd;
  termios _oldtio;
  termios _newtio;
  bool    _vok[4];
  bool    _iok[4];

  bool setFloat( int board, int channel, std::string variable, float min, float max, float value, float format ) 
    throw( std::runtime_error );
  bool setOnOff( int board, int channel, std::string variable, std::string on, std::string off, std::string value ) 
    throw( std::runtime_error );
  bool getFloat( int board, int channel, std::string variable, float &value )  throw( std::runtime_error );
  bool getOnOff( int board, int channel, std::string variable, std::string on, std::string off, std::string &value ) 
    throw( std::runtime_error );

  std::string serialRead( void );
  bool checkAnswer( std::string &answer );

};


#endif // N1471_H
