// +--------------------------------------------------+
// | libAttiny+libN1471 test program                  |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V1.1                                             |
// | 13 november 2015                                 |
// +--------------------------------------------------+

#include <iostream>
#include <iomanip>
#include <bitset>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <string>
#include <sstream>
#include <poll.h>
#include "init.h"
#include "N1471.h"
#include "constants.h"

N1471* n1471_0 = 0;
N1471* n1471_1 = 0;

float rampup0 = 0;
float rampdown0 = 0; 

bool _correctVoltageFirstCall = true;
double t_old = 0;
double p_old = 0;
float  vset_old = 0;
float  vmax = 0;
float  vmon = 0;
float  imon = 0;
float  vset = 0;
float  iset = 0;

void theUsage( char* cp ) {
    std::cout << 
"\n\
usage: " << cp << " [-h] [-a alpha] [-A beta]\n\
                    [-b board0] [-B board1]\n\
                    [-c channel0] [-C channel1]\n\
                    [-d vdiffmin]\n\
                    [-i ch0ISet] [-I ch1ISet]\n\
                    [s tsleep]\n\
                    [-v ch0ISet] [-V ch1VSet]\n\n\
     h             : print this help message\n\
     a alpha       : alpha coeff. in formula\n\
     A beta        : beta coeff. in formula\n\
     b board0      : 1st N1471 device id\n\
     B board1      : 2nd N1471 device id\n\
     c channel0    : 1st N1471 channel id (PT corrected)\n\
     C channel1    : 2nd N1471 channel id (constant)\n\
     d vdiffmin    : DeltaVmin. Correct only if |DeltaV| > |DeltaVmin|\n\
     i ch0ISet     : ISet for channel0\n\
     I ch1ISet     : ISet for channel1\n\
     p wuv         : Wait until abs(vset-vmon)/vmon < wuv\n\
     s tsleep      : DeltaT between PT measurements\n\
     v ch0VSet     : VSet for channel0\n\
     V ch1VSet     : VSet for channel1\n\
\n";

}

void theOptions( int argc, char **argv ) {
  int         rc;
  char        *cp;
  std::string ch = "";

  if(( cp = (char*) strrchr( argv[0], '/' )) != NULL ) {
    ++cp;
  }
  else {
    cp = argv[0];
  }

  if( argc > 1 ) {
    while(( rc = getopt( argc, argv, "ha:A:b:B:c:C:d:i:I:p:s:v:V:" )) != -1 ) {
      switch( rc ) {
      case 'h':
        theUsage(cp);
        exit(0);
        break;
      case 'a':
	PN::alpha = atoi(optarg);
        break;
      case 'A':
	PN::beta = atoi(optarg);
        break;
      case 'b':
	PN::N1471_0_board = atoi(optarg);
        break;
      case 'B':
	PN::N1471_1_board = atoi(optarg);
        break;
      case 'c':
	PN::N1471_0_channel = atoi(optarg);
        break;
      case 'C':
	PN::N1471_1_channel = atoi(optarg);
        break;
      case 'd':
	PN::vdiffmin = atof(optarg);
        break;
      case 'i':
	PN::N1471_0_iset = atof(optarg);
        break;
      case 'I':
	PN::N1471_1_iset = atof(optarg);
        break;
      case 'p':
	PN::wuv = atof(optarg);
        break;
      case 's':
	PN::tsleep = atoi(optarg);
        break;
      case 'v':
	PN::N1471_0_vset = atof(optarg);
        break;
      case 'V':
	PN::N1471_1_vset = atof(optarg);
        break;
      case '?':
        return;
        break;
      default:
        break;
      }
    }  
  }

  std::cout << "\033[1;33;40mPreset values\033[0m" << std::endl
            << "alpha           " << PN::alpha << std::endl
	    << "beta            " << PN::beta << std::endl
	    << "vdiffmin        " << PN::vdiffmin << std::endl
	    << "tsleep          " << PN::tsleep << std::endl
	    << "wuv             " << PN::wuv << std::endl
	    << "N1471_0_board   " << PN::N1471_0_board << std::endl
    	    << "N1471_0_channel " << PN::N1471_0_channel << std::endl
    	    << "N1471_0_iset    " << PN::N1471_0_iset << std::endl
    	    << "N1471_0_vset    " << PN::N1471_0_vset << std::endl
	    << "N1471_1_board   " << PN::N1471_1_board << std::endl
    	    << "N1471_1_channel " << PN::N1471_1_channel << std::endl
    	    << "N1471_1_iset    " << PN::N1471_1_iset << std::endl
    	    << "N1471_1_vset    " << PN::N1471_1_vset << std::endl
            << std::endl;

}

