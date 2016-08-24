// +--------------------------------------------------+
// | libAttiny+libN1471 constants                     |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V0.0                                             |
// | 12 november 2015                                 |
// +--------------------------------------------------+

namespace PN {
  double       alpha    = 1.0; // coefficients in the correction formula
  double       beta     = 0.0; //
  float        vdiffmin = 0.5; // volts. correct voltage only if DV>vdiffmin 
  unsigned int tsleep   = 10;  // time(seconds) between two measurements
  float        wuv      = 0.5; // % accuracy for initial ramp up
  
  float N1471_0_vset     = 4999.5;    // initial set voltage
  float N1471_0_iset     =   10.05;   // initial set current
  int   N1471_0_board    =    0;      // 1st voltage ch. (with corrected VSet)
  int   N1471_0_channel  =    0;
  
  float N1471_1_vset     = 1234.5;    // initial set voltage
  float N1471_1_iset     =    6.9;    // initial set current
  int   N1471_1_board    =    0;      // 2nd voltage ch. (constant VSet)
  int   N1471_1_channel  =    1;
};
