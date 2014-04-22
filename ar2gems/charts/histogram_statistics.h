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
//#include <vtkMath.h>
#include <vtkTable.h>
#include <vtkStringArray.h>
#include <vtkFloatArray.h>
#include <vtkVariantArray.h>
#include <vtkSmartPointer.h>

class Continuous_distribution;
class Non_parametric_distribution;

namespace charts {
  enum DescriptiveStats { DATA, GRID, N, MEAN, VARIANCE, MIN, MAX, SUM, SKEWNESS, KURTOSIS };
}

class CHARTS_DECL Continuous_statistics {
public:
  Continuous_statistics(  vtkSmartPointer<vtkVariantArray> desc_stats_array,
                          vtkSmartPointer<vtkVariantArray> quantile_stats_array,
                          vtkSmartPointer<vtkTable> histo_table,
                          vtkSmartPointer<vtkTable> histo_line_table,
                           vtkSmartPointer<vtkTable> cumulative_histo_table)
  {
    desc_stats_array_     =  desc_stats_array;
    quantile_stats_array_ = quantile_stats_array;
    histo_table_          = histo_table;
    histo_line_table_     = histo_line_table;
    cumulative_histo_table_ =  cumulative_histo_table;

    desc_stats_names_array_ = vtkSmartPointer<vtkStringArray>::New();
    desc_stats_names_array_->SetName("Stats");
    desc_stats_names_array_->SetNumberOfValues(10);
    desc_stats_names_array_->SetValue(charts::DATA, "");
    desc_stats_names_array_->SetValue(charts::GRID, "");
    desc_stats_names_array_->SetValue(charts::N, "N");
    desc_stats_names_array_->SetValue(charts::MEAN, "Mean");
    desc_stats_names_array_->SetValue(charts::VARIANCE, "Var");
    desc_stats_names_array_->SetValue(charts::MIN, "Min");
    desc_stats_names_array_->SetValue(charts::MAX, "Max");
    desc_stats_names_array_->SetValue(charts::SUM, "Sum");
    desc_stats_names_array_->SetValue(charts::SKEWNESS, "Skewness");
    desc_stats_names_array_->SetValue(charts::KURTOSIS, "Kurtosis");
  }

  int get_number_of_histo_bins() const {return histo_table_->GetNumberOfRows();}
  vtkSmartPointer<vtkVariantArray>    get_descriptive_statistics() { return desc_stats_array_;}
  vtkSmartPointer<vtkStringArray>     get_descriptive_statistics_name() { return desc_stats_names_array_;}
  vtkSmartPointer<vtkVariantArray>    get_quantile_statistics() { return quantile_stats_array_;}
  vtkSmartPointer<vtkTable>           get_histogram_table() { return histo_table_;}
  vtkSmartPointer<vtkTable>           get_histogram_width_table() { return histo_line_table_;}
  vtkSmartPointer<vtkTable>           get_cumulative_histogram_table() { return cumulative_histo_table_;}

  vtkSmartPointer<vtkTable> get_histogram_table(int nbins) {
    int number_of_max_bins = this->get_number_of_histo_bins();
    vtkSmartPointer<vtkTable> histo_nbins_table  = vtkSmartPointer<vtkTable>::New();
    vtkSmartPointer<vtkFloatArray> x = vtkSmartPointer<vtkFloatArray>::New();
    vtkSmartPointer<vtkFloatArray> freq = vtkSmartPointer<vtkFloatArray>::New();
    x->SetName( histo_table_->GetColumnName(0)  );
    freq->SetName( histo_table_->GetColumnName(1)  );
    histo_nbins_table->AddColumn(x);
    histo_nbins_table->AddColumn(freq);
    if( nbins%number_of_max_bins > 0) return histo_nbins_table;
    histo_nbins_table->SetNumberOfRows(nbins);
    int bins_steps = number_of_max_bins/nbins;
    int index = 0;
    for(int i=0; i> nbins; ++i) {
      float f = 0.0;
      float x_centered = 0.0;
      for(int j=0; j<bins_steps; ++j, ++index ) {
        x_centered += histo_table_->GetValue(index,0).ToFloat();
        f += histo_table_->GetValue(index,1).ToFloat();
      }
      x->SetValue(i,x_centered/bins_steps);
      freq->SetValue(i,f);
    }
    return histo_nbins_table;
  }

  vtkSmartPointer<vtkTable> get_histogram_width_table(int nbins) {return histo_line_table_;}

private:

  vtkSmartPointer<vtkStringArray>  desc_stats_names_array_;
  vtkSmartPointer<vtkVariantArray> desc_stats_array_;
  vtkSmartPointer<vtkVariantArray> quantile_stats_array_;
  vtkSmartPointer<vtkTable> histo_table_;
  vtkSmartPointer<vtkTable> histo_line_table_;
  vtkSmartPointer<vtkTable> cumulative_histo_table_;

};


CHARTS_DECL Continuous_statistics* build_histogram_table(int number_bins, const Grid_continuous_property* prop, const Grid_region* region, const Grid_filter* filter, float min, float max );
CHARTS_DECL Continuous_statistics* build_histogram_table(int number_bins, const Grid_continuous_property* prop, const std::vector<bool>& mask, float min, float max );
CHARTS_DECL Continuous_statistics* build_histogram_table(int number_bins, const Grid_continuous_property* prop, const Grid_weight_property* weights,const Grid_region* region, const Grid_filter* filter,bool normalized_weights , float min, float max );
CHARTS_DECL Continuous_statistics* build_histogram_table(int number_bins, const Grid_continuous_property* prop, const std::vector<float>& weights,const Grid_region* region, const Grid_filter* filter,bool normalized_weights , float min, float max );
CHARTS_DECL Continuous_statistics* build_histogram_table(int number_bins, vtkFloatArray* data,  float min, float max );
CHARTS_DECL Continuous_statistics* build_histogram_table(int number_bins, std::vector< std::pair<float, float> >& data_weights , std::string prop_name, std::string grid_name, float min, float max );
CHARTS_DECL Continuous_statistics* build_histogram_table(Non_parametric_distribution* distribution);
CHARTS_DECL Continuous_statistics* build_histogram_table(int number_bins, Continuous_distribution* distribution,  float min, float max );

class CHARTS_DECL accumulate_pairs {
public:
  template<typename T>
  float operator()(float sum, const std::pair<T,T>& pair) {
    return sum + pair.first;
  }
};

#endif
