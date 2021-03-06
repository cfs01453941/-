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

#include "block.h" //for coordinate
#include "initial_files.h"

#define SIZE 100

using namespace std;

/*
typedef struct _coordinates{
    double x;
    double y;
}coordinates;
*/
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
    //int coordinateX;
    //int coordinateY;
    coordinate origin;

    coordinate lb;
    coordinate rt;

    _size Size;

    string orientation;

    vector<component_pin> pin_list;

    int layer_size; //*********

    //void set_lb(coordinate);
    //void set_rt(coordinate);

    int get_block_num();
    //void set_lb_rt(double width, double height, vector<component_pin>);
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
/*
    double coordinate_lb_x;
    double coordinate_lb_y;
    double coordinate_rt_x;
    double coordinate_rt_y;

    double coordinate_fixed_x;
    double coordinate_fixed_y;
*/
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
private:

    vector<Pins> Pin_list;
    double unit_length;
};
#endif
