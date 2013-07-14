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



#include <charts/log_data_chart.h>
#include <grid/utilities.h> 
#include <qtplugins/rename_dialog.h>
#include <charts/histogram_statistics.h>

//#include "report_window.h"
#include <vtkDescriptiveStatistics.h> 
#include <vtkOrderStatistics.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkAxis.h>
#include <vtkColorSeries.h>
#include <vtkChartLegend.h>
#include <vtkPen.h>

#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QAbstractItemView>
#include <QTableView>
#include <QSplitter>
#include <QHeaderView>
#include <QTextDocumentWriter>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextBrowser>
#include <QTextFormat >
#include <QTextCharFormat>
#include <QTextTableFormat >
#include <QTextBlockFormat >
#include <QFileDialog>
#include <QSize>

Log_data_chart::Log_data_chart( const Log_data* log_data, Grid_continuous_property* value_prop, Grid_filter* filter, QWidget *parent)
  :Chart_base_display_controls(parent)
{

  QString title = QString("Analysis of hole %1 for %2").arg(log_data->name().c_str()).arg(value_prop->name().c_str());

  this->setWindowTitle(title);
  this->setWindowIcon(QPixmap(":/icons/appli/Pixmaps/ar2gems-icon-256x256.png"));


  qvtkWidget_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
  qvtkWidget_->resize(400,300);
  chart_control_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum));

  QSplitter* main_splitter = new QSplitter(Qt::Horizontal, this);
  QSplitter* chart_splitter = new QSplitter(Qt::Vertical, main_splitter);

  data_table_view_ = vtkSmartPointer<vtkQtTableView>::New();
  data_table_view_->GetWidget()->setParent(main_splitter);

  chart_control_->setParent(chart_splitter);

  /*
  QTabWidget* bottom_tab = new QTabWidget(main_splitter);
  bottom_tab->addTab(chart_control_,"Chart parameters");
  bottom_tab->addTab(data_table_view_->GetWidget(),"Statistics");
  bottom_tab->setCurrentIndex(1);
  */

  chart_splitter->addWidget( qvtkWidget_ );
  chart_splitter->addWidget( chart_control_ );
  //main_splitter->addWidget( bottom_tab );
  main_splitter->addWidget(chart_splitter );
  main_splitter->addWidget(data_table_view_->GetWidget() );

  chart_control_->hide_x_log_scale(true);
  chart_control_->set_xaxis_label("Distance from top");
  QString yaxis_name = QString::fromStdString(value_prop->name().c_str());
  chart_control_->set_yaxis_label(yaxis_name);
  chart_control_->set_title(title);
  chart_control_->set_legend_visibility( true );


  QVBoxLayout* layout = new QVBoxLayout(this);

  main_splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
  report_frame_->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));


  layout->addWidget(main_splitter);
  layout->addWidget(report_frame_);

  value_table_ = build_log_data_chart_table( log_data, value_prop, filter );

  data_table_view_->SetRepresentationFromInput(value_table_);
//  table_->setColumnHidden(0, true );
  data_table_view_->Update();
  QTableView* table = dynamic_cast<QTableView*>(data_table_view_->GetWidget());
//  table->setColumnHidden(1, true );
//  table->setColumnHidden(2, true );

  table->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  table->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  //data_view_->GetWidget()->update();

  this->intialize_plots();
  qvtkWidget_->update();


}


Log_data_chart::~Log_data_chart()
{

}
/*
void Log_data_chart::initialize_data_tables(const Log_data* log_data, Grid_continuous_property* value_prop, Grid_filter* filter){



  value_table_ = vtkSmartPointer<vtkTable>::New();

  vtkSmartPointer<vtkFloatArray> distance_array = vtkSmartPointer<vtkFloatArray>::New();
  distance_array->SetName(  "Length" );
  distance_array->SetNumberOfValues(log_data->number_of_segments()*2);
  

  vtkSmartPointer<vtkFloatArray> value_array = vtkSmartPointer<vtkFloatArray>::New();
  value_array->SetName( value_prop->name().c_str() );
  value_array->SetNumberOfValues(log_data->number_of_segments()*2);

  std::map<int, Log_data::Segment_geometry>::const_iterator it = log_data->segment_begin();
  for(int i=0 ; it != log_data->segment_end(); ++it, ++i) {
     distance_array->SetValue( 2*i, it->second.from );
     distance_array->SetValue( 2*i+1, it->second.to );
     float val = Grid_continuous_property::no_data_value;
     
     if(   value_prop->is_informed(it->second.nodeid) && (!filter ||  filter->is_valid_nodeid(it->second.nodeid) ) )   {
       val = value_prop->get_value(it->second.nodeid);
     }
     value_array->SetValue( 2*i, val );
     value_array->SetValue( 2*i+1, val );

  }

  value_table_->AddColumn(distance_array);
  value_table_->AddColumn(value_array);


}
*/
void Log_data_chart::intialize_plots(){


  log_data_plot_ = chart_->AddPlot(vtkChart::LINE);
  log_data_plot_->SetInputData(value_table_, 0, 1);


  chart_->Update();
  chart_->Modified();

  vtkAxis* yaxis = chart_->GetAxis(vtkAxis::LEFT);
  yaxis->Modified();
  double ymin = yaxis->GetMinimum();
  double ymax = yaxis->GetMaximum();

  vtkAxis* xaxis = chart_->GetAxis(vtkAxis::BOTTOM);
  double xmin = xaxis->GetMinimum();
  double xmax = xaxis->GetMaximum();  

}



vtkSmartPointer<vtkTable>  build_log_data_chart_table(const Log_data* log_data, const Grid_continuous_property* value_prop,  const Grid_filter* filter){

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
  vtkSmartPointer<vtkFloatArray> distance_array = vtkSmartPointer<vtkFloatArray>::New();
  distance_array->SetName(  "Distance" );
  distance_array->SetNumberOfValues(log_data->number_of_segments()*2);
  

  vtkSmartPointer<vtkFloatArray> value_array = vtkSmartPointer<vtkFloatArray>::New();
  value_array->SetName( value_prop->name().c_str() );
  value_array->SetNumberOfValues(log_data->number_of_segments()*2);

  std::map<int, Log_data::Segment_geometry>::const_iterator it = log_data->segment_begin();
  for(int i=0 ; it != log_data->segment_end(); ++it, ++i) {
     distance_array->SetValue( 2*i, it->second.from );
     distance_array->SetValue( 2*i+1, it->second.to );
     float val = Grid_continuous_property::no_data_value;
     
     if(   value_prop->is_informed(it->second.nodeid) && (!filter ||  filter->is_valid_nodeid(it->second.nodeid) ) )   {
       val = value_prop->get_value(it->second.nodeid);
     }
     value_array->SetValue( 2*i, val );
     value_array->SetValue( 2*i+1, val );

  }

  table->AddColumn(distance_array);
  table->AddColumn(value_array);

  return table;

}