void getN1471Status( N1471* n1471 ) {
  unsigned int n1471status = 0;
  bool status = n1471->getStatus( PN::N1471_0_board, PN::N1471_0_channel, n1471status );
  if( status ) {
    std::cout << "\033[33m";
    if( (n1471status)&1 == 1 ) { std::cout << "Channel ON" << std::endl; }
    else if( (n1471status)&1 == 1 ) { std::cout << "Channel OFF"<< std::endl; }
    if( (n1471status>>1)&1 == 1 ) std::cout << "Channel Ramp UP" << std::endl;
    if( (n1471status>>2)&1 == 1 ) std::cout << "Channel Ramp DOWN" << std::endl;
    if( (n1471status>>3)&1 == 1 ) std::cout << "Imon > Iset" << std::endl;
    if( (n1471status>>4)&1 == 1 ) std::cout << "Vmon > Vset + 250V" << std::endl;
    if( (n1471status>>5)&1 == 1 ) std::cout << "Vmon < Vset - 250V" << std::endl;
    if( (n1471status>>6)&1 == 1 ) std::cout << "Vout in MaxV protection" << std::endl;
    if( (n1471status>>7)&1 == 1 ) std::cout << "Ch OFF via TRIP (Imon >= Iset during TRIP)" << std::endl;
    if( (n1471status>>8)&1 == 1 ) std::cout << "Power Max; Power Out > 1.7W" << std::endl;
    if( (n1471status>>9)&1 == 1 ) std::cout << "Temp > 105 deg. C" << std::endl;
    if( (n1471status>>10)&1 == 1 ) std::cout << "Ch disabled (REMOTE mode and switch on OFF position)" << std::endl;
    if( (n1471status>>11)&1 == 1 ) std::cout << "Ch in KILL via front panel" << std::endl;
    if( (n1471status>>12)&1 == 1 ) std::cout << "Ch in INTERLOCK via front panel" << std::endl;
    if( (n1471status>>13)&1 == 1 ) std::cout << "Calibration error" << std::endl << std::endl;
    std::cout << "\033[0m";
  }
  else {
    std::cout << std::endl << "\033[31mError in getting N1471 status\033[0m" << std::endl << std::endl;
  }	
}

