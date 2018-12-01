#include <iostream>
#include<fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "initial_files.h"
#include "tech.h"
#include "v_file.h"
#include "Input_DEF.h"
#include "block.h"
#include "header.h"
#include <iomanip>
#include <algorithm>
#define BLOCKAGE_USING 4
#define PIN_PREV 30
using namespace std;

struct cmpCost{
    bool operator() (Node const &a, Node const &b){ return a.cost > b.cost; }
};
struct cmpOf{
    bool operator() (Node const &a, Node const &b){
        return of_all_edge[a.layer][a.y][a.x] < of_all_edge[b.layer][b.y][b.x];
    }
};

struct cmpDrop{
    bool operator() (LINE const &a, LINE const &b){
       return a.length > b.length;
    }
};

void initial_Cost(int,double,double);
void SPFA(NET, LINE, int);
void init(NET, LINE, priority_queue <Node, vector<Node>, cmpCost>*);
void memsetAll(int ,double ,double);
void traceBack(NET* , LINE*);
void moveBack(Node &, NET, LINE);
void alocSpace();
int calcOverflow();
void pushOverFlowQueue(priority_queue <Node, vector<Node>, cmpOf>*);
void reroute();
void ripup_reroute(int &, int &, int &, int &);
Node move(Node  , int & , int & );
int hg(int &, int &,int &, int, int &);
double be();
double pg(int &, int &, int &,int , int &, int &);
bool searchingLine(LINE ,int ,int ,int );
int calculateNodeCost(int,int,int);
void detailRoute(tech tech_parser);
bool linecmp(sp_line , sp_line);

vector<vector<vector<vector<LINE> > > > pass_line_list;
int parent[LAYER_SIZE][MAP_SIZE/grid_size][MAP_SIZE/grid_size] = {};
int hi = 1;
double pass = 1;
double cb = 0.5, cr = 0.1,bg;
double k1 = 5, k2 = 1.5, k3 = 3;
double penalty ,alpha =1;
int times = 0;

void ripup(int &x, int &y, int &layer, int &dir, priority_queue <LINE, vector<LINE>, cmpDrop>*);

Node e = {-1, -1, -1, -1, -1};
Node tg = {-1, -1, -1, -1, -1};
int vc = 1,hc = 1,uc = 1;//vertical / hori / up constriant

double awayCost = 1;
int MAX_COST = 10000000;

fstream fpppp;
int steps;
string strName;

