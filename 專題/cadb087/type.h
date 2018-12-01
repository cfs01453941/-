#ifndef TYPE_H
#define TYPE_H
#include "block.h"
#include "Input_DEF.h"
#include <math.h>
class sp_line{
    public :
        int layer;
        int type; ///1 up 2 down 3 left 4 right
        coordinate begin;
        coordinate end;
        double resistance;
        double length;
};
class sp_via{
    public :
        int layer_s, layer_d;
        coordinate coor;
        double resistance;
};
class VIA_o{
public:
    string type;
    coordinate lb,rt;
    double resistance;
    int layer_d,layer_s;
};
class LINE_o{
public:
	int type; ///Vertical LINE:1  Horizontal LINE:2
    coordinate begin, end;
    double length, width, resistance;
    int layer;
};

class ELEMENT_o{
public:
    string type;
    LINE_o line;
    VIA_o via;
    int number;
};

class PATH_o{
public:
    vector<ELEMENT_o> element_list;
    component_pin pin;
	initial_files_Pin pin_destination;
	string pin_name_on_comp;
    string target_comp_name;
    sp_line sp_last;
};

class NET_o{
public:
    vector<PATH_o> path_list;
    double voltage;
    Pins src;
};

class Node{
    public :
        int layer;
        int x;
        int y;
        double cost;
        int dir;
    private :
};

class USAGE{
public:
    int usage_type; ///Vertical LINE:1  Horizontal LINE:2 VIA:3
    int src_layer,end_layer;
    double R;
    coordinate src, end;
};

class Stepping_stone{
    public :
        int layer;
        double x;
        double y;
};
class LINE{
public:
    coordinate src;
    coordinate macro_lb;
	coordinate macro_rt;
    vector<pin_metal> end;
    double drop,current,width,length;
    vector<USAGE> usage_list;
    vector<Stepping_stone> node_list;
    string name;
    component_pin pin_name_on_comp;
    string target_comp_name;
    initial_files_Pin target_pin;
    int number,line_number; ///number = belong to # net
                            ///line_number = individual number
};

class NET{
public:
    vector<LINE> line_list;
    double length;
    double voltage;
    Pins src;
    int NET_number;
};

class TEMP_CON{
public:
    int layer;
    double x,y;
};

class int_coord{
public :
    int x;
    int y;
};

class VIA_Group{
public:
    double r;
    string name;
    int d,s;
};

#endif
