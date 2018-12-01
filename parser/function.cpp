#include "function.h"

double distance_of_tow_point( coordinate p1, coordinate p2 ){

    double d = abs(p1.x-p2.x) + abs(p1.y-p2.y);
    cout << " d = " << d << endl;
    return d;
}