int main(int argc, char* argv[])
{
    //FILE* fp1 = fopen("blocks.lef","r");
    //Blocks blocks_parser(fp1);
    //string in("initial_files");
    Initial_file initial_parser(argv[5]);
    FILE* fp3 = fopen(argv[3],"r");
    //priority_queue <Node, vector<Node>, cmpCost> *qAq = new priority_queue <Node, vector<Node>, cmpCost>(); //originally named q
    //Initial_file initial_parser("case2/initial_files");
    //Initial_file initial_parser("case2/initial_files");

    //FILE* fp3 = fopen("case2/tech.lef","r");
    //FILE* fp3 = fopen("case2/tech.lef","r");
    tech tech_parser(fp3);
    sort(tech_parser.layer_list.begin(),tech_parser.layer_list.end(),compare);
    FILE* fp4 = fopen(argv[1],"r");
    char str[1000];
    char *pch;
    strcpy(str, argv[1]);
    char *delim2 = ".";
    pch = strtok(str,delim2);
    strcpy(str, pch);
    string case_name(str);
    // string case_name = "case1";
    //FILE* fp4 = fopen("case2/case2.v","r");
    //FILE* fp4 = fopen("case2/case2.v","r");
    V_file v_parser(fp4);
    string in_block(argv[4]);
    //string in_block("case2/blocks.lef");
    //string in_block("case2/blocks.lef");
    block_Block block_parser(in_block);
    string in1(argv[2]);
    //string in1("case2/case2_input.def");
    //string in1("case2/case2_input.def");
    Input_def input_parser(in1);
    input_parser.rotate_all_block(block_parser.getBlock());
    input_parser.set_coordinate( block_parser.getBlock() );
    input_parser.set_pin_current( initial_parser.get_initial_block() );


    //input_parser.Print();
    /// set GOD_MAP
    Layer = initial_parser.metal_layer.size();
    Height = input_parser.unit_height/input_parser.unit_length;
    Width = input_parser.unit_width/input_parser.unit_length;
    alocSpace();
    vector<Components>::iterator comp_i;
    for(comp_i=input_parser.Comp_list.begin(); comp_i!=input_parser.Comp_list.end(); comp_i++){
        for(int i = 0; i < comp_i->layer_size; i++){
            for(int j = comp_i->lb.y; j < comp_i->rt.y; j++){
                for(int k = comp_i->lb.x; k < comp_i->rt.x; k++){
                    GOD_MAP[i][j][k] = BLOCKAGE_BLOCK;
                }
            }
        }
        vector<component_pin>::iterator pin_i;
        vector<pin_metal>::iterator pin_metal_i;
        for(pin_i=comp_i->pin_list.begin();pin_i!=comp_i->pin_list.end();pin_i++){
            for(pin_metal_i=pin_i->metal.begin(); pin_metal_i != pin_i->metal.end(); pin_metal_i++){
                for(int j = pin_metal_i->lb.y; j < pin_metal_i->rt.y; j++){
                    for(int k = pin_metal_i->lb.x; k < pin_metal_i->rt.x; k++){
                        GOD_MAP[pin_metal_i->layer-1][j][k] = BLOCKAGE_PIN;
                    }
                }
            }
        }
    }
    initial_Cost(Layer,Height,Width);
    ///set NET_LIST
    vector<Pins>::iterator src_pin_i;
	int net_number = 0;
    for(src_pin_i = input_parser.Pin_list.begin(); src_pin_i != input_parser.Pin_list.end(); src_pin_i++){
        NET tempnet;
        tempnet.src = (*src_pin_i);
        tempnet.voltage = initial_parser.getPowerSrc(tempnet.src.netName)->src_v;
        BlockofV *tempblock = v_parser.v_head;
		tempnet.NET_number = net_number++;
		int line_number = 0;

        while(tempblock != NULL){
            for(int i = 0;i < tempblock->num_of_pins;i++){
                if(tempblock->pins[i] != tempnet.src.pinName.c_str()[tempnet.src.pinName.size()-1]-'0') continue;
                LINE templine;
                templine.src.x = tempnet.src.fixed.x;
                templine.src.y = tempnet.src.fixed.y;
				templine.number = tempnet.NET_number;
				templine.line_number = line_number++;
                string tempblock_str(tempblock->name);
                string pin_name;
                for(comp_i=input_parser.Comp_list.begin(); comp_i!=input_parser.Comp_list.end(); comp_i++){
                    if(!strcmp(comp_i->name.c_str(),tempblock->name)){
                        vector<component_pin>::iterator pin_i;
                        for(pin_i=comp_i->pin_list.begin();pin_i!=comp_i->pin_list.end();pin_i++){
                            if(pin_i->name.c_str()[pin_i->name.size()-1] - 'A' == i){
                                templine.end = pin_i->metal;
                                templine.macro_lb = comp_i->lb;
                                templine.macro_rt = comp_i->rt;
                                templine.pin_name_on_comp = *pin_i;
                                templine.target_comp_name = comp_i->name;
                                pin_name = pin_i->name;
                                break;
                            }
                        }
                        break;
                    }
                }
                initial_files_Pin temppin = initial_parser.getInitial_files_Pin(tempblock_str,pin_name);
                templine.target_pin = temppin;
                templine.name = temppin.pin_name;
                templine.current = temppin.current;
                templine.drop = temppin.drop;
                templine.width = 8;
                tempnet.line_list.push_back(templine);
                break;
            }
            tempblock = tempblock->next;
        }
        vector<LINE>::iterator line_i;
        double temp_length = 0.0;
        for(line_i = tempnet.line_list.begin(); line_i != tempnet.line_list.end(); line_i ++){
            vector<pin_metal>::iterator pin_metal_i;
            for(pin_metal_i = line_i->end.begin(); pin_metal_i != line_i->end.end(); pin_metal_i++){
               if(!DOUBLE_if_ZERO(pin_metal_i->lb.x) || !DOUBLE_if_ZERO(pin_metal_i->lb.y)){
                    line_i->length= sqrt(pow(fabs(pin_metal_i->lb.x-line_i->src.x),2)+pow(fabs(pin_metal_i->lb.y-line_i->src.y),2));
                    temp_length += line_i->length;
                }else continue;
                break;
            }
        }
        tempnet.length /= tempnet.line_list.size();
        sort(tempnet.line_list.begin(),tempnet.line_list.end(),LINE_COMP);
        netlist.push_back(tempnet);
    }



    sort(netlist.begin(),netlist.end(),NET_COMP);
    vector<NET>::iterator NET_i;
    vector<LINE>::iterator LINE_i;

    /// initial routing
    for (NET_i = netlist.begin(); NET_i !=netlist.end(); NET_i++){
        for(LINE_i = NET_i->line_list.begin(); LINE_i != NET_i->line_list.end(); LINE_i++){
            steps=0;
            SPFA(*NET_i,*LINE_i, 0);
            NET *net_ptr = &(*NET_i);
            LINE *line_ptr = &(*LINE_i);
            traceBack(net_ptr,line_ptr);
        }
    }

    ///rip up & re-route
	int overflow_number = calcOverflow();
	while(overflow_number > 0){
        times++;
        cout<<overflow_number<<" "<<times<<endl;
        pass += 1;
        bg = be();
        priority_queue <Node, vector<Node>, cmpOf> *rrPQ = new priority_queue <Node, vector<Node>, cmpOf>();

        priority_queue <LINE, vector<LINE>, cmpDrop> *reroutePQ = new priority_queue <LINE, vector<LINE>, cmpDrop>() ;// originally name reroutePQ


		pushOverFlowQueue(rrPQ);
		while(!(*rrPQ).empty()){
            int current_layer,current_x,current_y,current_dir;
            current_x = (*rrPQ).top().x;
            current_y = (*rrPQ).top().y;
            current_layer = (*rrPQ).top().layer;
            current_dir = (*rrPQ).top().dir;
            (*rrPQ).pop();
            if (of_all_edge[current_layer][current_y][current_x] == 0) continue;
            ripup(current_x,current_y,current_layer,current_dir,reroutePQ);
		}

		/// reroute
		while (!(*reroutePQ).empty()){
            LINE line = (*reroutePQ).top();
            (*reroutePQ).pop();

            vector<NET> ::iterator net_i;
            vector<LINE> ::iterator line_i;
            for(net_i = netlist.begin(); net_i != netlist.end(); net_i++){
                if(line.number == net_i->NET_number){
                    for(line_i = net_i->line_list.begin(); line_i != net_i->line_list.end(); line_i++){
                        if (line_i->line_number == line.line_number){
                            NET *n_ptr = &(*net_i);
                            LINE *l_ptr = &(*line_i);
                            SPFA(*n_ptr, *l_ptr, 1);
                            traceBack(n_ptr,l_ptr);
                            break;
                        }
                    }
                }
            }

        }
        *reroutePQ = priority_queue <LINE, vector<LINE>, cmpDrop>();
        delete reroutePQ;
		delete rrPQ;
		overflow_number = calcOverflow();
		cout << "overflow_number " << overflow_number << endl;
	}

    detailRoute(tech_parser);


    vector<NET_o>::iterator net_i;
    vector<PATH_o>::iterator path_i;
    vector<ELEMENT_o>::iterator element_i;
    vector<ELEMENT_o>::iterator temp;
    coordinate tp;

    /// METAL USAGE
    fstream file1;
    file1.open("output_files",ios::out);
    for(int i=0; i<LAYER_SIZE; i++){
        for(int j=0; j<MAP_SIZE; j++){
            for(int k=0; k<MAP_SIZE; k++){
                GOD_MAP[i][j][k]==0;
            }
        }
    }
    for(net_i=net_output.begin(); net_i!=net_output.end(); net_i++){
        for(path_i=net_i->path_list.begin(); path_i!=net_i->path_list.end(); path_i++){
            for(element_i=path_i->element_list.begin(); element_i!=path_i->element_list.end(); element_i++){
                if(element_i->type=="LINE"){
                    if(element_i->line.begin.x==element_i->line.end.x){
                        for(int a=min(element_i->line.begin.y,element_i->line.end.y); a<max(element_i->line.begin.y,element_i->line.end.y); a++)
                            for(int b=element_i->line.begin.x-element_i->line.width/2; b<element_i->line.begin.x+element_i->line.width/2; b++)
                                GOD_MAP[element_i->line.layer-1][a][b]=BLOCKAGE_USING;
                    }
                    else if(element_i->line.begin.y==element_i->line.end.y){
                        for(int a=min(element_i->line.begin.x,element_i->line.end.x); a<max(element_i->line.begin.x,element_i->line.end.x); a++)
                            for(int b=element_i->line.begin.y-element_i->line.width/2; b<element_i->line.begin.y+element_i->line.width/2; b++)
                                GOD_MAP[element_i->line.layer-1][b][a]=BLOCKAGE_USING;
                    }
                }
            }
        }
    }
    int M_U[10]={0,0,0,0,0,0,0,0,0,0};
    file1<<"# The metal usage report"<<endl;
    for(int i=0; i<Layer; i++){
        for(int j=0; j<(int)Height; j++){
            for(int k=0; k<(int)Width; k++){
                if(GOD_MAP[i][j][k]==BLOCKAGE_USING)
                    M_U[i]++;
            }
        }
    }
    double sum=0;
    for(int i=0; i<Layer; i++){
        sum += M_U[i]*initial_parser.metal_layer.at(i).weight;
        if(M_U[i]!=0){
            file1<<"M"<<i+1<<" "<<M_U[i]<<endl;
        }
    }
    file1 <<"Total "<<sum<<endl;

    /// SP
    fstream file;
    file.open(case_name+".sp",ios::out);
    vector<sp_line> spl_list;
    vector<sp_line>::iterator spl_i;
    sp_line spl;
    vector<sp_via> spv_list;
    vector<sp_via>::iterator spv_i;
    sp_via spv;

    file<<"# R=RESISTANCE RPERSQ* L/W"<<endl;
    for( net_i =net_output.begin() ; net_i != net_output.end() ; net_i++){
        int number=1;
        for(path_i=net_i->path_list.begin() ;path_i!=net_i->path_list.end();path_i++ ){
            spl_list.clear();
            spv_list.clear();
            tp.x=net_i->src.fixed.x/1000;
            tp.y=net_i->src.fixed.y/1000;

            /// record line and via
            for(element_i=path_i->element_list.begin();element_i!=path_i->element_list.end();element_i++){
                if(element_i->type=="LINE"){
                    spl.begin.x = tp.x;
                    spl.begin.y = tp.y;
                    spl.layer = element_i->line.layer;
                    if(element_i->line.begin.x==element_i->line.end.x){
                        spl.end.x = tp.x;
                        spl.end.y = element_i->line.end.y;
                        spl.length = fabs(spl.begin.y - spl.end.y);
                        spl.resistance = spl.length / element_i->line.width * tech_parser.layer_list.at(2*element_i->line.layer-2).RESISTANCE_RPERSQ;
                        tp.y = element_i->line.end.y;
                        if(spl.end.y > spl.begin.y)
                            spl.type = 1;
                        else if(spl.end.y < spl.begin.y)
                            spl.type = 2;
                    }
                    else{
                        spl.end.y = tp.y;
                        spl.end.x = element_i->line.end.x;
                        spl.length = fabs(spl.begin.x - spl.end.x);
                        spl.resistance = spl.length / element_i->line.width * tech_parser.layer_list.at(2*element_i->line.layer-2).RESISTANCE_RPERSQ;
                        tp.x = element_i->line.end.x;
                        if(spl.end.x < spl.begin.x)
                            spl.type = 3;
                        else if(spl.end.x > spl.begin.x)
                            spl.type = 4;

                    }
                    spl_list.push_back(spl);
                }
                else if(element_i->type=="VIA"){
                    spv.coor.x = tp.x;
                    spv.coor.y = tp.y;
                    spv.layer_d = element_i->via.layer_d;
                    spv.layer_s = element_i->via.layer_s;
                    spv.resistance = element_i->via.resistance;
                    int same=0;
                    for(spv_i=spv_list.begin(); spv_i!=spv_list.end(); spv_i++){
                        if(spv_i->coor.x == spv.coor.x && spv_i->coor.y == spv.coor.y &&
                           spv_i->layer_d == spv.layer_d && spv_i->layer_s == spv.layer_s
                           && spv_i->resistance == spv.resistance){
                               same=1;
                                break;
                        }
                    }
                    if(same==0){
                        spv_list.push_back(spv);
                    }
                }
            }
            path_i->sp_last = spl;

            /// split line
            sort(spl_list.begin(), spl_list.end(), linecmp);
            for(int i=0; i<spl_list.size(); i++){
                for(int j=i+1; j<spl_list.size(); j++){
                    if(spl_list[i].type==spl_list[j].type && spl_list[i].begin.x==spl_list[j].begin.x && spl_list[i].begin.y==spl_list[j].begin.y && spl_list[i].layer==spl_list[j].layer){
                        spl_list[i].begin.x = spl_list[j].end.x;
                        spl_list[i].begin.y = spl_list[j].end.y;
                        spl_list[i].length = fabs(spl_list[i].begin.x - spl_list[i].begin.y);
                        spl_list[i].resistance =  spl_list[j].resistance - spl_list[i].resistance;
                        break;
                    }
                }
            }
            for(int i=0; i<spl_list.size(); i++,number++){
                file<<setprecision(7)<<"R"<<net_i->src.pinName<<"_"<<number<<" M"<<spl_list[i].layer<<"_"<<spl_list[i].begin.x*1000<<"_"<<spl_list[i].begin.y*1000
                    <<"  M"<<spl_list[i].layer<<"_"<<spl_list[i].end.x*1000<<"_"<<spl_list[i].end.y*1000<<" "
                    <<spl_list[i].resistance<<endl;
            }
            for(int i=0; i<spv_list.size(); i++,number++){
                file<<setprecision(7)<<"R"<<net_i->src.pinName<<"_"<<number<<" M"<<spv_list[i].layer_s<<"_"<<spv_list[i].coor.x*1000<<"_"<<spv_list[i].coor.y*1000
                    <<"  M"<<spv_list[i].layer_d<<"_"<<spv_list[i].coor.x*1000<<"_"<<spv_list[i].coor.y*1000<<" "<<spv_list[i].resistance<<endl;
            }
        }
        file<<setprecision(7)<<"V_"<<net_i->src.pinName<<"_"<<"1"<<" "<<"M"<<net_i->src.metal<<"_"<<net_i->src.fixed.x<<"_"<<net_i->src.fixed.y<<" gnd "<<net_i->voltage<<endl;
        int index=1;
        for(path_i=net_i->path_list.begin(); path_i!=net_i->path_list.end(); path_i++){
            file <<setprecision(7)<< "I_" << net_i->src.pinName << "_" << index << " M" << path_i->sp_last.layer <<"_"<< path_i->sp_last.end.x*1000
            << "_" << path_i->sp_last.end.y*1000 << " gnd "<< (path_i->pin_destination.current)/1000 << endl;
            index++;
        }
        file<<endl;
    }
    file<<endl;
    file<<".tran 1ns 1ns"<<endl;
    file<<".end"<<endl;
    file.close();

    /// record current
    for(int i=0; i<LAYER_SIZE; i++){
        for(int j=0; j<MAP_SIZE; j++){
            for(int k=0; k<MAP_SIZE; k++){
                GOD_MAP[i][j][k]==0;
            }
        }
    }
    for( net_i =net_output.begin() ; net_i != net_output.end() ; net_i++){
        for(path_i=net_i->path_list.begin() ;path_i!=net_i->path_list.end();path_i++ ){
            tp.x=net_i->src.fixed.x/1000;
            tp.y=net_i->src.fixed.y/1000;
            for(element_i=path_i->element_list.begin();element_i!=path_i->element_list.end();element_i++){
                if(element_i->type=="VIA")
                    continue;
                if(element_i->line.begin.x==element_i->line.end.x){
                    for(int a = (int)min(tp.y,element_i->line.end.y); a<(int)max(tp.y,element_i->line.end.y); a++){
                        GOD_MAP[element_i->line.layer-1][a][(int)tp.x]+=path_i->pin_destination.current;
                    }
                    tp.y = element_i->line.end.y;
                }else if(element_i->line.begin.y==element_i->line.end.y){
                    for(int a = (int)min(tp.x,element_i->line.end.x); a<(int)max(tp.x,element_i->line.end.x); a++){
                        GOD_MAP[element_i->line.layer-1][(int)tp.y][a]+=path_i->pin_destination.current;
                    }
                    tp.x = element_i->line.end.x;
                }
            }
        }
    }

    /// IR drop report
    file1 <<endl;
    file1 <<"# The IR drop of each power pin (%)"<<endl;
    for( net_i =net_output.begin() ; net_i != net_output.end() ; net_i++){
        for(path_i=net_i->path_list.begin() ;path_i!=net_i->path_list.end();path_i++ ){
            tp.x=net_i->src.fixed.x/1000;
            tp.y=net_i->src.fixed.y/1000;
            file1<<path_i->target_comp_name<<"/"<<path_i->pin_name_on_comp<<" ";
            double vol_ub = net_i->voltage;
            double vol_sj = 0;
            for(element_i=path_i->element_list.begin();element_i!=path_i->element_list.end();element_i++){
                if(element_i->type=="LINE"){
                    if(element_i->line.begin.x==element_i->line.end.x){
                        for(int a = (int)min(tp.y,element_i->line.end.y); a<(int)max(tp.y,element_i->line.end.y); a++){
                            vol_sj += (double)(GOD_MAP[element_i->line.layer-1][a][(int)tp.x]/element_i->line.width*tech_parser.layer_list.at(2*element_i->line.layer-2).RESISTANCE_RPERSQ);
                        }
                        tp.y = element_i->line.end.y;
                    }else if(element_i->line.begin.y==element_i->line.end.y){
                        for(int a = (int)min(tp.x,element_i->line.end.x); a<(int)max(tp.x,element_i->line.end.x); a++){
                            vol_sj += (double)(GOD_MAP[element_i->line.layer-1][(int)tp.y][a]/element_i->line.width*tech_parser.layer_list.at(2*element_i->line.layer-2).RESISTANCE_RPERSQ);
                        }
                        tp.x = element_i->line.end.x;
                    }
                }else if(element_i->type=="VIA"){
                    vol_sj += (double)GOD_MAP[element_i->via.layer_d-1][(int)tp.y][(int)tp.x]*element_i->via.resistance;
                }
            }
            ///file1<<setprecision(2)<<((vol_sj/1000)/vol_ub)*100<<endl;
            file1<<fixed<<setprecision(2)<<((vol_sj/1000)/vol_ub)*100<<endl;
        }
    }
    file1<<endl;
    file1.close();

    output outputDef(net_output,initial_parser,case_name);

    return 0;
}

