/* -----------------------------------------------------------------------------
** Copyright© 2012 Advanced Resources and Risk Technology, LLC
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



#ifndef __LOG_DATA_CHART_H
#define __LOG_DATA_CHART_H

#include "common.h"

#include <charts/chart_base_display_controls.h>
#include <charts/chart_display_control.h>
#include <math/continuous_distribution.h>
#include <grid/grid_filter.h>
#include <grid/log_data_grid.h>

#include <vtkMath.h>
#include <vtkTable.h>
#include <vtkStringArray.h>
#include <vtkFloatArray.h>
#include <vtkSmartPointer.h>
#include <vtkQtTableView.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPlotBar.h>
#include <vtkPlotLine.h>
#include <vtkPlotPoints.h>
#include <vtkVariantArray.h>

#include <QColor>
#include <QToolBox>
#include <QTabWidget>
#include <QModelIndexList>
#include <QDropEvent>
#include <QTextDocument>
#include <vtkQtTableView.h>


vtkSmartPointer<vtkTable> CHARTS_DECL build_log_data_chart_table(const Log_data* log_data, Grid_continuous_property* value_prop,  Grid_filter* filter);


class CHARTS_DECL Log_data_chart : public Chart_base_display_controls 
{
  Q_OBJECT

public:
    Log_data_chart( const Log_data* log_data, Grid_continuous_property* value_prop, Grid_filter* filter, QWidget *parent=0);
    ~Log_data_chart();


public slots :

//  virtual void save_report();
//  virtual void view_report();
//  virtual void print_report();


private:

  enum DescriptiveStats {  DATA, N, MEAN, VARIANCE, MIN, MAX, SUM, SKEWNESS, KURTOSIS };

  void initialize_data_tables(const Log_data* log_data, Grid_continuous_property* value_prop, Grid_filter* filter);
  void intialize_plots();
  void set_axis_label();

private slots :

private:

  vtkSmartPointer<vtkPlot> log_data_plot_;

  

  vtkSmartPointer<vtkTable> value_stats_table_;
  vtkSmartPointer<vtkTable> value_table_;
  vtkSmartPointer<vtkTable> length_table_;
  vtkSmartPointer<vtkTable> length_stats_table_;

  vtkSmartPointer<vtkQtTableView> stats_table_view_;
  vtkSmartPointer<vtkQtTableView> data_table_view_;

  QTabWidget* information_view_;
  std::string name_grid_;
  std::string name_log_data_;
  std::string name_value_;
  std::string name_region_;
    
};



#endif // CHART_CONTINUOUS_HISTOGRAM_H
