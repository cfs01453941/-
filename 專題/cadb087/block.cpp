#include <iostream>
#include<fstream>
#include <istream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string.h>
#include <cstdlib>
#include "block.h"
#define SIZE 100

using namespace std;

void Block::print_result(){ ///print one block type
    vector<Pin>::iterator pin_i;
    vector<Metal>::iterator metal_i;
    cout << "block num : " << num << "\n";
        for( pin_i=pin.begin(); pin_i!=pin.end(); pin_i++ ){
            cout << "pin name : " << (*pin_i).name << endl;

            for(metal_i=(*pin_i).metal.begin(); metal_i!=(*pin_i).metal.end(); metal_i++ ){
                cout << "metal layer : " << (*metal_i).layer << endl;
            }
        }
        cout << "OBS" << endl;
        for(metal_i=metal.begin(); metal_i!=metal.end(); metal_i++ ){
            cout << "OBS metal layer : " << (*metal_i).layer << endl;
        }
        cout << endl;
}

vector<string> block_Block::cut_line(char* line){
    char *test = strtok(line, " ");
    int i=0;
    vector<string> words;

    while (test != NULL) {
        string str(test);
        words.push_back(str);
        test = strtok(NULL, " ");
        i++;
    }
    return words;
}

void block_Block::print(){  /// print all block types
    vector<Block>::iterator block_i;
    for(block_i=block_type.begin(); block_i!=block_type.end(); block_i++){
        (*block_i).print_result();
    }
}

block_Block::block_Block(string input)
{
    fstream fin;
    char line[10000];
    fin.open(input.c_str(), ios::in);
    vector<string> words;

    Block B;
    Pin P;
    Metal M;

    string state = "empty";

    while(fin.getline(line,sizeof(line),'\n')){
        words = cut_line(line);
        if(!words.empty()){
            if( words.size()>=2 && !strcmp( words.at(0).c_str(), "MACRO") ){
                B.pin.clear();
                B.metal.clear();
                B.num = atoi( words.at(1).c_str()+5 );
            }
            else if( words.size()>=2 && !strcmp( words.at(0).c_str(), "CLASS") ){
                B._class = words.at(1);
            }
            else if( words.size()>=3 && !strcmp( words.at(0).c_str(), "ORIGIN") ){
                B.origin.x = atof( words.at(1).c_str() );
                B.origin.y = atof( words.at(2).c_str() );
            }
            else if( words.size()>=4 && !strcmp( words.at(0).c_str(), "SIZE") ){
                B.Size.width = atof( words.at(1).c_str() );
                B.Size.height = atof( words.at(3).c_str() );
                B.center.x = atof( words.at(1).c_str() ) / 2;
                B.center.y = atof( words.at(3).c_str() ) / 2;
            }
            else if( words.size()>=4 && !strcmp( words.at(0).c_str(), "SYMMETRY") ){
                B.symmetry.x = words.at(1);
                B.symmetry.y = words.at(2);
                B.symmetry.ang = words.at(3);
            }
            else if( words.size()>=2 && !strcmp( words.at(0).c_str(), "PIN") ){
                state = "PIN";
                P.name = words.at(1);
            }
            else if( words.size()>=1 && !strcmp( words.at(0).c_str(), "OBS") ){
                state = "OBS";
            }
            if( !strcmp( state.c_str(), "PIN" ) ){
                if( words.size()>=2 && !strcmp( words.at(0).c_str(), "DIRECTION") ){
                    P.direction = words.at(1);
                }
                else if( words.size()>=2 && !strcmp( words.at(0).c_str(), "USE") ){
                    P.use = words.at(1);
                }
                else if( words.size()>=1 && !strcmp( words.at(0).c_str(), "PORT") ){

                }
                else if( words.size()>=2 && !strcmp( words.at(0).c_str(), "LAYER") ){
                    M.layer = atoi( words.at(1).c_str()+5 );
                }
                else if( words.size()>=5 && !strcmp( words.at(0).c_str(), "RECT") ){
                    M.coordinate_lb.x = atoi( words.at(1).c_str() );
                    M.coordinate_lb.y = atoi( words.at(2).c_str() );
                    M.coordinate_rt.x = atoi( words.at(3).c_str() );
                    M.coordinate_rt.y = atoi( words.at(4).c_str() );
                    P.metal.push_back(M);
                }
            }
            if( !strcmp( state.c_str(), "OBS" ) ){
                if( words.size()>=2 && !strcmp( words.at(0).c_str(), "LAYER") ){
                    M.layer = atoi( words.at(1).c_str()+5 );
                }
                else if( words.size()>=5 && !strcmp( words.at(0).c_str(), "RECT") ){
                    M.coordinate_lb.x = atoi( words.at(1).c_str() );
                    M.coordinate_lb.y = atoi( words.at(2).c_str() );
                    M.coordinate_rt.x = atoi( words.at(3).c_str() );
                    M.coordinate_rt.y = atoi( words.at(4).c_str() );
                    B.metal.push_back(M);
                }
            }
            if( words.size()>=2 && !strcmp( words.at(0).c_str(), "END") ){
                if( !strncmp( words.at(1).c_str(), "VDD", 3)){
                    B.pin.push_back(P);
                    P.metal.clear();
                    state = "empty";
                }
                else if( !strncmp( words.at(1).c_str(), "block", 5)){
                    block_type.push_back(B);

                }
            }
        }
    }
}

vector<Block> block_Block::getBlock(){
        return block_type;
}

void coordinate::Rotate(double RM[]){
    coordinate new_pt;
    new_pt.x = x*RM[0] + y*RM[1];
    new_pt.y = x*RM[2] + y*RM[3];
    x = new_pt.x;
    y = new_pt.y;
}
