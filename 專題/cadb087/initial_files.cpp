#include "initial_files.h"

vector<string> Initial_file::cut_line(char* line){   //  useless
    char strline[strlen(line)];
    strcpy(strline,line);
    char *delim = " ";
    char *test = strtok(strline, delim);
    int i=0;
    vector<string> words;
    while (test != NULL) {
        string str(test);   // char* to string
        words.push_back(str);
        test = strtok(NULL, delim);
        i++;
    }
    return words;
}
power_src* Initial_file::getPowerSrc(string name){
    power_src *temp = this->Src_head;
    while(temp != NULL){
        if(temp->name == name) return temp;
        else temp = temp->next;
    }
    return  0;
}

initial_files_Pin Initial_file::getInitial_files_Pin(string block_name,string pin_name){
    vector<initial_files_Block>::iterator block_i;
    for(block_i = this->Block_list.begin(); block_i != this->Block_list.end(); block_i++){
        if(block_i->name == block_name){
            vector<initial_files_Pin>::iterator pin_i;
            for(pin_i = block_i->pins.begin(); pin_i != block_i->pins.end(); pin_i++){
                if(pin_i->pin_name == pin_name) return (*pin_i);
            }
        }
    }
}

power_src* Initial_file::setPowerSrc(vector<string> words){
    power_src* src = new power_src();
    src->name = words.front();
    src->src_v = atof(words.back().c_str());
    return src;
}
layer Initial_file::setLayer(vector<string> words){
    layer src;
    src.name = words.front();
    src.weight = atof(words.back().c_str());
    return src;
}

void Print(vector<initial_files_Block> block_list,power_src *src_head,layer *metal_head){
    vector<initial_files_Block>::iterator block_i;
    vector<initial_files_Pin>::iterator pin_i;
    for(block_i=block_list.begin(); block_i!=block_list.end(); block_i++){
        cout << "block name : " << (*block_i).name << "\n";
        for( pin_i=(*block_i).pins.begin(); pin_i!=(*block_i).pins.end(); pin_i++ ){
            cout << "initial_files_Pin name : " << (*pin_i).pin_name << "  current : " << (*pin_i).current << "  IR_drop : "<<(*pin_i).drop<<"%\n";
        }
        cout << "\n";
    }
    power_src *temp1 = src_head;
    cout<<"\nvoltage\n";
    while(temp1 != NULL){
        cout<<"name : "<<temp1->name<<"\n";
        cout<<"VDD : "<<temp1->src_v<<"\n";
        temp1 = temp1->next;
    }
}

Initial_file::Initial_file(string input)
{
    ios_base::sync_with_stdio(false);
    char line[100];
    vector<string> words;
    int state = 0;
    fstream fin;
    fin.open(input.c_str(), ios::in);
    vector<initial_files_Block> block_list;
    initial_files_Block B;
    vector<initial_files_Block>::iterator block_i;
    bool new_block = true;
    power_src *src_head,*src_tail;
    src_head = src_tail = NULL;

    while( fin.getline(line, sizeof(line),'\n') ){
        words = cut_line(line);
        if(words.empty()) continue;
        if(!strcmp(words.back().c_str(),"(mA)")){
            state = CURRENT;
        }
        else if(!strcmp(words.back().c_str(),"(V)")){
            state = VOLTAGE;
        }
        else if(!strcmp(words.back().c_str(),"(%)")){
            state = IR_DROP;
        }
        else if(!strcmp(words.back().c_str(),"layers")){
            state = WEIGHT;
        }
        else {
            if(state == CURRENT){
                for(block_i=block_list.begin(); block_i!=block_list.end(); block_i++){
                    if( (*block_i).name == words.at(0) ){   //  block ¤w¸g¦s¦b
                        vector<initial_files_Pin>::iterator pin_i;
                        for(pin_i=(*block_i).pins.begin(); pin_i!=(*block_i).pins.end(); pin_i++){
                            if((*pin_i).pin_name == words.at(1)){
                                new_block = false;
                                break;
                            }
                        }
                        if(new_block == false) break;
                        initial_files_Pin P;
                        P.pin_name = words.at(1);
                        P.current = atoi(words.at(2).c_str());
                        P.drop = 0.0;
                        (*block_i).pins.push_back(P);

                        new_block = false;
                        break;
                    }
                }
                if( new_block && words.size()>=2){  // add a new block
                    initial_files_Pin P;
                    B.name = words.at(0);
                    P.pin_name = words.at(1);
                    P.current = atof(words.at(2).c_str());
                    P.drop = 0.0;
                    B.pins.push_back(P);
                    block_list.push_back(B);
                    B.pins.clear();
                }
                new_block = true;
            }
            else if(state == VOLTAGE){
                if(src_head == NULL){
                    src_head = setPowerSrc(words);
                    src_tail = src_head;
                }else{
                    src_tail->next = setPowerSrc(words);
                    src_tail = src_tail->next;
                }
            }
            else if(state == IR_DROP){
                new_block = true;
                for(block_i=block_list.begin(); block_i!=block_list.end(); block_i++){
                    if( (*block_i).name == words.at(0) ){   //
                        vector<initial_files_Pin>::iterator pin_i;
                        for(pin_i=(*block_i).pins.begin(); pin_i!=(*block_i).pins.end(); pin_i++){
                            if((*pin_i).pin_name == words.at(1)){
                                (*pin_i).drop = atof(words.at(2).c_str());
                                new_block = false;
                                break;
                            }
                        }
                        if(new_block == false) break;
                    }
                }
                new_block = true;
            }
            else if(state == WEIGHT){
                metal_layer.push_back(setLayer(words));
            }
        }
    }
    fin.close();
    this->Block_list = block_list;
    this->Src_head = src_head;
    this->Src_tail = src_tail;
}

vector<initial_files_Block> Initial_file::get_initial_block(){
    return Block_list;
}

vector<layer> Initial_file::get_metal_layer(){
    return metal_layer;
}
