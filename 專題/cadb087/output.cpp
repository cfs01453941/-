#include "initial_files.h"
#include "output.h"
#include <iomanip>

output::output(vector<NET_o> netlist, Initial_file paser, string case_name){
    vector<NET_o> netlist_o;
    netlist_o.assign(netlist.begin(),netlist.end());
    NET_o N;
    fstream fp;
    fp.open(case_name+"_output.def", ios::out);
    fp<<"SPECIALNETS "<<netlist_o.size()<<" ;\n";
    vector <NET_o>::iterator net_i;
    vector <ELEMENT_o>::iterator element_i;
    vector <PATH_o>::iterator path_i;
    int flag=0;
    for(net_i = netlist_o.begin(); net_i != netlist_o.end(); net_i++ ){
        fp <<"- " << net_i->src.netName << endl;
        fp <<"  + FIXED " ;
        flag=1;
        for(path_i = net_i->path_list.begin(); path_i != net_i->path_list.end(); path_i++ ){
            for(element_i = path_i->element_list.begin(); element_i != path_i->element_list.end(); element_i++ ){
                if(element_i->type == "LINE"){
                    if(flag==1)
                        flag=0;
                    else
                        fp<<setprecision(7)<< "    NEW ";
                    if (element_i->line.begin.x == element_i->line.end.x)
                        fp <<"METAL"<<element_i->line.layer<<" "<<element_i->line.width*1000<<" + SHAPE STRIPE ( "<<element_i->line.begin.x*1000<<" "<<element_i->line.begin.y*1000<<" ) ( * "<<element_i->line.end.y*1000<<" )"<<endl;
                    else
                        fp <<"METAL"<<element_i->line.layer<<" "<<element_i->line.width*1000<<" + SHAPE STRIPE ( "<<element_i->line.begin.x*1000<<" "<<element_i->line.begin.y*1000<<" ) ( "<<element_i->line.end.x*1000<<" * )"<<endl;
                }
            }

        }
        for(path_i = net_i->path_list.begin(); path_i != net_i->path_list.end(); path_i++ ){

            for(element_i = path_i->element_list.begin(); element_i != path_i->element_list.end(); element_i++ ){
                if(element_i->type == "VIA"){
                    fp<< "    NEW ";
                    int top = element_i->via.layer_d>element_i->via.layer_s ? element_i->via.layer_d : element_i->via.layer_s;
                    fp <<"METAL"<<top<<" 0 ( "<<(element_i->via.lb.x+element_i->via.rt.x)/2*1000<<" "<<(element_i->via.lb.y+element_i->via.rt.y)/2*1000<<" ) "<<element_i->via.type<<endl;
                }
            }

        }
        fp<<" ;"<<endl;
    }
    fp<<"END SPECIALNETS"<<endl;
    fp.close();
}

