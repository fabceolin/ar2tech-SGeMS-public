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



#ifndef CHART_CONTINUOUS_HISTOGRAM_H
#define CHART_CONTINUOUS_HISTOGRAM_H

#include <charts/chart_base.h>
#include <charts/common.h>
#include <charts/histogram_statistics.h>

#include <grid/grid_property.h>
#include <grid/grid_weight_property.h>
#include <grid/grid_region.h>
//#include <math/continuous_distribution.h>

#include <charts/histogram_proxy_model.h>
#include <charts/chart_widget.h>
#include <charts/chart_display_control.h>

//#include <vtkMath.h>
#include <vtkTable.h>
#include <vtkStringArray.h>
#include <vtkFloatArray.h>
#include <vtkSmartPointer.h>
#include <vtkQtTableView.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPlotBar.h>
#include <vtkPlotLine.h>
#include <vtkVariantArray.h>

#include <QColor>
#include <QToolBox>
#include <QTabWidget>
#include <QModelIndexList>
#include <QDropEvent>

class Continuous_distribution;

class CHARTS_DECL Chart_continuous_histogram : public Chart_base
{
  Q_OBJECT

public:
    Chart_continuous_histogram(int nbins = 55,QWidget *parent=0);
    virtual ~Chart_continuous_histogram();


public slots :

  void load_data(QModelIndexList indexes);

  void add_data(Histogram_item* prop_item);
  void update_data_display(Histogram_item* item);

  //Add the data to the model first which then send it to the chart
  void add_data( Grid_continuous_property* prop);
  void add_data( Grid_continuous_property* prop, Grid_weight_property* weigths);
  void add_data( Grid_continuous_property* prop, Grid_region* region);
  void add_data( Grid_property_group* group);
  void add_data( Grid_property_group* group, Grid_weight_property* weigths);
  void add_data( Grid_property_group* group, Grid_region* region);
  void add_data( Continuous_distribution* dist);
  void remove_data( int id);

  void set_visibility(Histogram_item* item);
  void set_color(Histogram_item* item);
  void set_data_filter(Histogram_item* item);
  void set_data_display_style(Histogram_item* item);
  void set_numbers_of_bins(int nbins);


	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

private:

  enum DescriptiveStats {  DATA, GRID, N, MEAN, VARIANCE, MIN, MAX, SUM, SKEWNESS, KURTOSIS };


  struct histo_data {

    histo_data() {
      stats = 0;
    }

    ~histo_data() {
      delete stats;
    }

    std::string name;
    Grid_continuous_property* prop;
    Grid_weight_property* weight;
    Grid_region* region;
    Continuous_distribution* dist;
    Geostat_grid* grid;
    float mean;
    float variance;
    float skewness;
    float median;
    float p10;
    float p90;
    Continuous_statistics* stats; 
    vtkSmartPointer<vtkPlot> plot_bar;
    vtkSmartPointer<vtkPlot> plot_line;
    int id;
  };

  void initialize_data(Histogram_property_item* item);
  //void initialize_data(Histogram_distribution_item* item);
  void initialize_plot(Histogram_item* item);


  void process_data(histo_data& data);
  void compute_stats(histo_data& data);
  void compute_stats_with_weights(histo_data& data);
  //void compute_stats_from_distribution(histo_data& data);

  void intialize_stats_tables();
  void add_data_to_stats_table(histo_data& data);
  void remove_data_to_stats_table(histo_data& data);

  void initialize_default_colors();

  void manage_plot_display(histo_data& data, QString display_style );

  void update_chart_display_control();

private:

  Histogram_proxy_model* model_;

  int nbins_;

  float min_values_;
  float max_values_;

  std::map<int, histo_data> data_stats_;

  vtkSmartPointer<vtkTable> histo_table_;
  vtkSmartPointer<vtkQtTableView> descriptive_stats_view_;
  vtkSmartPointer<vtkQtTableView> ordered_stats_view_;

  vtkSmartPointer<vtkTable> descriptive_stats_table_;
  vtkSmartPointer<vtkTable> ordered_stats_table_;

  Chart_widget* chart_widget_;

  QTabWidget* information_view_;
  Chart_display_control* chart_control_;

  QList<QColor> default_colors_;
  int default_color_id_;
  int max_index_default_colors_;
    
};


#endif // CHART_CONTINUOUS_HISTOGRAM_H
