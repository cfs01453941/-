#ifndef TECH_H
#define TECH_H
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <algorithm>
#define LAYER 1
#define VIA 2
#define bot 0
#define mid 1
#define yop 2
using namespace std;

class tech_layer{
public:
    char NAME[10];
    char TYPE[10];
    int WIDTH ;
    int MAXWIDTH ;
    int SPACING ;
    int PITCH ;
    int OFFSET ;
    int layer;
    char DIRECTION[15] ;
    double RESISTANCE_RPERSQ ;
};

class tech_via{
public:
    char NAME[10];
    double RESISTANCE;
    char LAYERS[3][15];
    double RECT[3][4];
    int layer[2];
};
class tech{
public:
    tech();
    tech(FILE *);
    int Database_micron;
    double ManuFACTURINGGRID;
    vector<tech_layer> layer_list;
    vector<tech_via> via_list;
};
#endif
