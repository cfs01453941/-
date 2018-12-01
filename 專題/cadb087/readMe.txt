netList : net的資料結構 主要用vector存

目前進度:
1. 已經將net全吃進netlist, 裡面有電壓, 平均長度, source pin的資訊, 以及一個專門存line的vector

2. line裡面有起始座標, 終點座標的vector , 用vector是因為終點可接到不同層. 電壓降, 電流, 線寬, 起始點與終點的fly line距離 , 專門存usage的vector, 終點pin的名字

3. usage有type (Vertical LINE:1  Horizontal LINE:2 VIA:3) ,起始層, 終點層(若via就會不同層),此資源占用的電阻數, 此資源的起始到終點

4.netList以及各個net裡皆已由小排到大

5.由於有些座標是沒除1000的,所以處理上要特別小心

結構如下
class USAGE{
public:
    int usage_type; //Vertical LINE:1  Horizontal LINE:2 VIA:3
    int src_layer,end_layer;
    double R;
    coordinate src, end;
};

class LINE{
public:
    coordinate src;
    vector<pin_metal> end;
    double drop,current,width,length;
    vector<USAGE> usage_list;
    string name;
};

class NET{
public:
    vector<LINE> line_list;
    double length;
    double voltage;
    Pins src;
};



