#ifndef INITIAL_H
#define INITIAL_H
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
#define SIZE 100000

#define CURRENT 1
#define VOLTAGE 2
#define IR_DROP 3
#define WEIGHT 4
using namespace std;
class initial_files_Pin{
public:
    string pin_name;
    double current;
    double drop;
};

class initial_files_Block{
public:
    string name;
    vector<initial_files_Pin> pins;
};

typedef struct _power_src{
    string name;
    double src_v;
    struct _power_src *next;
}power_src;

typedef struct _layer{
    string name;
    double weight;
    struct _layer *next;
}layer;
class Initial_file{
public:
    Initial_file();
    Initial_file(string);
    vector<string> cut_line(char*);
    power_src* setPowerSrc(vector<string>);
    layer* setLayer(vector<string>);

    vector<initial_files_Block> get_initial_block();

private:
    vector<initial_files_Block> Block_list;
    power_src *Src_head,*Src_tail;
    layer *Metal_head,*Metal_tail;

};
#endif
