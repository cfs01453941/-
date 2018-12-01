#ifndef HEADER_H
#define HEADER_H
#include "tech.h"
#include "Input_DEF.h"
#include "initial_files.h"
#include "output.h"
#include "type.h"
#include <queue>
#include <limits>
#define MAP_SIZE 4000
#define LAYER_SIZE 7
#define V_LINE 1
#define H_LINE 2
#define BYVIA 3
#define BLOCKAGE_BLOCK 1
#define BLOCKAGE_PIN 2
#define BLOCKAGE_SRC 3

#define grid_size 10

int Layer;
double Height, Width;

int GOD_MAP[LAYER_SIZE][MAP_SIZE][MAP_SIZE];
int edge_cost[LAYER_SIZE][MAP_SIZE/grid_size][MAP_SIZE/grid_size];//up edge cost
int all_history[LAYER_SIZE][MAP_SIZE/grid_size][MAP_SIZE/grid_size];
int of_all_edge[LAYER_SIZE][MAP_SIZE/grid_size][MAP_SIZE/grid_size];
int mc[LAYER_SIZE][MAP_SIZE/grid_size][MAP_SIZE/grid_size];

vector <VIA_Group>  via_G;
class METAL_Group{
public:
    int layer;
    double r;
};
vector <METAL_Group> metal_G;
vector<NET> netlist;
vector<NET_o> net_output;
int pollutevoltage;
bool NET_COMP(const NET ,const NET );
bool LINE_COMP(const LINE,const LINE);
bool compare(const tech_layer ,const tech_layer );
int DOUBLE_if_Cons(double ,double);
int DOUBLE_if_ZERO(double);

#endif
