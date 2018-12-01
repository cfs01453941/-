#include <iostream>
#include<fstream>
#include <stdlib.h>
#include <stdio.h>
//#include "blocks.h"
#include "initial_files.h"
#include "tech.h"
#include "v_file.h"
#include "Input_DEF.h"
#include "block.h"
#include "function.h"

#define MAP_SIZE 3500
#define LAYER_SIZE 7

#define BLOCKAGE_BLOCK 1
#define BLOCKAGE_PIN 2
#define BLOCKAGE_SRC 3

using namespace std;

int GOD_MAP[LAYER_SIZE][MAP_SIZE][MAP_SIZE];

int main()
{
//    /*
    //FILE* fp1 = fopen("blocks.lef","r");
    //Blocks blocks_parser(fp1);
    string in("initial_files");
    Initial_file initial_parser(in);
    FILE* fp3 = fopen("tech.lef","r");
    tech tech_parser(fp3);
    FILE* fp4 = fopen("case2.v","r");
    V_file v_parser(fp4);
 //   */




//>>>>>>>>>>>>> block >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    string in_block("blocks.lef");
    block_Block block_parser(in_block);

    //block_parser.print();
    //block_parser.getBlock().at(0).print_result();
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//>>>>>>>>>>>> input >>>>>>>>>>>>>>>>>>>>>>>>
    string in1("case2_input.def");
    Input_def input_parser(in1);
    input_parser.rotate_all_block(block_parser.getBlock());


    input_parser.set_coordinate( block_parser.getBlock() );

    //input_parser.set_pin(block_parser.getBlock());
    input_parser.set_pin_current( initial_parser.get_initial_block() );
//
    input_parser.Print();
 //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    //v_parser.Print();

    // set GOD_MAP
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
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//print GOD_MAP

    FILE* BLOCKOUT = fopen("Graph.out","w");
    for(int i = MAP_SIZE-1; i >=0 ; i -= 5){
        int num1 = 0,num2 = 0;
        for(int j = 0; j < MAP_SIZE; j+=5){
            int k = 4,tempnum = 0;
            for(k = 0; k < 7; k++)
                if(GOD_MAP[k][i][j] != 0){
                    tempnum = GOD_MAP[k][i][j];
                    if(tempnum == BLOCKAGE_PIN) break;
                }
            if(k == 7 && tempnum == 0) fprintf(BLOCKOUT,"_");
            else fprintf(BLOCKOUT,"%d",tempnum);
        }
        fprintf(BLOCKOUT,"\n");
    }

    coordinate p1,p2;
    p1.x = 3.5;
    p1.y = 3.5;
    p2.x = 3.4;
    p2.y = 3.5;

    cout << distance_of_tow_point(p1,p2) << endl;

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    return 0;
}
