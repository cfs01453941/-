#ifndef FUNCTION_H
#define FUNCTION_H
#include <iostream>
#include<fstream>
#include <istream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string.h>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include "block.h" //for coordinate
#include <cmath>

#define SIZE 100000

class map_point{
public:
    int layer;
    coordinate position;
};

double distance_of_tow_point( coordinate p1, coordinate p2 );


#endif
