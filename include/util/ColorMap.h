#ifndef _H_COLOR_MAP_
#define _H_COLOR_MAP_

#include <string.h>

class ColorMap {
  private:
    int temp;
    double t_r,t_g,t_b;


  public:
    ColorMap();
    ~ColorMap();

    void GetJet(double x, int*r,int*g,int*b );
    void GetHotToCold(double x, int*r,int*g,int*b );
};

#endif
