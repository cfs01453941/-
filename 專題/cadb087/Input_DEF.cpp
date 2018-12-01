#include "Input_DEF.h"

vector<string> Input_def::cut_line(char* line){
    char *test = strtok(line, " ");
    int i=0;
    vector<string> words;

    while (test != NULL) {
        string str(test);   // char* to string
        words.push_back(str);
        test = strtok(NULL, " ");
        i++;
    }
    return words;
}
void Input_def::Print(){
    vector<Components> comp_list = this->Comp_list;
    vector<Pins> pin_list = this->Pin_list;
    vector<Components>::iterator comp_i;

    cout << "unit: " << unit_length << endl;

    for(comp_i=comp_list.begin(); comp_i!=comp_list.end(); comp_i++){
        cout << "name: " << (*comp_i).name << endl;
        cout << "block_num: " << (*comp_i).block_num << endl;
        cout << "coordinate: " << (*comp_i).origin.x  << ", " << (*comp_i).origin.y << endl;
        cout << "orientation: " << (*comp_i).orientation << endl;
        cout << "size-- width: " << (*comp_i).Size.width << " , height: " << (*comp_i).Size.height << endl;
        cout << "lb: " << (*comp_i).lb.x << ", " << (*comp_i).lb.y << endl;
        cout << "rt: " << (*comp_i).rt.x << ", " << (*comp_i).rt.y << endl << endl;

        vector<component_pin>::iterator pin_list_i;
        for(pin_list_i=comp_i->pin_list.begin(); pin_list_i!=comp_i->pin_list.end(); pin_list_i++){
            cout << "pin name: " << pin_list_i->name << endl;
            cout << "pin current: " << pin_list_i->current << endl;
            vector<pin_metal>::iterator pin_metal_i;
            for(pin_metal_i=pin_list_i->metal.begin(); pin_metal_i!=pin_list_i->metal.end(); pin_metal_i++){
                cout << "layer: " << pin_metal_i->layer << endl;
                cout << "lb: " << (int)pin_metal_i->lb.x << ", " << pin_metal_i->lb.y << endl;
                cout << "rt: " << (int)pin_metal_i->rt.x << ", " << pin_metal_i->rt.y << endl << endl;
            }
        }
    }


    vector<Pins>::iterator pin_i;
    for(pin_i=pin_list.begin(); pin_i!=pin_list.end(); pin_i++){
        cout << "pinName: " << (*pin_i).pinName << endl;
        cout << "netName: " << (*pin_i).netName << endl;
        cout << "direction: " << (*pin_i).direction << endl;
        cout << "use: " << (*pin_i).use << endl;
        cout << "metal: " << (*pin_i).metal << endl;
        cout << "point_lb: " << (*pin_i).lb.x << ", " << (*pin_i).lb.y << endl;
        cout << "point_rt: " << (*pin_i).rt.x << ", " << (*pin_i).rt.y << endl;
        cout << "fixed: " << (*pin_i).fixed.x << ", " << (*pin_i).fixed.y << endl;
        cout << "orientation: " << (*pin_i).orientation << endl << endl;
    }
}
Input_def::Input_def(string fp)
{
    fstream fin;
    char line[10000];
    fin.open(fp.c_str(), ios::in);
    vector<string> words;
    string state;
    int comp_num = 0;
    vector<Components> comp_list;
    Components C;
    int pin_num = 0;
    vector<Pins> pin_list;
    Pins P;

    while(fin.getline(line,sizeof(line),'\n')){
        words = cut_line(line);
        if(!words.empty()){
            if(strcmp( words.at(0).c_str(), "UNITS") == 0){
                unit_length = atof(words.at(3).c_str());
            }

            if( !words.empty() && strcmp( words.at(0).c_str(), "COMPONENTS") == 0 ){  // string to char*
                comp_num = atof( words.at(1).c_str() );    // string to int
                state = "component";
                continue;
            }
            if( !words.empty() && strcmp( words.at(0).c_str(), "DIEAREA") == 0 ){  // string to char*
                unit_width = atof( words.at(6).c_str() );
                unit_height = atof( words.at(7).c_str() );
                continue;
            }
            if( !words.empty() && state ==  "component" ){
                if( strcmp( words.at(0).c_str(), "-") == 0 ){
                    C.name = words.at(1);
                    C.block_num = atof(words.at(2).c_str()+5);
                    C.origin.x = atof( words.at(6).c_str() ) / unit_length;
                    C.origin.y = atof( words.at(7).c_str() ) / unit_length;

                    C.orientation = words.at(9);
                    comp_list.push_back(C);
                }
            }
            if( !words.empty() && strcmp( words.at(0).c_str(), "PINS") == 0 ){  // string to char*
                comp_num = atof( words.at(1).c_str() );    // string to int
                state = "pin";
                continue;
            }
            if( !words.empty() &&  state == "pin" ){
                if( words.size()>=10 && strcmp( words.at(0).c_str(), "-") == 0 ){
                    P.pinName = words.at(1);
                    P.netName = words.at(4);
                    P.direction = words.at(7);
                    P.use = words.at(10);
                }

                if( words.size()>=9 && strcmp( words.at(1).c_str(), "LAYER" ) == 0 ){
                    P.metal = atof(words.at(2).c_str()+5);
                    P.lb.x = atof(words.at(4).c_str());
                    P.lb.y = atof(words.at(5).c_str());
                    P.rt.x = atof(words.at(8).c_str());
                    P.rt.y = atof(words.at(9).c_str());

                }

                if( words.size()>=6 && strcmp( words.at(1).c_str(), "FIXED") == 0 ){
                    P.fixed.x = atof(words.at(3).c_str());
                    P.fixed.y = atof(words.at(4).c_str());
                    P.orientation = words.at(6);
                    pin_list.push_back(P);
                }

            }
            words.clear();
        }
    }

    this->Comp_list = comp_list;
    this->Pin_list = pin_list;
}

