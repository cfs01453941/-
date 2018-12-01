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

class Components{
public:
    string name;
    int block_num;
    int coordinateX;
    int coordinateY;
    string orientation;
};

class Pins{
public:
    string pinName;
    string netName;
    string direction;   // {INPUT | OUTPUT | INOUT | FEEDTHRU}
    string use; // {SIGNAL | POWER | GROUND | CLOCK | TIEOFF | ANALOG}

    int metal;

    int coordinate_lb_x;
    int coordinate_lb_y;
    int coordinate_rt_x;
    int coordinate_rt_y;

    int coordinate_fixed_x;
    int coordinate_fixed_y;
    string orientation;

};

vector<string> cut_line(char* line){   //  useless
    char *test = strtok(line, " ");
    int i=0;
    vector<string> words;

    while (test != NULL) {
        string str(test);   // char* to string

        words.push_back(str);
        //cout << "i = " << i << " words.at(i) = " << words.at(i) << endl;
        test = strtok(NULL, " ");
        i++;
    }
    return words;
}

int main()
{
    fstream fin;
    char line[SIZE];
    fin.open("case3_input.def", ios::in);

    vector<string> words;

    char* state;

    int comp_num = 0;
    vector<Components> comp_list;
    Components C;

    int pin_num = 0;
    vector<Pins> pin_list;
    Pins P;

    while(fin.getline(line,sizeof(line),'\n')){
        words = cut_line(line);

        //>>>>>>>>>>>>>>>>>>>>>>>>>> read components >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        if( !words.empty() && strcmp( words.at(0).c_str(), "COMPONENTS") == 0 ){  // string to char*
            comp_num = atoi( words.at(1).c_str() );    // string to int
            state = "component";
            continue;
        }

        if( !words.empty() && strcmp( state, "component") == 0 ){
            if( strcmp( words.at(0).c_str(), "-") == 0 ){
                C.name = words.at(1);
                C.block_num = atoi(words.at(2).c_str()+5);
                C.coordinateX = atoi( words.at(6).c_str() );
                C.coordinateY = atoi( words.at(7).c_str() );
                C.orientation = words.at(9);
                comp_list.push_back(C);
            }
        }
        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   */

        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> read pins >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        if( !words.empty() && strcmp( words.at(0).c_str(), "PINS") == 0 ){  // string to char*
            comp_num = atoi( words.at(1).c_str() );    // string to int
            state = "pin";
            continue;
        }

        if( !words.empty() && strcmp( state, "pin") == 0 ){
            if( words.size()>=10 && strcmp( words.at(0).c_str(), "-") == 0 ){
                P.pinName = words.at(1);
                P.netName = words.at(4);
                P.direction = words.at(7);
                P.use = words.at(10);
            }

            if( words.size()>=9 && strcmp( words.at(1).c_str(), "LAYER" ) == 0 ){
                P.metal = atoi(words.at(2).c_str()+5);
                P.coordinate_lb_x = atoi(words.at(4).c_str());
                P.coordinate_lb_y = atoi(words.at(5).c_str());
                P.coordinate_rt_x = atoi(words.at(8).c_str());
                P.coordinate_rt_y = atoi(words.at(9).c_str());
            }

            if( words.size()>=6 && strcmp( words.at(1).c_str(), "FIXED") == 0 ){
                P.coordinate_fixed_x = atoi(words.at(3).c_str());
                P.coordinate_fixed_y = atoi(words.at(4).c_str());
                P.orientation = words.at(6);
                pin_list.push_back(P);
            }

        }

        //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        words.clear();
    }


    //>>>>>>>>>>>>>>>>>>> print result >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    vector<Components>::iterator comp_i;
    for(comp_i=comp_list.begin(); comp_i!=comp_list.end(); comp_i++){
        cout << "name: " << (*comp_i).name << endl;
        cout << "block_num: " << (*comp_i).block_num << endl;
        cout << "coordinate: " << (*comp_i).coordinateX  << ", " << (*comp_i).coordinateY << endl;
        cout << "orientation: " << (*comp_i).orientation << endl << endl;
    }

    vector<Pins>::iterator pin_i;
    for(pin_i=pin_list.begin(); pin_i!=pin_list.end(); pin_i++){
        cout << "pinName: " << (*pin_i).pinName << endl;
        cout << "netName: " << (*pin_i).netName << endl;
        cout << "direction: " << (*pin_i).direction << endl;
        cout << "use: " << (*pin_i).use << endl;

        cout << "metal: " << (*pin_i).metal << endl;

        cout << "point_lb: " << (*pin_i).coordinate_lb_x << ", " << (*pin_i).coordinate_lb_y << endl;
        cout << "point_rt: " << (*pin_i).coordinate_rt_x << ", " << (*pin_i).coordinate_rt_y << endl;
        cout << "fixed: " << (*pin_i).coordinate_fixed_x << ", " << (*pin_i).coordinate_fixed_y << endl;

        cout << "orientation: " << (*pin_i).orientation << endl << endl;

    }

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    return 0;
}