void detailRoute(tech tech_parser){
    vector<NET>::iterator net_i;
    for( net_i = netlist.begin(); net_i != netlist.end(); net_i++){
        vector <LINE> ::iterator line_i;
        NET_o temp_Net;
        temp_Net.src = net_i->src;
        temp_Net.voltage = net_i->voltage;
        for(line_i = net_i->line_list.begin(); line_i != net_i->line_list.end(); line_i++){
            vector<Stepping_stone>::iterator stone_i;
            vector<Stepping_stone> tempList;
            int dir = -1;
            Stepping_stone prev = line_i->node_list.front(),p;
            for(stone_i = line_i->node_list.begin()+1; stone_i != line_i->node_list.end(); stone_i++){
                p = *stone_i;
                int tempdir;
                if(p.x > prev.x) tempdir = 1;
                else if(p.x < prev.x) tempdir = 2;
                else if(p.y > prev.y) tempdir = 3;
                else if(p.y < prev.y) tempdir = 4;
                else if(p.layer > prev.layer) tempdir = 5;
                else if(p.layer < prev.layer) tempdir = 6;
                if(dir != tempdir || dir >= 5){
                    dir = tempdir;
                    tempList.push_back(prev);
                }
                prev = p;
            }
            if(tempList.back().x != line_i->node_list.back().x ||
               tempList.back().y != line_i->node_list.back().y ||
               tempList.back().layer != line_i->node_list.back().layer) tempList.push_back(line_i->node_list.back());

            vector<Stepping_stone>::reverse_iterator back_stone;
            PATH_o temp_path;
            temp_path.pin = line_i->pin_name_on_comp;
            temp_path.pin_destination = line_i->target_pin;
            temp_path.pin_name_on_comp = line_i->pin_name_on_comp.name;
            temp_path.target_comp_name = line_i->target_comp_name;
            prev = tempList.back();
            int turn=0;
            for(back_stone = tempList.rbegin()+1; back_stone!= tempList.rend(); back_stone++){
                p = *back_stone;
                int tempdir,end_diff = 0;
                if(p.x > prev.x) tempdir = 1;
                else if(p.x < prev.x) tempdir = 2;
                else if(p.y > prev.y) tempdir = 3;
                else if(p.y < prev.y) tempdir = 4;
                else if(p.layer > prev.layer) tempdir = 5;
                else if(p.layer < prev.layer) tempdir = 6;
                ELEMENT_o ele_i;
                Node target;
                if(back_stone == tempList.rend()-1){
                    target.x = (line_i->end.back().lb.x + line_i->end.back().rt.x) / 2 ;
                    target.y = (line_i->end.back().lb.y + line_i->end.back().rt.y) / 2 ;
                    end_diff = 1;
                }else if(back_stone == tempList.rbegin()+1) end_diff = -1;
                if(tempdir == 1){   //left to right
                    ele_i.line.width = line_i->width;
                    ele_i.type = "LINE";
                    if(end_diff == -1) ele_i.line.begin.x = prev.x*grid_size;
                    else ele_i.line.begin.x = prev.x*grid_size + 0.5*(grid_size - line_i->width);
                    ele_i.line.begin.y = prev.y*grid_size + grid_size/2;
                    if(end_diff == 1) ele_i.line.end.x = target.x;
                    else ele_i.line.end.x = p.x*grid_size + (line_i->width + grid_size)/2;
                    ele_i.line.end.y = prev.y*grid_size + grid_size/2;
                    ele_i.line.layer = prev.layer + 1;
                    ele_i.line.length = fabs(ele_i.line.begin.x - ele_i.line.end.x);
                    ele_i.line.type=2;
                }else if(tempdir == 2){  //right to left
                    ele_i.line.width = line_i->width;
                    ele_i.type = "LINE";
                    if(end_diff == -1) ele_i.line.begin.x = (prev.x + 1)*grid_size;
                    else ele_i.line.begin.x = prev.x*grid_size + 0.5*(line_i->width + grid_size);
                    ele_i.line.begin.y = prev.y*grid_size + grid_size/2;
                    if(end_diff == 1) ele_i.line.end.x = target.x;
                    else ele_i.line.end.x = p.x*grid_size + 0.5*(grid_size - line_i->width);
                    ele_i.line.end.y = prev.y*grid_size + grid_size/2;
                    ele_i.line.layer = prev.layer + 1;
                    ele_i.line.length = fabs(ele_i.line.begin.x - ele_i.line.end.x);
                    ele_i.line.type=2;
                }else if(tempdir == 3){  //low to high
                    ele_i.line.width = line_i->width;
                    ele_i.type = "LINE";
                    ele_i.line.begin.x = prev.x*grid_size + grid_size/2;
                    if(end_diff == -1) ele_i.line.begin.y = prev.y*grid_size;
                    else ele_i.line.begin.y = prev.y*grid_size + 0.5*(grid_size - line_i->width);
                    ele_i.line.end.x = p.x*grid_size + grid_size/2;
                    if(end_diff == 1) ele_i.line.end.y = target.y;
                    else ele_i.line.end.y = p.y*grid_size + 0.5*(line_i->width + grid_size);
                    ele_i.line.layer = prev.layer + 1;
                    ele_i.line.length = fabs(ele_i.line.begin.x - ele_i.line.end.x);
                    ele_i.line.type=1;
                }else if(tempdir == 4){ //high to low
                    ele_i.line.width = line_i->width;
                    ele_i.type = "LINE";
                    ele_i.line.begin.x = prev.x*grid_size + grid_size/2;
                    if(end_diff == -1)  ele_i.line.begin.y = (prev.y + 1)*grid_size ;
                    else ele_i.line.begin.y = prev.y*grid_size + 0.5*(grid_size + line_i->width);
                    ele_i.line.end.x = p.x*grid_size + grid_size/2;
                    if(end_diff == 1) ele_i.line.end.y = target.y;
                    else ele_i.line.end.y = p.y*grid_size + 0.5*(grid_size - line_i->width);
                    ele_i.line.layer = prev.layer + 1;
                    ele_i.line.length = fabs(ele_i.line.begin.x - ele_i.line.end.x);
                    ele_i.line.type=1;
                }else if(tempdir == 5 || tempdir == 6){
                    ele_i.type = "VIA";
                    ele_i.via.layer_s = prev.layer + 1;
                    ele_i.via.layer_d = p.layer + 1;
                    ///cout<<ele_i.via.layer_s<<" to "<<ele_i.via.layer_d<<endl;
                    char t3 = min(ele_i.via.layer_s,ele_i.via.layer_d)+'0';
                    string t1 = "via";
                    string t2 = "_B";
                    ele_i.via.type = t1+ t3 + t2;
                    ///cout<<"SO "<<ele_i.via.type<<"\n"<<endl;
                    vector<tech_via>::iterator via_o;
                    for(via_o=tech_parser.via_list.begin(); via_o!=tech_parser.via_list.end(); via_o++){
                        if(via_o->NAME == ele_i.via.type){
                            ele_i.via.resistance = via_o->RESISTANCE;
                            break;
                        }
                    }
                    ele_i.via.lb.x = prev.x*grid_size + 0.5*(grid_size - 5);
                    ele_i.via.lb.y = prev.y*grid_size + 0.5*(grid_size - 5);
                    ele_i.via.rt.x = ele_i.via.lb.x + 5;
                    ele_i.via.rt.y = ele_i.via.lb.y + 5;
                }
                temp_path.element_list.push_back(ele_i);
                prev = p;
                turn++;
            }
            temp_Net.path_list.push_back(temp_path);
        }
        net_output.push_back(temp_Net);
    }
}
bool NET_COMP(const NET a,const NET b){
    return a.length < b.length;
}
bool LINE_COMP(const LINE a,const LINE b){
    return a.length < b.length;
}
bool compare(const tech_layer a,const tech_layer b){
    return a.layer < b.layer;
}
int DOUBLE_if_Cons(double number1,double number2){
    int value;
    (fabs(number1 - number2) <= 1e-15)? value = 1:value =  0;
    return value;
}
int DOUBLE_if_ZERO(double number){
    int value;
    (fabs(number) <= 1e-15)? value = 1:value =  0;
    return value;
}
void initial_Cost(int layer,double height,double width){
    for(int i = 0; i <layer; i++){
        for(int j = 0; j < height/grid_size; j++){
            for(int k = 0 ; k < width/grid_size; k++){
                edge_cost[i][j][k] = 0;
                all_history[i][j][k] = 1;
                of_all_edge[i][j][k] = 0;
            }
        }
    }
    for(int i = 0; i < layer; i++){
        for(int j = 0; j < height; j += grid_size){
            for(int k = 0; k < width; k += grid_size){
                bool isBlock = false;
                for(int grid_x=0; grid_x<grid_size; grid_x++){
                    for(int grid_y=0; grid_y<grid_size; grid_y++){
                        if(GOD_MAP[i][j+grid_y][k+grid_x]==BLOCKAGE_BLOCK){
                            isBlock = true;
                            edge_cost[i][j/grid_size][k/grid_size] = MAX_COST;
                            break;
                        }
                    }
                    if(isBlock) break;
                }
            }
        }
    }
    for(int i = 0; i < layer; i++){
        for(int j = 0; j < height; j += grid_size){
            for(int k = 0; k < width; k += grid_size){
                bool isPin = false;
                for(int grid_x=0; grid_x<grid_size; grid_x++){
                    for(int grid_y=0; grid_y<grid_size; grid_y++){
                        if(GOD_MAP[i][j+grid_y][k+grid_x]==BLOCKAGE_PIN){
                            isPin = true;
                            all_history[i][j/grid_size][k/grid_size] = 30;
                            edge_cost[i][j/grid_size][k/grid_size] = 30;

                            break;
                        }
                    }
                    if(isPin) break;
                }
            }
        }
    }
    for(int i = 0; i < layer; i++){
        for(int j = 0; j < height / grid_size; j++){
            for(int k = 0; k < width / grid_size; k++){
                if(edge_cost[i][j][k] == MAX_COST){
                    if(j > 0 && edge_cost[i][j-1][k] != MAX_COST && edge_cost[i][j-1][k] != PIN_PREV) edge_cost[i][j-1][k] = 50;
                    if(j < height / grid_size && edge_cost[i][j+1][k] != MAX_COST && edge_cost[i][j+1][k] != PIN_PREV) edge_cost[i][j+1][k] = 50;
                    if(k > 0 && edge_cost[i][j][k-1] != MAX_COST && edge_cost[i][j][k-1] != PIN_PREV) edge_cost[i][j][k-1] = 50;
                    if(k < width / grid_size && edge_cost[i][j][k+1] != MAX_COST && edge_cost[i][j][k+1] != PIN_PREV) edge_cost[i][j][k+1] = 50;
                }
            }
        }
    }
}
void alocSpace(){
    pass_line_list.resize(Layer);
    for(int i = 0; i < Layer; i++){
        pass_line_list[i].resize(Height/grid_size);
    }
    for(int i = 0; i < Layer; i++){
        for(int j = 0; j < Height/grid_size; j++){
            pass_line_list[i][j].resize(Width/grid_size);
        }
    }
    return;
}
Node st;
void init(NET net, LINE line, priority_queue <Node, vector<Node>, cmpCost> *qAq ){
    while(!(*qAq).empty()){(*qAq).pop();}
    st = {net.src.metal-1, net.src.fixed.x/grid_size/1000, net.src.fixed.y/grid_size/1000, 0, -1};
    // target
    tg.x = (line.end.back().lb.x + line.end.back().rt.x) / 2 / grid_size;
    tg.y = (line.end.back().lb.y + line.end.back().rt.y) / 2 / grid_size;
    tg.layer = line.end.back().layer-1;
    tg.cost = -1;

    mc[st.layer][st.y][st.x] = 0;
    (*qAq).push(st);
    return;
}

