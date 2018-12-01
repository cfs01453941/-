#include "v_file.h"
BlockofV *head;
BlockofV *cur;
BlockofV *tail;
void V_file::Print(){
    cur = head;
    while(cur->next!=NULL){
        printf("Name : %s\n", cur->name);
        printf("Type : %s\n", cur->block_type);
        printf("Num of pins : %d\n", cur->num_of_pins);
        int a;
        printf("Pins : ");
        for(a=0 ; a<cur->num_of_pins; a++)
            printf("%d ",cur->pins[a]);
        printf("\n\n");
        cur = cur->next;
    }
    printf("Name : %s\n", cur->name);
    printf("Num of pins : %d\n", cur->num_of_pins);
    int a;
    printf("Pins : ");
    for(a=0 ; a<cur->num_of_pins; a++)
        printf("%d ",cur->pins[a]);

    printf("END\n");
}
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
            //printf("%s\n",tail->block_type);
            fscanf(fp, "%s",tail->name);
            //printf("%s\n",tail->name);
            fscanf(fp, "%s",in);
            int index=0;
            while(1){
                fscanf(fp, "%s",in);
                if(strcmp(in, ");")==0){
                    //printf("Num of pins : %d\n", tail->num_of_pins);
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
                    //printf("%d\n", tail->pins[index]);
                    tail->num_of_pins++;
                    index++;
                }
            }
        }
    }
    //Print();
    this->v_head = head;
    this->v_tail = tail;
    this->v_cur = cur;

}
