#ifndef TECH_H
#define TECH_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define LAYER 1
#define VIA 2
#define bot 0
#define mid 1
#define top 2
typedef struct Layer{
    char NAME[10];
    char TYPE[10];
    int WIDTH ;
    int MAXWIDTH ;
    int SPACING ;
    int PITCH ;
    int OFFSET ;
    char DIRECTION[15] ;
    double RESISTANCE_RPERSQ ;
    struct Layer *next ;
}tech_layer;

typedef struct Via{
    char NAME[10];
    double RESISTANCE;
    char LAYERS[3][15];
    double RECT[3][4];
    struct Via *next;
}via;
class tech{
public:
    tech();
    tech(FILE *);
    tech_layer* search4layer(char*);
    via* search4via(char*);
    void Print();
private:
    int Database_micron;
    double ManuFACTURINGGRID;
    tech_layer *Head_layer ;
    tech_layer *Cur_layer ;
    tech_layer *Tail_layer ;
    via *Head_via;
    via *Cur_via;
    via *Tail_via;
};
#endif