void SPFA(NET net, LINE line, int reroute){
    memsetAll(Layer, Height, Width);
    priority_queue <Node, vector<Node>, cmpCost> *qAq = new priority_queue <Node, vector<Node>, cmpCost>();
    init(net, line, qAq);
    while (!(*qAq).empty()){
        Node n = (*qAq).top();
        (*qAq).pop();
        //int check = 0;
        if (n.x == tg.x && n.y == tg.y && n.layer == tg.layer) {
            if (tg.cost != -1 && n.cost >= tg.cost) break;
            tg.cost = n.cost;
            continue;
        }

        for (int j = 0; j < 6; j++){
            // N, E, S, W, U, D
            if(parent[n.layer][n.y][n.x] == -1 && j >= 4) continue;
            Node new_n = move(n, j, reroute);
            if (new_n.cost == -1 ||
                (mc[new_n.layer][new_n.y][new_n.x] != -1 &&
                 new_n.cost >= mc[new_n.layer][new_n.y][new_n.x])){
                continue;
            }
            mc[new_n.layer][new_n.y][new_n.x] = new_n.cost;
            parent[new_n.layer][new_n.y][new_n.x] = j;
            (*qAq).push(new_n);
        }
    }
    delete qAq;
    return;
}


Node move(Node  n, int & dir, int & reroute){
    Node n2 = n;
    int h = 1;
    e.dir = dir;
    int history_weight = 3,UD_weight = 1,layer_weight = 0;
    if(dir == parent[n2.layer][n2.y][n2.x]) history_weight -= 1;
    switch(dir){
        case 0: // W
            n2.x--;
            if (n2.x < 0) {n2.x++;return e;}
            if(edge_cost[n2.layer][n2.y][n2.x] == MAX_COST) {n2.x++;return e;}
            n2.cost += (all_history[n2.layer][n2.y][n2.x] * history_weight + layer_weight);
            if (n.x <= tg.x) { n2.cost += awayCost; }
            break;
        case 1: // N
            n2.y++;
            if (n2.y >= Height/grid_size) {n2.y--; return e;}
            if(edge_cost[n2.layer][n2.y][n2.x] == MAX_COST) { return e;};
            n2.cost += (all_history[n2.layer][n2.y][n2.x] * history_weight + layer_weight);
            if (n.y >= tg.y) { n2.cost += awayCost; }
            break;
        case 2: // E
            n2.x++;
            if (n2.x >= Width/grid_size) {n2.x--;return e;}
            if(edge_cost[n2.layer][n2.y][n2.x] == MAX_COST) {return e;};
            n2.cost += (all_history[n2.layer][n2.y][n2.x] * history_weight + layer_weight);
            if (n.x >= tg.x) { n2.cost += awayCost; }
            break;
        case 3: // S
            n2.y--;
            if ( n2.y < 0) {n2.y++;return e;}
            if(edge_cost[n2.layer][n2.y][n2.x] == MAX_COST) {n2.y++;return e;}
            n2.cost += (all_history[n2.layer][n2.y][n2.x] * history_weight + layer_weight);
            if (n.y <= tg.y) { n2.cost += awayCost; }
            break;
        case 4: // U
            n2.layer++;
            if (n2.layer >= Layer) {n2.layer--;  return e;}
            if(edge_cost[n2.layer][n2.y][n2.x] == MAX_COST)  {return e;}
            if(edge_cost[n2.layer][n2.y][n2.x] == PIN_PREV) {return e;}
            n2.cost += (all_history[n2.layer][n2.y][n2.x] * history_weight - UD_weight);
            if(n2.cost < 0) n2.cost = 0;
            if (n.layer > tg.layer) { n2.cost += awayCost; }
            break;
        case 5: // D
            n2.layer--;
            if (n2.layer < 0) {n2.layer++;  return e;}
            if(edge_cost[n2.layer][n2.y][n2.x] == MAX_COST)  {n2.layer++;  return e;}
            if(edge_cost[n2.layer][n2.y][n2.x] == PIN_PREV) {return e;}
            n2.cost += (all_history[n2.layer][n2.y][n2.x] * history_weight + UD_weight);
            if (n.layer < tg.layer) { n2.cost += awayCost; }
            break;
    }
    n2.dir = dir;
    return n2;
}


