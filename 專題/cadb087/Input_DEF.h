#ifndef INPUT_DEF_H
#define INPUT_DEF_H
#include <iostream>
#include<fstream>
#include <istream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string.h>
#include <cstdlib>

#include "block.h"
#include "initial_files.h"

using namespace std;
class pin_metal{
public:
    int layer;
    coordinate lb;
    coordinate rt;
};

class component_pin{
public:
    string name;
    vector<pin_metal> metal;
    double current;
};

class Components{
public:
    string name;
    int block_num;
    coordinate origin;
    coordinate lb;
    coordinate rt;
    _size Size;
    string orientation;
    vector<component_pin> pin_list;

    int layer_size;
    int get_block_num();
    void set_lb_rt(double width, double height);
    void set_pin_position();
    void rotate_block();
};
class Pins{
public:
    string pinName;
    string netName;
    string direction;   // {INPUT | OUTPUT | INOUT | FEEDTHRU}
    string use; // {SIGNAL | POWER | GROUND | CLOCK | TIEOFF | ANALOG}
    int metal;
    coordinate lb, rt, fixed;
    string orientation;
};

class Input_def{
public:
    Input_def();
    Input_def(string);
    vector<Components> Comp_list;
    vector<string> cut_line(char*);
    void Print();
    void set_coordinate(vector<Block>);
    void set_pin(vector<Block>);
    void rotate_all_block(vector<Block>);
    void set_pin_current(vector<initial_files_Block>);
    vector<Pins> Pin_list;
    double unit_length, unit_width, unit_height;
};
#endif
