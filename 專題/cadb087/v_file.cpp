#include "v_file.h"
BlockofV *head;
BlockofV *cur;
BlockofV *tail;

V_file::V_file(FILE *fp){
    char in[100];
    char temp;
    head = new BlockofV();
    tail = head;
    tail->num_of_pins =0;
    char t[10] = "blockn" ;
    while(fscanf(fp, "%s",in)!=EOF){
        if(strncmp(in, t, 5)==0){
            strcpy(tail->block_type, in);
            fscanf(fp, "%s",tail->name);
            fscanf(fp, "%s",in);
            int index=0;
            while(1){
                fscanf(fp, "%s",in);
                if(strcmp(in, ");")==0){
                    tail->next = new BlockofV();
                    tail = tail->next;
                    tail->num_of_pins =0;
                    tail->next = NULL;
                    break;
                }
                else{
                    if(in[strlen(in)-1]==',')
                        tail->pins[index] = (int)(in[strlen(in)-3]-'0');
                    else
                        tail->pins[index] = (int)(in[strlen(in)-2]-'0');
                    tail->num_of_pins++;
                    index++;
                }
            }
        }
    }
    this->v_head = head;
    this->v_tail = tail;
    this->v_cur = cur;
}
