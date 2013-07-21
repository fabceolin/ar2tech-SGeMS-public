/* -----------------------------------------------------------------------------
** Copyright (c) 2012 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of Advanced Resources and Risk Technology, LLC (AR2TECH)
** version of the open source software sgems.  It is a derivative work by
** AR2TECH (THE LICENSOR) based on the x-free license granted in the original
** version of the software (see notice below) and now sublicensed such that it
** cannot be distributed or modified without the explicit and written permission
** of AR2TECH.
**
** Only AR2TECH can modify, alter or revoke the licensing terms for this
** file/software.
**
** This file cannot be modified or distributed without the explicit and written
** consent of AR2TECH.
**
** Contact Dr. Alex Boucher (aboucher@ar2tech.com) for any questions regarding
** the licensing of this file/software
**
** The open-source version of sgems can be downloaded at
** sourceforge.net/projects/sgems.
** ----------------------------------------------------------------------------*/

#include <charts/multiple_charts_viewport.h>
#include <cmath>


std::pair<int,int> get_viewport_matrix(int number_of_charts) {

  int n_chart_x;
  int n_chart_y;

  if(number_of_charts == 1) {
    n_chart_x  =1;
    n_chart_y  =1;
  }
  else if(number_of_charts == 2) {
    n_chart_x  =2;
    n_chart_y  =1;
  }
  else if(number_of_charts == 3) {
    n_chart_x  =3;
    n_chart_y  =1;
  }
  else if(number_of_charts == 4) {
    n_chart_x  =2;
    n_chart_y  =2;
  }
  else if(number_of_charts < 6) {
    n_chart_x  =3;
    n_chart_y  =2;
  }
  else if(number_of_charts < 9) {
    n_chart_x  =3;
    n_chart_y  =3;
  }
  else if(number_of_charts < 12) {
    n_chart_x  =4;
    n_chart_y  =3;
  }
  else if(number_of_charts < 15) {
    n_chart_x  =5;
    n_chart_y  =3;
  }
  else if(number_of_charts < 20) {
    n_chart_x  =5;
    n_chart_y  =4;
  }
  else if(number_of_charts < 25) {
    n_chart_x  =5;
    n_chart_y  =5;
  }
  else if(number_of_charts < 30) {
    n_chart_x  =6;
    n_chart_y  =5;
  }
  else if(number_of_charts < 35) {
    n_chart_x  =7;
    n_chart_y  =5;
  }
  else {
    n_chart_x  =7;
    n_chart_y  = std::ceil((float)number_of_charts/7);
  }

  return std::make_pair(n_chart_x,n_chart_y);

}

std::vector<charts::viewport> get_charts_viewport(int number_of_charts) {

  std::pair<int,int> chart_matrix = get_viewport_matrix( number_of_charts);

  std::vector<charts::viewport> viewports;



  double x_inc = 1.0/chart_matrix.first;
  double y_inc = 1.0/chart_matrix.second;

  for(int j=chart_matrix.second-1; j>=0; --j) {
    double ymin = j*y_inc;
    double ymax = (j+1)*y_inc;
    for(int i=0; i<chart_matrix.first; ++i) {
      viewports.push_back(  charts::viewport( x_inc*i, ymin, x_inc*(i+1), ymax ) );
    }
  }

  return viewports;
}
