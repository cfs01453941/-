#include <iostream>
#include<fstream>
#include <istream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string.h>
#include <cstdlib>

#define SIZE 100

using namespace std;

typedef struct _coordinate{
    double x;
    double y;
}coordinate;

class Metal{
public:
    int layer;
    string rect;
    coordinate coordinate_lb, coordinate_rt;
};

class Component{
public:
    string name;
    int block_num;
    //int coordinateX;
    int coordinateY;
    string orientation;
};

class Pin{
public:
    string name;
    string direction;   // {INPUT | OUTPUT | INOUT | FEEDTHRU}
    string use; // {SIGNAL | POWER | GROUND | CLOCK | TIEOFF | ANALOG}
    vector<Metal> metal;

};

class Block{
public:
    int num;
    string _class;
    coordinate origin;
    typedef struct __size{
        double width;
        double height;
    }_size;
    _size Size;
    typedef struct SYMMETRY{
        string x;
        string y;
        string ang;
    }_symmetry;
    _symmetry symmetry;
    vector<Pin> pin;
    vector<Metal> metal;

    void print_result();

    ~Block();
};

void Block::print_result(){
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

Block::~Block(){

}

vector<string> cut_line(char* line){   //  useless
    char *test = strtok(line, " ");
    int i=0;
    vector<string> words;

    while (test != NULL) {
        string str(test);   // char* to string

        words.push_back(str);
        cout << "i = " << i << " words.at(i) = " << words.at(i) << endl;
        test = strtok(NULL, " ");
        i++;
    }
    return words;
}



int main()
{
    fstream fin;
    char line[SIZE];
    fin.open("blocks.lef", ios::in);

    vector<string> words;
    vector<Block> block_type;

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
                cout << "enter OBS" << endl;
            }

            //>>>>>>>>>>>>>>>>>>>>>>>>>> set pin >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
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
            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> set metal >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
            if( !strcmp( state.c_str(), "OBS" ) ){
                    cout << "test OBS"<< endl<<endl;
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
            //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

            if( words.size()>=2 && !strcmp( words.at(0).c_str(), "END") ){

                if( !strncmp( words.at(1).c_str(), "VDD", 3)){
                    //cout << "end VDD" << endl;
                    B.pin.push_back(P);
                    P.metal.clear();
                    state = "empty";
                }
                else if( !strncmp( words.at(1).c_str(), "block", 5)){
                    //cout << "end block" << endl;
                    block_type.push_back(B);
                }
            }
        }
    }



    vector<Block>::iterator block_i;
    //vector<Pin>::iterator pin_i;
    //vector<Metal>::iterator metal_i;
    for(block_i=block_type.begin(); block_i!=block_type.end(); block_i++){
/*
        cout << "block num : " << (*block_i).num << "\n";
        for( pin_i=(*block_i).pin.begin(); pin_i!=(*block_i).pin.end(); pin_i++ ){
            cout << "pin name : " << (*pin_i).name << endl;

            for(metal_i=(*pin_i).metal.begin(); metal_i!=(*pin_i).metal.end(); metal_i++ ){
                cout << "metal layer : " << (*metal_i).layer << endl;
            }

        }

        cout << "OBS" << endl;
        for(metal_i=(*block_i).metal.begin(); metal_i!=(*block_i).metal.end(); metal_i++ ){
            cout << "OBS metal layer : " << (*metal_i).layer << endl;
        }
        cout << "\n";
*/
        (*block_i).print_result();
    }
    return 0;
}
