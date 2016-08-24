#ifndef init_H
#define init_H
// +--------------------------------------------------+
// | initialization                                   |  
// |                                                  |
// | Benigno Gobbo                                    |
// | (c) INFN Sezione di Trieste                      |
// | V1.0                                             |
// | 05 November 2015                                 |
// +--------------------------------------------------+

#include <vector>
#include <iostream>
#include <stdexcept>
#include "Attiny45.h"
#include "MS56xx.h"
#include "ADT7420.h"

extern std::vector<Attiny45*> _attinys;
extern std::vector<ADT7420*>  _adts;
extern std::vector<MS56xx*>   _ms56s;

void init( void );

#endif // init_H