void redefine_lb_rt(coordinate &lb, coordinate &rt){
    coordinate new_lb, new_rt;
    new_lb.x = min(lb.x, rt.x);
    new_lb.y = min(lb.y, rt.y);
    new_rt.x = max(lb.x, rt.x);
    new_rt.y = max(lb.y, rt.y);
    lb = new_lb;
    rt = new_rt;
}

void Components::rotate_block(){
    double RM[4] = {};
    bool flip = false;
    if( strcmp(orientation.c_str(),"N")==0){
        RM[0] = 1.0;
        RM[1] = 0.0;
        RM[2] = 0.0;
        RM[3] = 1.0;
        flip = false;
    }
    else if( strcmp(orientation.c_str(),"W")==0){
        RM[0] = 0.0;
        RM[1] = -1.0;
        RM[2] = 1.0;
        RM[3] = 0.0;
        flip = false;
    }
    else if( strcmp(orientation.c_str(),"S")==0){
        RM[0] = -1.0;
        RM[1] = 0.0;
        RM[2] = 0.0;
        RM[3] = -1.0;
        flip = false;
    }
    else if( strcmp(orientation.c_str(),"E")==0){
        RM[0] = 0.0;
        RM[1] = 1.0;
        RM[2] = -1.0;
        RM[3] = 0.0;
        flip = false;
    }
    if( strcmp(orientation.c_str(),"FN")==0){
        RM[0] = 1.0;
        RM[1] = 0.0;
        RM[2] = 0.0;
        RM[3] = 1.0;
        flip = true;
        //return;
    }
    else if( strcmp(orientation.c_str(),"FW")==0){
        RM[0] = 0.0;
        RM[1] = -1.0;
        RM[2] = 1.0;
        RM[3] = 0.0;
        flip = true;
    }
    else if( strcmp(orientation.c_str(),"FS")==0){
        RM[0] = -1.0;
        RM[1] = 0.0;
        RM[2] = 0.0;
        RM[3] = -1.0;
        flip = true;
    }
    else if( strcmp(orientation.c_str(),"FE")==0){
        RM[0] = 0.0;
        RM[1] = 1.0;
        RM[2] = -1.0;
        RM[3] = 0.0;
        flip = true;
    }
    else {
        double RM[] = {0.0, 1.0, -1.0, 0.0};
        flip = false;
    }

    lb.Rotate(RM);  // rotate component
    rt.Rotate(RM);
    redefine_lb_rt(lb,rt);

    if(flip){   /// flip
        lb.x = (-1.0)*lb.x;
        rt.x = (-1.0)*rt.x;
        redefine_lb_rt(lb, rt);
    }

    // rotate pins
    vector<component_pin>::iterator pin_list_i;
    for(pin_list_i=pin_list.begin(); pin_list_i!=pin_list.end(); pin_list_i++){

        vector<pin_metal>::iterator pin_metal_i;
        for(pin_metal_i=pin_list_i->metal.begin(); pin_metal_i!=pin_list_i->metal.end(); pin_metal_i++){
            (pin_metal_i->lb).Rotate(RM);
            (pin_metal_i->rt).Rotate(RM);
            redefine_lb_rt(pin_metal_i->lb, pin_metal_i->rt);
            if(flip){   /// flip
                pin_metal_i->lb.x = (-1.0)* pin_metal_i->lb.x;
                pin_metal_i->rt.x = (-1.0)* pin_metal_i->rt.x;
                redefine_lb_rt(pin_metal_i->lb, pin_metal_i->rt);
            }
        }
    }
}