void memsetAll(int layer,double height,double width){
    for(int i=0; i<layer; i++){
        for(int j=0; j<height/grid_size; j++){
            for(int k=0; k<width/grid_size; k++){
                mc[i][j][k]= -1;
                parent[i][j][k]= -1;
            }
        }
    }
    return;
}

bool searchingLine(LINE line,int x,int y,int layer){
    vector<LINE> temp;
    temp = pass_line_list[layer][y][x];
    vector<LINE> ::iterator line_i;
    for(line_i = temp.begin(); line_i != temp.end(); line_i++){
        if(line_i->number == line.number) return true;
    }
    return false;
}

int calculateNodeCost(int x,int y,int layer){
    vector<LINE> temp;
    temp = pass_line_list[layer][y][x];
    vector<int> NET_TYPE;
    vector<LINE> ::iterator line_i;
    vector<int> ::iterator N_i;
    int over = 0;
    for(line_i = temp.begin(); line_i != temp.end(); line_i++){
        N_i = find(NET_TYPE.begin(),NET_TYPE.end(),line_i->number);
        if(N_i == NET_TYPE.end()) NET_TYPE.push_back(line_i->number);
    }
    return NET_TYPE.size();
}

void moveBack(Node &n, NET net, LINE line ){
    switch (parent[n.layer][n.y][n.x]){
        case 1: // N
            n.y--;
            // if(!searchingLine(line,n.x,n.y,n.layer)) horizontal_edge_cost[n.layer][n.y][n.x]++;//edge_cost_south
            pass_line_list[n.layer][n.y][n.x].push_back(line);
            // << "push line:    NET " << line.number << ", line # " << line.line_number << endl;
            //fpppp << "netNId(" << n.layer << ", " << n.x << ", " << n.y << ") size:" << netNId[n.layer][n.y][n.x].size() << "   ";
            break;
        case 2: // E
            n.x--;
            //if(!searchingLine(line,n.x,n.y,n.layer)) vertical_edge_cost[n.layer][n.y][n.x]++;
            pass_line_list[n.layer][n.y][n.x].push_back(line);
            break;
        case 3: // S
           // if(!searchingLine(line,n.x,n.y,n.layer)) horizontal_edge_cost[n.layer][n.y][n.x]++;
            n.y++;
            pass_line_list[n.layer][n.y][n.x].push_back(line);
            break;
        case 0: // W
          //  if(!searchingLine(line,n.x,n.y,n.layer)) vertical_edge_cost[n.layer][n.y][n.x]++;
            n.x++;
            pass_line_list[n.layer][n.y][n.x].push_back(line);
            break;
        case 4: //up
            n.layer--;
           // if(!searchingLine(line,n.x,n.y,n.layer)) up_edge_cost[n.layer][n.y][n.x]++;
            pass_line_list[n.layer][n.y][n.x].push_back(line);
            break;
        case 5: //down
           // if(!searchingLine(line,n.x,n.y,n.layer)) up_edge_cost[n.layer][n.y][n.x]++;
            n.layer++;
            pass_line_list[n.layer][n.y][n.x].push_back(line);

            break;
    }
    return;
}