void correctVoltage( void ) {

  // Here
  // DeltaV = PN::alpha * ( DeltaT/T(K) - DeltaP/P(Pa) ) + PN::beta;
  //
  
  if( _correctVoltageFirstCall ) {
    _correctVoltageFirstCall = false;
    _adts[0]->readTemperature( _attinys[0], t_old );
    _ms56s[0]->getPressure( _attinys[0], p_old );
    vset_old = vset;
    return;
  }
  else{
    double t = 0;
    _adts[0]->readTemperature( _attinys[0], t );
    t += 273.15; // Temperature is in Kelvin
    double p = 0;
    _ms56s[0]->getPressure( _attinys[0], p ); // Pressure is in Pascal
    float v = vset * ( 1. + PN::alpha * (( p - p_old ) / p_old - ( t - t_old ) / t_old ) + PN::beta );
    float vdiff = fabs(v-vset);
    if( v > 0 && v < vmax && vmax > 0 && vdiff > PN::vdiffmin ) {
      vset_old = vset;
      vset = v;
    }
    p_old = p;
    t_old = t;

    time_t now;
    struct tm * timedate;
    char timebuffer[80];
    time( &now );
    timedate = localtime( &now );
    strftime( timebuffer, 80, "%Y/%m/%d %H:%M:%S", timedate ); 

    std::cout << std::endl << "\033[1;34mHV Voltage (P,T) Correction\033[0m" << std::endl;
    std::cout << "Date/time:" << timebuffer << std::endl;
    if( n1471_0->getVMon( PN::N1471_0_board, PN::N1471_0_channel, vmon ) &&
	n1471_0->getIMon( PN::N1471_0_board, PN::N1471_0_channel, imon ) ) {
      std::cout << "\033[32mVmon: " << vmon << ", Imon: " << imon << "\033[0m" << std::endl;
    }
    std::cout << "T: " << std::fixed << std::setprecision(2) << t << " K,    " 
	      << "P: " << p_old/100. << " hP" << std::endl;
    std::cout << std::setprecision(6); std::cout.unsetf( std::ios_base::floatfield ); 
    if( vdiff > PN::vdiffmin ) {
      bool status = n1471_0->getRUp( PN::N1471_0_board, PN::N1471_0_channel, rampup0 );
      status = n1471_0->getRDwn( PN::N1471_0_board, PN::N1471_0_channel, rampdown0 );
      status = n1471_0->setRUp( PN::N1471_0_board, PN::N1471_0_channel,  1 );
      status = n1471_0->setRDwn( PN::N1471_0_board, PN::N1471_0_channel, 1 );
      float ru = 0, rd = 0;
      status = n1471_0->getRUp( PN::N1471_0_board, PN::N1471_0_channel, ru );
      status = n1471_0->getRDwn( PN::N1471_0_board, PN::N1471_0_channel, rd );
      if( ru != 1 || rd != 1 ) {
	std::cout << std::endl << "\033[31mError in setting maximum RampUp and/or RampDown voltage\033[0m" << std::endl << std::endl;
	exit(1);
      }

      std::cout << "\033[1;35mSetting Vset to " << vset << "V\033[0m" << std::endl;
      status = n1471_0->setVSet( PN::N1471_0_board, PN::N1471_0_channel, vset );
      if( !status ) {
	std::cout << std::endl << "\033[31mError in setting Vset\033[0m" << std::endl << std::endl;
      }
      float vset_verify;
      status = n1471_0->getVSet( PN::N1471_0_board, PN::N1471_0_channel, vset_verify );
      if( !status ) {
	std::cout << std::endl << "\033[31mError in getting Vset\033[0m" << std::endl << std::endl;
      }
      std::cout << "Vset is now " << vset_verify << " V" << std::endl;

      std::cout << "Waiting end of ramp up/down operations " << std::endl;
      unsigned int nstat = -1;
      do {
	status = n1471_0->getStatus( PN::N1471_0_board, PN::N1471_0_channel, nstat );
	usleep( 200000 );
      } while( (nstat>>1)&1 != 0 && (nstat>>2)&1 != 0);
      
      status = n1471_0->setRUp( PN::N1471_0_board, PN::N1471_0_channel,  rampup0 );
      status = n1471_0->setRDwn( PN::N1471_0_board, PN::N1471_0_channel, rampdown0 );
      status = n1471_0->getRUp( PN::N1471_0_board, PN::N1471_0_channel, ru );
      status = n1471_0->getRDwn( PN::N1471_0_board, PN::N1471_0_channel, rd );
      if( ru !=  rampup0 || rd != rampdown0 ) {
	std::cout << std::endl << "\033[31mError in setting maximum RampUp and/or RampDown voltage\033[0m" << std::endl << std::endl;
	exit(1);
      }

    }

    usleep( 500000 );
    unsigned int n1471status = 0;
    std::cout << std::endl << "\033[2;33mN1471_0 Channel Status\033[0m" << std::endl;
    getN1471Status( n1471_0 );
      
    if( n1471_0->getVMon( PN::N1471_0_board, PN::N1471_0_channel, vmon ) &&
	n1471_0->getIMon( PN::N1471_0_board, PN::N1471_0_channel, imon ) ) {
      std::cout << "\033[32mVmon: " << vmon << ", Imon: " << imon << "\033[0m" << std::endl;
    }
  }
}

void endProgram( void ) { 

  std::string pw = "";
  int status = 0;
  n1471_0->setPw( PN::N1471_0_board, PN::N1471_0_channel, "OFF" );
  usleep( 100000 );
  n1471_0->getPw( PN::N1471_0_board, PN::N1471_0_channel, pw );
  std::cout << "Channel: " << PN::N1471_0_channel << ", Pw: " << pw << std::endl;
  std::cout << std::endl << "\033[2;33mN1471_0 Channel Status\033[0m" << std::endl;
  getN1471Status( n1471_0 );
  status = n1471_0->disconnect();
  if( status ) {
    std::cout << "Channel " << PN::N1471_0_channel << " disconnect status: OK" << std::endl;
  }
  else {
    std::cout << "\033[1;31mChannel " << PN::N1471_0_channel << " disconnect status: NOT OK\033[0m" << std::endl;
  }
  
  n1471_1->setPw( PN::N1471_1_board, PN::N1471_1_channel, "OFF" );
  n1471_1->getPw( PN::N1471_1_board, PN::N1471_1_channel, pw );
  std::cout << "Channel: " << PN::N1471_1_channel << ", Pw: " << pw << std::endl;
  std::cout << std::endl << "\033[2;33mN1471_1 Channel Status\033[0m" << std::endl;
  getN1471Status( n1471_1 );
  status = n1471_1->disconnect();
  if( status ) {
    std::cout << "Channel " << PN::N1471_1_channel << " disconnect status: OK" << std::endl;
  }
  else {
    std::cout << "\033[1;31mChannel " << PN::N1471_1_channel << " disconnect status: NOT OK\033[0m" << std::endl;
  }
  std::cout << std::endl << "\033[1;35;40m  That's all folks!  \033[0m" << std::endl << std::endl;
  exit( 0 );
  
}

