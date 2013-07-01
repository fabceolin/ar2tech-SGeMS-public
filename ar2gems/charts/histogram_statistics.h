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



#ifndef __HISTOGRAM_STATISTICS__
#define __HISTOGRAM_STATISTICS__

#include <charts/common.h>

#include <grid/grid_categorical_property.h>
#include <grid/grid_categorical_property.h>
#include <grid/grid_weight_property.h>
#include <grid/grid_filter.h>
#include <grid/grid_region.h>

#include <vtkDescriptiveStatistics.h> 
#include <vtkOrderStatistics.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkMath.h>
#include <vtkTable.h>
#include <vtkStringArray.h>
#include <vtkFloatArray.h>
#include <vtkVariantArray.h>
#include <vtkSmartPointer.h>

namespace charts {
  enum DescriptiveStats {  DATA, GRID, N, MEAN, VARIANCE, MIN, MAX, SUM, SKEWNESS, KURTOSIS };
}

class CHARTS_DECL Continuous_statistics {
public:
  Continuous_statistics(  vtkSmartPointer<vtkVariantArray> desc_stats_array,
                          vtkSmartPointer<vtkVariantArray> quantile_stats_array,
                          vtkSmartPointer<vtkTable> histo_table,
                          vtkSmartPointer<vtkTable> histo_line_table) :
        desc_stats_array_(desc_stats_array),
        quantile_stats_array_(quantile_stats_array),
        histo_table_(histo_table),
        histo_line_table_(histo_line_table){}


  vtkSmartPointer<vtkVariantArray> get_descriptive_statistics() { return desc_stats_array_;}
  vtkSmartPointer<vtkVariantArray> get_quantile_statistics() { return quantile_stats_array_;}
  vtkSmartPointer<vtkTable> get_histogram_table() {return histo_table_;}
  vtkSmartPointer<vtkTable> get_histogram_width_table() {return histo_line_table_;}


private:

  vtkSmartPointer<vtkVariantArray> desc_stats_array_;
  vtkSmartPointer<vtkVariantArray> quantile_stats_array_;
  vtkSmartPointer<vtkTable> histo_table_;
  vtkSmartPointer<vtkTable> histo_line_table_;

};


Continuous_statistics CHARTS_DECL build_histogram_table(int number_bins, const Grid_continuous_property* prop, const Grid_region* region, const Grid_filter* filter, float min, float max );

Continuous_statistics CHARTS_DECL build_histogram_table(int number_bins, const Grid_continuous_property* prop, const std::vector<bool>& mask, float min, float max );

Continuous_statistics CHARTS_DECL build_histogram_table(int number_bins, const Grid_continuous_property* prop, const Grid_weight_property* weights, 
                                            const Grid_region* region, const Grid_filter* filter,bool normalized_weights , float min, float max );

Continuous_statistics CHARTS_DECL build_histogram_table(int number_bins, const Grid_continuous_property* prop, const std::vector<float>& weights, 
                                            const Grid_region* region, const Grid_filter* filter,bool normalized_weights , float min, float max );

Continuous_statistics CHARTS_DECL build_histogram_table(int number_bins, std::vector< std::pair<float, float> >& data_weights , std::string prop_name, std::string grid_name, float min, float max );


class CHARTS_DECL accumulate_pairs {
public:
  template<typename T>
  float operator()(float sum, const std::pair<T,T>& pair) {
    return sum + pair.first;
  }
};

#endif