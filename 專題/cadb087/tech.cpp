#include "tech.h"
#include <string.h>
int database_micron;
double MANUFACTURINGGRID;

using namespace std;


tech::tech(FILE* fp){
    char in[100];
    int mode = LAYER; // read layer info first

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
            tech_layer temp;
            fscanf(fp, "%s", temp.NAME);
            temp.layer = temp.NAME[strlen(temp.NAME) - 1] - '0';
            while(1){
                fscanf(fp, "%s", in);
                if(strcmp(in, "END")==0){
                    this->layer_list.push_back(temp);
                    break;
                }
                if(strcmp(in, "TYPE")==0){
                    fscanf(fp, "%s", temp.TYPE);
                }
                else if(strcmp(in, "WIDTH")==0){
                    fscanf(fp, "%d", &temp.WIDTH);
                }
                else if(strcmp(in, "MAXWIDTH")==0){
                    fscanf(fp, "%d", &temp.MAXWIDTH);
                }
                else if(strcmp(in, "SPACING")==0){
                    fscanf(fp, "%d", &temp.SPACING);
                }
                else if(strcmp(in, "PITCH")==0){
                    fscanf(fp, "%d", &temp.PITCH);
                }
                else if(strcmp(in, "OFFSET")==0){
                    fscanf(fp, "%d", &temp.OFFSET);
                }
                else if(strcmp(in, "DIRECTION")==0){
                    fscanf(fp, "%s", temp.DIRECTION);
                }
                else if(strcmp(in, "RESISTANCE")==0){
                    fscanf(fp, "%s", in);
                    fscanf(fp, "%s", in);
                    char *t;
                    t = strtok(in ,"e");
                    double a = atof(t);
                    t = strtok(NULL ,"0");
                    t = strtok(NULL ,"");
                    int b = atoi(t)*(-1);
                    temp.RESISTANCE_RPERSQ = (double)(a*pow(10,b));
                }
            }
        }
    }
    vector<tech_layer>::iterator fuck;
    tech_via temp; ///via der temp ok? boom~
    fscanf(fp, "%s", temp.NAME);
    int flag=1;
    int end=0;
    while (fscanf(fp, "%s", in)!=EOF && mode==VIA){
        if(strcmp(in, "RESISTANCE")==0){
            fscanf(fp, "%s", in);
            char *t;
            t = strtok(in ,"e");
            double a = atof(t);
            t = strtok(NULL ,"0");
            t = strtok(NULL ,"");
            int b = atoi(t)*(-1);
           temp.RESISTANCE = (double)(a*pow(10,b));
        }
        else if(strcmp(in, "LAYER")==0 && flag==1){
            fscanf(fp, "%s", temp.LAYERS[bot]);
            fscanf(fp, "%s", in);
            fscanf(fp, "%s", in);
            fscanf(fp, "%lf %lf %lf %lf", &temp.RECT[bot][0], &temp.RECT[bot][1], &temp.RECT[bot][2], &temp.RECT[bot][3]);
            flag = 2;
        }
        else if(strcmp(in, "LAYER")==0 && flag==2){
            fscanf(fp, "%s", temp.LAYERS[mid]);
            fscanf(fp, "%s", in);
            fscanf(fp, "%s", in);
            fscanf(fp, "%lf %lf %lf %lf", &temp.RECT[mid][0], &temp.RECT[mid][1], &temp.RECT[mid][2], &temp.RECT[mid][3]);
            flag = 3;
            temp.layer[0] = temp.LAYERS[mid][strlen(temp.LAYERS[mid])-2] - '0';
            temp.layer[1] = temp.LAYERS[mid][strlen(temp.LAYERS[mid])-1] - '0';
        }
        else if(strcmp(in, "LAYER")==0 && flag==3){
            fscanf(fp, "%s", temp.LAYERS[yop]);
            fscanf(fp, "%s", in);
            fscanf(fp, "%s", in);
            fscanf(fp, "%lf %lf %lf %lf", &temp.RECT[yop][0], &temp.RECT[yop][1], &temp.RECT[yop][2], &temp.RECT[yop][3]);
            flag = 1;
        }
        else if(strcmp(in, "END")==0){
            if(end==1)
                continue;
            this->via_list.push_back(temp);
            end = 1;
        }
        else if(strcmp(in, "VIA")==0){
            end = 0;
            fscanf(fp, "%s", temp.NAME);
        }
    }
    fclose(fp);
    this->Database_micron = database_micron;
    this->ManuFACTURINGGRID = MANUFACTURINGGRID;
}
