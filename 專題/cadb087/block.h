#ifndef BLOCK_TEST_H
#define BLOCK_TEST_H
#include <iostream>
#include <iostream>
#include<fstream>
#include <istream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string.h>
#include <cstdlib>

using namespace std;

class coordinate{
public:
    double x;
    double y;
    void Rotate(double[] );
};


class Metal{
public:
    int layer;
    string rect;
    coordinate coordinate_lb, coordinate_rt;
};

class _size{
public:
    double width;
    double height;
};

class Pin{
public:
    string name;
    string direction;   // {INPUT | OUTPUT | INOUT | FEEDTHRU}
    string use; // {SIGNAL | POWER | GROUND | CLOCK | TIEOFF | ANALOG}
    vector<Metal> metal;
    double current;

};

class Block{
public:
    int num;
    string _class;
    coordinate origin;

    _size Size;
    typedef struct SYMMETRY{
        string x;
        string y;
        string ang;
    }_symmetry;
    _symmetry symmetry;
    vector<Pin> pin;
    vector<Metal> metal;

    coordinate center;

    void print_result();


};

class block_Block{
public:
    block_Block(string);
    vector<string> cut_line(char*);
    void print();
    vector<Block> getBlock();
private:
    vector<Block> block_type;
};


#endif
