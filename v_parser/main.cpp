#include <iostream>
#include<fstream>
#include <istream>
#include <vector>
#include <iterator>
#include <sstream>
#include <string.h>
#include <cstdlib>

#define v_file_SIZE 2000

using namespace std;

typedef struct _coordinate{
    double x;
    double y;
}coordinate;

class net_pin{
    string name;
    coordinate pin_coordinate;
};

class Net{
public:
    string source_name;
    coordinate source_coordinate;

    vector<net_pin> pins;
};

vector<string> cut_line(char* line){
    char *test = strtok(line, " ");
    int i=0;
    vector<string> words;

    while (test != NULL) {
        string str(test);   // char* to string

        words.push_back(str);
        cout << "i = " << i << " words.at(i) = " << words.at(i) << endl;
        cout << *(words.at(i).begin()) << endl << endl;
        test = strtok(NULL, " ");
        i++;
    }
    return words;
}



int main()
{
    fstream fin;
    char line[v_file_SIZE];
    string input = "case1.v";
    fin.open(input.c_str(), ios::in);

    vector<string> words;

    vector<Net> nets;

    while(fin.getline(line,sizeof(line),'\n')){
        words = cut_line(line);

        if(!words.empty()){
            if( words.size()>=2 && !strcmp( words.at(0).c_str(), "input") ){
                for(int i=1; i<words.size(); i++){

                }
            }
        }


    }




    return 0;
}