void traceBack(NET *net, LINE *line){
    NET *n = net;
    Node node = tg;
    while (parent[node.layer][node.y][node.x] != -1){
        Stepping_stone stone = {node.layer, node.x, node.y};
        line->node_list.push_back(stone);
        moveBack(node, *n, *line);
    }
    Stepping_stone stone = {node.layer, node.x, node.y};
    line->node_list.push_back(stone);
    return;
}

int calcOverflow(){
    int over = 0;
	for(int k = 0; k < Layer; k++){
	    for (int i = 0 ; i < Height/grid_size; i++){
			for (int j = 0; j < Width/grid_size; j++){
                if(edge_cost[k][i][j] != MAX_COST){
                    of_all_edge[k][i][j] = max(calculateNodeCost(j,i,k) - 1,0);
                    over += of_all_edge[k][i][j];
                }
                else of_all_edge[k][i][j] = 0;
			}
	    }
	}
    return over;
}

void pushOverFlowQueue(priority_queue <Node, vector<Node>, cmpOf> *rrPQ){
    while(!(*rrPQ).empty()) (*rrPQ).pop();
	for(int k = 0; k < Layer; k++)
	    for (int i = 0 ; i < Height/grid_size; i++)
			for (int j = 0; j < Width/grid_size; j++)
				if(of_all_edge[k][i][j] > 0){
					all_history[k][i][j]++;
					Node temp = {k,j,i,0,-1};
					(*rrPQ).push(temp);
				}
    return;
}