int main( int argc, char **argv ) {

    std::cout << "\033[1;32;40m    You are -more or less- welcome to this insipid, stupid program    \033[0m"
	      << std::endl << std::endl;
  
  theOptions( argc, argv );

  try {
    
    // -----------N1471_0------------------------------------------

    std::cout << "\033[1;34mConnecting to N1471_0...\033[0m" << std::endl; 
    n1471_0  = new N1471;
    bool status = n1471_0->connectBoard( PN::N1471_0_board );
    std::cout << "N1471_0 connection status: " << status << std::endl;

    n1471_0->setVSet( PN::N1471_0_board, PN::N1471_0_channel, PN::N1471_0_vset );
    n1471_0->setISet( PN::N1471_0_board, PN::N1471_0_channel, PN::N1471_0_iset );
    n1471_0->setPw( PN::N1471_0_board, PN::N1471_0_channel, "ON" );
    std::cout << "Initial N1471_0 setup: VSet = " << PN::N1471_0_vset
	      << ", ISet = " << PN::N1471_0_iset << std::endl;
    vset = PN::N1471_0_vset;
    
    // check
    std::string pw, pdwn;
    unsigned int alarms = 0;
    n1471_0->getVSet( PN::N1471_0_board, PN::N1471_0_channel, vset );
    n1471_0->getISet( PN::N1471_0_board, PN::N1471_0_channel, iset );
    std::cout << "Channel: " << PN::N1471_0_channel << ", read VSet: " << vset
	      << ", read ISet: " << iset << std::endl;
    n1471_0->getMaxV( PN::N1471_0_board, PN::N1471_0_channel, vmax );
    std::cout << "Channel: " << PN::N1471_0_channel << ", MaxV: " << vmax << std::endl;
    n1471_0->getPw( PN::N1471_0_board, PN::N1471_0_channel, pw );
    std::cout << "Channel: " << PN::N1471_0_channel << ", Pw: " << pw << std::endl;
        if( pw != "ON" ) {
      std::cout  << "\033[1;31m Channel " << PN::N1471_0_channel << " is not ON! \033[0m" << std::endl;
      exit(1);
    }
    n1471_0->getPDwn( PN::N1471_0_board, PN::N1471_0_channel, pdwn );
    std::cout << "Channel: " << PN::N1471_0_channel << ", PDwn: " << pdwn << std::endl;
    n1471_0->getAlarms( PN::N1471_0_board, alarms );
    std::cout << "Channel: " << PN::N1471_0_channel << ", Alarms: " << alarms << std::endl;
    std::cout << std::endl << "\033[2;33mN1471_0 Channel Status\033[0m" << std::endl;
    getN1471Status( n1471_0 );
    
    // -----------N1471_1------------------------------------------

    std::cout << "\033[1;34mConnecting to N1471_1...\033[0m" << std::endl; 
    n1471_1  = new N1471;
    status = n1471_1->connectBoard( PN::N1471_1_board );
    std::cout << "N1471_1 connection status: " << status << std::endl;

    n1471_1->setVSet( PN::N1471_1_board, PN::N1471_1_channel, PN::N1471_1_vset );
    n1471_1->setISet( PN::N1471_1_board, PN::N1471_1_channel, PN::N1471_1_iset );
    n1471_1->setPw( PN::N1471_1_board, PN::N1471_1_channel, "ON" );
    std::cout << "Initial N1471_1 setup: VSet = " << PN::N1471_1_vset
	      << ", ISet = " << PN::N1471_1_iset << std::endl;
    
    // check...
    float iset1 = 0, vset1 = PN::N1471_1_vset;
    n1471_1->getVSet( PN::N1471_1_board, PN::N1471_1_channel, vset1 );
    n1471_1->getISet( PN::N1471_1_board, PN::N1471_1_channel, iset1 );
    std::cout << "Channel: " << PN::N1471_1_channel << ", read VSet: " << vset1
	      << ", read ISet: " << iset1 << std::endl;
    n1471_1->getMaxV( PN::N1471_1_board, PN::N1471_1_channel, vmax );
    std::cout << "Channel: " << PN::N1471_1_channel << ", MaxV: " << vmax << std::endl;
    n1471_1->getPw( PN::N1471_1_board, PN::N1471_1_channel, pw );
    std::cout << "Channel: " << PN::N1471_1_channel << ", Pw: " << pw << std::endl;
    if( pw != "ON" ) {
      std::cout  << "\033[1;31m Channel " << PN::N1471_1_channel << " is not ON! \033[0m" << std::endl;
      exit(1);
    }
    n1471_1->getPDwn( PN::N1471_1_board, PN::N1471_1_channel, pdwn );
    std::cout << "Channel: " << PN::N1471_1_channel << ", PDwn: " << pdwn << std::endl;
    n1471_1->getAlarms( PN::N1471_1_board, alarms );
    std::cout << "Channel: " << PN::N1471_1_channel << ", Alarms: " << alarms << std::endl;
    std::cout << std::endl << "\033[2;33mN1471_1 Channel Status\033[0m" << std::endl;
    getN1471Status( n1471_1 );
    
    // wait for runp up...
    
    float vmon0 = 0, imon0 = 0, vmon1 = 0, imon1 = 0;
    std::cout << "\033[1;34mWaiting for N1471_0 and N1471_1 ramp up...\033[0m" << std::endl;
    do {
      sleep( 2 );
      n1471_0->getVMon( PN::N1471_0_board, PN::N1471_0_channel, vmon0 );
      n1471_0->getIMon( PN::N1471_0_board, PN::N1471_0_channel, imon0 );
      n1471_1->getVMon( PN::N1471_1_board, PN::N1471_1_channel, vmon1 );
      n1471_1->getIMon( PN::N1471_1_board, PN::N1471_1_channel, imon1 );
      std::cout << "Channel: " << PN::N1471_0_channel << ", VMon: " << vmon0 
		<<", IMon: " << imon0 << std::endl;
      std::cout << "Channel: " << PN::N1471_1_channel << ", VMon: " << vmon1 
		<<", IMon: " << imon1 << std::endl;
    } while( (fabs( vmon0 - PN::N1471_0_vset )/ PN::N1471_0_vset*100 > PN::wuv ) ||
	     (fabs( vmon1 - PN::N1471_1_vset )/ PN::N1471_1_vset*100 > PN::wuv ) ); // best than wuv%
    std::cout << std::endl << "\033[2;33mN1471_0 Channel Status\033[0m" << std::endl;
    getN1471Status( n1471_0 );
    std::cout << std::endl << "\033[2;33mN1471_1 Channel Status\033[0m" << std::endl;
    getN1471Status( n1471_1 );
    

    // -----------Sensors-----------------------------------------

    std::cout << "\033[1;34mSensors init and check...\033[0m" << std::endl;
    init();

    double temp = 0;
    _adts[0]->readTemperature( _attinys[0], temp );
    std::cout << "ADT7420 Temperature:  " << std::fixed << std::setprecision(2) << temp << std::endl;

    _ms56s[0]->getTemperature( _attinys[0], temp );
    temp += 273.15;
    std::cout << "MS56xx  Temperature:  " << std::fixed << std::setprecision(2) << temp << std::endl;
    double pres = 0;
    _ms56s[0]->getPressure( _attinys[0], pres );
    std::cout << "MS56xx     Pressure: " << pres/100. << std::endl;

    // ----------- correct voltage --------------------------------
 
    while( true ) {
      correctVoltage();
      std::cout << "\033[1;33;44mSleeping " <<  PN::tsleep
		<< " seconds. Press any key to exit\033[0m" << std::endl;
      std::stringstream command; command << "/usr/bin/read -n1 -r -t " << PN::tsleep;
      int sysStatus = system( command.str().c_str() );
      if( sysStatus == 0 ) endProgram();
    }
      
  } catch( std::runtime_error &e ) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  return 0;

}