void Input_def::rotate_all_block(vector<Block> block_type){
    vector<Components>::iterator comp_i;
    for(comp_i=Comp_list.begin(); comp_i!=Comp_list.end(); comp_i++){
        int num = comp_i->block_num - 1;
        comp_i->layer_size = block_type.at(num).metal.size();
        double width = block_type.at(num).Size.width;
        double height = block_type.at(num).Size.height;
        comp_i->lb.x = 0 - block_type.at(num).center.x;
        comp_i->lb.y = 0 - block_type.at(num).center.y;
        comp_i->rt.x = width - block_type.at(num).center.x;
        comp_i->rt.y = height - block_type.at(num).center.y;

        vector<Pin>::iterator pin_i;
        component_pin p;
        int s = 0;
        for(pin_i=block_type.at(num).pin.begin(); pin_i!=block_type.at(num).pin.end(); pin_i++){
            p.name = pin_i->name;

            vector<Metal>::iterator metal_i;
            pin_metal m;
            int t=0;
            for(metal_i=pin_i->metal.begin(); metal_i!=pin_i->metal.end(); metal_i++){

                m.layer = metal_i->layer;
                m.lb.x = metal_i->coordinate_lb.x - block_type.at(num).center.x;
                m.lb.y = metal_i->coordinate_lb.y - block_type.at(num).center.y;
                m.rt.x = metal_i->coordinate_rt.x - block_type.at(num).center.x;
                m.rt.y = metal_i->coordinate_rt.y - block_type.at(num).center.y;

                p.metal.push_back(m);
            }
            comp_i->pin_list.push_back(p);
            p.metal.clear();
        }

         (*comp_i).rotate_block();
         comp_i->Size.width  = comp_i->rt.x - comp_i->lb.x;
         comp_i->Size.height  = comp_i->rt.y - comp_i->lb.y;
    }
}


void Input_def::set_coordinate(vector<Block> block_type){
    vector<Components>::iterator comp_i;
    for(comp_i=Comp_list.begin(); comp_i!=Comp_list.end(); comp_i++){
        int num = comp_i->block_num - 1;
        (*comp_i).set_lb_rt(comp_i->Size.width, comp_i->Size.height);
        (*comp_i).set_pin_position();
    }
}

void Input_def::set_pin(vector<Block> block_type){
    vector<Components>::iterator comp_i;
    for(comp_i=Comp_list.begin(); comp_i!=Comp_list.end(); comp_i++){
        int num = comp_i->block_num - 1;
        vector<Pin>::iterator pin_i;
        component_pin p;
        int s = 0;
        for(pin_i=block_type.at(num).pin.begin(); pin_i!=block_type.at(num).pin.end(); pin_i++){
            p.name = pin_i->name;
            vector<Metal>::iterator metal_i;
            pin_metal m;
            int t=0;
            for(metal_i=pin_i->metal.begin(); metal_i!=pin_i->metal.end(); metal_i++){
                m.layer = metal_i->layer;
                m.lb.x = comp_i->origin.x + metal_i->coordinate_lb.x;
                m.lb.y = comp_i->origin.y + metal_i->coordinate_lb.y;
                m.rt.x = comp_i->origin.x + metal_i->coordinate_rt.x;
                m.rt.y = comp_i->origin.y + metal_i->coordinate_rt.y;
                p.metal.push_back(m);
            }
            comp_i->pin_list.push_back(p);
            p.metal.clear();
        }
    }
}


int Components::get_block_num(){
    return block_num;
}
void Components::set_lb_rt(double width, double height){
    lb = origin;
    rt.x = origin.x + width;
    rt.y = origin.y + height;

}
void Components::set_pin_position(){
    vector<component_pin>::iterator comp_pin_i;
    for(comp_pin_i=pin_list.begin(); comp_pin_i!=pin_list.end(); comp_pin_i++){
        vector<pin_metal>::iterator pin_metal_i;
        for(pin_metal_i=(comp_pin_i->metal).begin(); pin_metal_i!=(comp_pin_i->metal).end(); pin_metal_i++){
            pin_metal_i->lb.x = pin_metal_i->lb.x + origin.x + Size.width/2;
            pin_metal_i->lb.y = pin_metal_i->lb.y + origin.y + Size.height/2;
            pin_metal_i->rt.x = pin_metal_i->rt.x + origin.x + Size.width/2;
            pin_metal_i->rt.y = pin_metal_i->rt.y + origin.y + Size.height/2;
        }

    }
}

void Input_def::set_pin_current(vector<initial_files_Block> Block_list){
    vector<Components> comp_list = this->Comp_list;
    vector<Components>::iterator comp_i;
    for(comp_i=Comp_list.begin(); comp_i!=Comp_list.end(); comp_i++){

        vector<initial_files_Block>::iterator initial_block_i;
        for(initial_block_i=Block_list.begin(); initial_block_i!=Block_list.end(); initial_block_i++){
            if( strcmp( (comp_i->name).c_str(), (initial_block_i->name).c_str()) == 0 ){

                vector<component_pin>::iterator pin_list_i;
                for(pin_list_i=comp_i->pin_list.begin(); pin_list_i!=comp_i->pin_list.end(); pin_list_i++){
                    vector<initial_files_Pin>::iterator initial_pin_i;
                    for(initial_pin_i=initial_block_i->pins.begin(); initial_pin_i!=initial_block_i->pins.end(); initial_pin_i++){
                        if(strcmp( (pin_list_i->name).c_str(), (initial_pin_i->pin_name).c_str()) == 0 ){
                           pin_list_i->current = initial_pin_i->current;

                        }
                    }
                }
            }
        }
    }
}