void ripup(int &x, int &y, int &layer, int &dir, priority_queue <LINE, vector<LINE>, cmpDrop>* reroutePQ){
    vector <LINE> idList;
    idList = pass_line_list[layer][y][x] ;
    for (unsigned int i = 0; i < idList.size(); i++){
        LINE line_index = idList[i];
        for(int layer=0; layer<Layer; layer++){
            for(int x=0; x<Width/grid_size; x++){
                for(int y=0; y<Height/grid_size; y++){

                    vector <LINE> ::iterator line_i;
                    for(line_i = pass_line_list[layer][y][x].begin(); line_i != pass_line_list[layer][y][x].end(); line_i++){
                        if(line_i->number == line_index.number  &&  line_i->line_number == line_index.line_number){
                            pass_line_list[layer][y][x].erase(line_i);

                            break;
                        }
                    }

                }
            }
        }
        ///clear NET list
        vector<NET> ::iterator net_i;
        int check = 0;
        for(net_i = netlist.begin(); net_i != netlist.end(); net_i++){
            if(net_i->NET_number == line_index.number){
                vector<LINE> ::iterator line_ii;
                for(line_ii = net_i->line_list.begin(); line_ii != net_i->line_list.end(); line_ii++){
                    if(line_ii->line_number == line_index.line_number) {
                        line_ii->node_list.clear();
                        line_ii->node_list.shrink_to_fit();
                        check = 1;
                        break;
                    }
                }
            }
            if(check == 1) break;
        }
        (*reroutePQ).push(line_index);
    }
    pass_line_list[layer][y][x].clear() ; //U
    pass_line_list[layer][y][x].shrink_to_fit();
}

double be(){
    return 1.0-exp(-cb*exp(-cr*pass));
}

double pg(int &x, int &y, int &layer,int dir, int &h, int &reroute){
    if (dir == 0) return (double) pow((edge_cost[layer][y][x]+1.0)/hc, k1);
    else if(dir == 1) return (double) pow((edge_cost[layer][y][x]+1.0)/vc, k1);
    else if(dir == 2) return (double) pow((edge_cost[layer][y][x]+1.0)/uc, k1);
}

bool linecmp(sp_line a, sp_line b){
    return a.length > b.length;
}
