#include "tech.h"
int database_micron;
double MANUFACTURINGGRID;
tech_layer *head_layer ;
tech_layer *cur_layer ;
tech_layer *tail_layer ;
via *head_via;
via *cur_via;
via *tail_via;
tech_layer* tech::search4layer(char* name){
    tech_layer *out = head_layer;
    while(strcmp(out->NAME, "NoInfo")!=0){
        if(strcmp(out->NAME, name)==0)
            return out;
        else
            out = out->next;
    }
    return NULL;
}

via* tech::search4via(char* name){
    via *out = head_via;
    while(out->next!=NULL){
        if(strcmp(out->NAME, name)==0)
            return out;
        else
            out = out->next;
    }
    return NULL;
}
void tech::Print(){
    cur_layer = head_layer;
    int numoflayer=0;
    while(strcmp(cur_layer->NAME, "NoInfo")!=0){
        numoflayer++;
        printf("Name : %s\n", cur_layer->NAME);
        printf("Type : %s\n", cur_layer->TYPE);
        printf("Width : %d\n", cur_layer->WIDTH);
        printf("Max width : %d\n", cur_layer->MAXWIDTH);
        printf("Spacing : %d\n", cur_layer->SPACING);
        printf("Pitch : %d\n", cur_layer->PITCH);
        printf("Offset : %d\n", cur_layer->OFFSET);
        printf("Direction : %s\n", cur_layer->DIRECTION);
        printf("Resistance per square : %.2lf\n", cur_layer->RESISTANCE_RPERSQ);
        printf("\n");
        cur_layer = cur_layer->next;
    }
    printf("!!! num of layers : %d !!!\n", numoflayer);

    cur_via = head_via;
    int numofvia=0;
    while(cur_via->next!=NULL){
        numofvia++;
        printf("%s\n", cur_via->NAME);
        printf("%.1lf\n", cur_via->RESISTANCE);
        printf("%s %f %f %f %f\n", cur_via->LAYERS[bot], cur_via->RECT[bot][0], cur_via->RECT[bot][1], cur_via->RECT[bot][2], cur_via->RECT[bot][3]);
        printf("%s %f %f %f %f\n", cur_via->LAYERS[mid], cur_via->RECT[mid][0], cur_via->RECT[mid][1], cur_via->RECT[mid][2], cur_via->RECT[mid][3]);
        printf("%s %f %f %f %f\n", cur_via->LAYERS[top], cur_via->RECT[top][0], cur_via->RECT[top][1], cur_via->RECT[top][2], cur_via->RECT[top][3]);
        printf("\n");
        cur_via = cur_via->next;
    }
    printf("!!! num of vias : %d !!!\n", numofvia);

    cur_layer = search4layer("METAL4");
    printf("Name : %s\n", cur_layer->NAME);
    printf("Type : %s\n", cur_layer->TYPE);
    printf("Width : %d\n", cur_layer->WIDTH);
    printf("Max width : %d\n", cur_layer->MAXWIDTH);
    printf("Spacing : %d\n", cur_layer->SPACING);
    printf("Pitch : %d\n", cur_layer->PITCH);
    printf("Offset : %d\n", cur_layer->OFFSET);
    printf("Direction : %s\n", cur_layer->DIRECTION);
    printf("Resistance per square : %.2lf\n", cur_layer->RESISTANCE_RPERSQ);
    printf("\n");

    cur_via = search4via("via2_A");
    printf("%s\n", cur_via->NAME);
    printf("%f\n", cur_via->RESISTANCE);
    printf("%s %f %f %f %f\n", cur_via->LAYERS[bot], cur_via->RECT[bot][0], cur_via->RECT[bot][1], cur_via->RECT[bot][2], cur_via->RECT[bot][3]);
    printf("%s %f %f %f %f\n", cur_via->LAYERS[mid], cur_via->RECT[mid][0], cur_via->RECT[mid][1], cur_via->RECT[mid][2], cur_via->RECT[mid][3]);
    printf("%s %f %f %f %f\n", cur_via->LAYERS[top], cur_via->RECT[top][0], cur_via->RECT[top][1], cur_via->RECT[top][2], cur_via->RECT[top][3]);
    printf("\n");
}
tech::tech(FILE* fp){
     char in[100];
    int mode = LAYER; // read layer info first

    head_layer = new tech_layer();
    tail_layer = head_layer;
    head_via = new via();
    tail_via = head_via;


    while (fscanf(fp, "%s", in)!=EOF && mode==LAYER){
        if(strcmp(in, "VIA")==0){
            mode = VIA;
            break;
        }

        if(strcmp(in, "MICRONS")==0)
            fscanf(fp, "%d", &database_micron);
        else if(strcmp(in, "MANUFACTURINGGRID")==0)
            fscanf(fp, "%lf", &MANUFACTURINGGRID);
        else if(strcmp(in, "LAYER")==0){
            fscanf(fp, "%s", tail_layer->NAME);
            while(1){
                fscanf(fp, "%s", in);
                if(strcmp(in, "END")==0){
                    tail_layer->next = new tech_layer();
                    tail_layer = tail_layer->next;
                    tail_layer->MAXWIDTH = 0;
                    tail_layer->OFFSET = 0;
                    tail_layer->PITCH = 0;
                    tail_layer->RESISTANCE_RPERSQ = 0;
                    tail_layer->SPACING = 0;
                    tail_layer->WIDTH = 0;
                    tail_layer->next = NULL;
                    strcpy(tail_layer->DIRECTION, "NoInfo");
                    strcpy(tail_layer->NAME, "NoInfo");
                    strcpy(tail_layer->TYPE, "NoInfo");
                    break;
                }
                if(strcmp(in, "TYPE")==0){
                    fscanf(fp, "%s", tail_layer->TYPE);
                }
                else if(strcmp(in, "WIDTH")==0){
                    fscanf(fp, "%d", &tail_layer->WIDTH);
                }
                else if(strcmp(in, "MAXWIDTH")==0){
                    fscanf(fp, "%d", &tail_layer->MAXWIDTH);
                }
                else if(strcmp(in, "SPACING")==0){
                    fscanf(fp, "%d", &tail_layer->SPACING);
                }
                else if(strcmp(in, "PITCH")==0){
                    fscanf(fp, "%d", &tail_layer->PITCH);
                }
                else if(strcmp(in, "OFFSET")==0){
                    fscanf(fp, "%d", &tail_layer->OFFSET);
                }
                else if(strcmp(in, "DIRECTION")==0){
                    fscanf(fp, "%s", tail_layer->DIRECTION);
                }
                else if(strcmp(in, "RESISTANCE")==0){
                    fscanf(fp, "%s", in);
                    fscanf(fp, "%s", in);
                    tail_layer->RESISTANCE_RPERSQ = atof(in);
                    char *temp;
                    temp = strtok(in ,"e");
                    double a = atof(temp);
                    temp = strtok(NULL ,"0");
                    temp = strtok(NULL ,"");
                    int b = atoi(temp)*(-1);
                    tail_layer->RESISTANCE_RPERSQ = (double)(a*pow(10,b));
                }
            }
        }
    }

    fscanf(fp, "%s", tail_via->NAME);
    int flag=1;
    int end=0;
    while (fscanf(fp, "%s", in)!=EOF && mode==VIA){
        if(strcmp(in, "RESISTANCE")==0){
            fscanf(fp, "%s", in);
            char *temp;
            temp = strtok(in ,"e");
            double a = atof(temp);
            temp = strtok(NULL ,"0");
            temp = strtok(NULL ,"");
            int b = atoi(temp)*(-1);
            tail_via->RESISTANCE = (double)(a*pow(10,b));
        }
        else if(strcmp(in, "LAYER")==0 && flag==1){
            fscanf(fp, "%s", tail_via->LAYERS[bot]);
            fscanf(fp, "%s", in);
            fscanf(fp, "%s", in);
            fscanf(fp, "%lf %lf %lf %lf", &tail_via->RECT[bot][0], &tail_via->RECT[bot][1], &tail_via->RECT[bot][2], &tail_via->RECT[bot][3]);
            flag = 2;
        }
        else if(strcmp(in, "LAYER")==0 && flag==2){
            fscanf(fp, "%s", tail_via->LAYERS[mid]);
            fscanf(fp, "%s", in);
            fscanf(fp, "%s", in);
            fscanf(fp, "%lf %lf %lf %lf", &tail_via->RECT[mid][0], &tail_via->RECT[mid][1], &tail_via->RECT[mid][2], &tail_via->RECT[mid][3]);
            flag = 3;
        }
        else if(strcmp(in, "LAYER")==0 && flag==3){
            fscanf(fp, "%s", tail_via->LAYERS[top]);
            fscanf(fp, "%s", in);
            fscanf(fp, "%s", in);
            fscanf(fp, "%lf %lf %lf %lf", &tail_via->RECT[top][0], &tail_via->RECT[top][1], &tail_via->RECT[top][2], &tail_via->RECT[top][3]);
            flag = 1;
        }
        else if(strcmp(in, "END")==0){
            if(end==1)
                continue;
            tail_via->next = new via();
            tail_via = tail_via->next;
            tail_via->next = NULL;
            end = 1;
        }
        else if(strcmp(in, "VIA")==0){
            end = 0;
            fscanf(fp, "%s", tail_via->NAME);
        }
    }
   // Print();
    fclose(fp);
    this->Database_micron = database_micron;
    this->ManuFACTURINGGRID = MANUFACTURINGGRID;
    this->Head_layer = head_layer;
    this->Cur_layer = cur_layer;
    this->Tail_layer = tail_layer;
    this->Head_via = head_via;
    this->Cur_via = cur_via;
    this->Tail_via = tail_via;

}
