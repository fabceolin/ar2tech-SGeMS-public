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


#include <charts/chart_matrix_widget.h>

#include <vtkChartXY.h>
#include <vtkAxis.h>
#include <vtkTextProperty.h>
#include <vtkChartLegend.h>
#include <vtkScatterPlotMatrix.h>

#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QAbstractItemView>
#include <QFileDialog>

#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkJPEGWriter.h>

Chart_matrix_widget::Chart_matrix_widget( QWidget *parent)
  :  QVTKWidget(parent)
{

	context_view_ = vtkSmartPointer<vtkContextView>::New();
	context_view_->SetInteractor(this->GetInteractor());
	this->SetRenderWindow(context_view_->GetRenderWindow());
	chart_matrix_ = vtkSmartPointer<vtkChartMatrix>::New();
	context_view_->GetScene()->AddItem(chart_matrix_);  

}





Chart_matrix_widget::~Chart_matrix_widget()
{
}



void Chart_matrix_widget::set_controler(Chart_display_control* controler){

  chart_control_ = controler;

  connect( chart_control_, SIGNAL(xaxis_label_changed(const QString&)), this, SLOT(set_x_axis_label(const QString&)) );
  connect( chart_control_, SIGNAL(yaxis_label_changed(const QString&)), this, SLOT(set_y_axis_label(const QString&)) );
  connect( chart_control_, SIGNAL(title_changed(const QString&)), this, SLOT(set_title(const QString&)) );
  connect( chart_control_, SIGNAL(legend_display_changed(bool)), this, SLOT(set_legend(bool)) );
  connect( chart_control_, SIGNAL(grid_display_changed(bool)), this, SLOT(set_grid(bool)) );
  connect( chart_control_, SIGNAL(x_grid_display_changed(bool)), this, SLOT(set_x_grid(bool)) );
  connect( chart_control_, SIGNAL(y_grid_display_changed(bool)), this, SLOT(set_y_grid(bool)) );

  bool ok = connect( chart_control_, SIGNAL(x_axis_font_size(int)), this, SLOT(set_x_axis_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(y_axis_font_size(int)), this, SLOT(set_y_axis_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(x_label_font_size(int)), this, SLOT(set_x_label_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(y_label_font_size(int)), this, SLOT(set_y_label_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(legend_font_size(int)), this, SLOT(set_legend_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(title_font_size(int)), this, SLOT(set_title_font_size(int)) );

  connect( chart_control_, SIGNAL(xaxis_min_changed(double)), this, SLOT(set_xaxis_min(double)) );
  connect( chart_control_, SIGNAL(xaxis_max_changed(double)), this, SLOT(set_xaxis_max(double)) );
  connect( chart_control_, SIGNAL(xaxis_precision_changed(int)), this, SLOT(set_xaxis_precision(int)) );
  connect( chart_control_, SIGNAL(xaxis_nticks_changed(int)), this, SLOT(set_xaxis_nticks(int)) );
  connect( chart_control_, SIGNAL(xaxis_logscale_changed(bool)), this, SLOT(set_xaxis_logscale(bool)) );
  connect( chart_control_, SIGNAL(xaxis_autoscale_changed()), this, SLOT(set_xaxis_autoscale()) );

  connect( chart_control_, SIGNAL(yaxis_min_changed(double)), this, SLOT(set_yaxis_min(double)) );
  connect( chart_control_, SIGNAL(yaxis_max_changed(double)), this, SLOT(set_yaxis_max(double)) );
  connect( chart_control_, SIGNAL(yaxis_precision_changed(int)), this, SLOT(set_yaxis_precision(int)) );
  connect( chart_control_, SIGNAL(yaxis_nticks_changed(int)), this, SLOT(set_yaxis_nticks(int)) );
  connect( chart_control_, SIGNAL(yaxis_logscale_changed(bool)), this, SLOT(set_yaxis_logscale(bool)) );
  connect( chart_control_, SIGNAL(yaxis_autoscale_changed()), this, SLOT(set_yaxis_autoscale()) );

}

/*
void Chart_matrix_widget::update_chart_axis_display_control(){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
 

  vtkAxis* xaxis = chart_->GetAxis(vtkAxis::BOTTOM);

  double xmin = xaxis->GetMinimum();
  double xmax = xaxis->GetMaximum();
  if( chart_control_->is_x_axis_log_scale() ) {
    xmin = std::pow(xmin,10);
    xmax = std::pow(xmax,10);
  }
  chart_control_->set_xaxis_min(xmin);
  chart_control_->set_xaxis_max(xmax);
  chart_control_->set_xaxis_precision(xaxis->GetPrecision());
  chart_control_->set_xaxis_nticks(xaxis->GetNumberOfTicks());

  vtkAxis* yaxis = chart_->GetAxis(vtkAxis::LEFT);
  double ymin = yaxis->GetMinimum();
  double ymax = yaxis->GetMaximum();
  if( chart_control_->is_y_axis_log_scale() ) {
    ymin = std::pow(ymin,10);
    ymax = std::pow(ymax,10);
  }
  chart_control_->set_yaxis_min(ymin);
  chart_control_->set_yaxis_max(ymax);
  chart_control_->set_yaxis_precision(yaxis->GetPrecision());
  chart_control_->set_yaxis_nticks(yaxis->GetNumberOfTicks());
}
*/

void Chart_matrix_widget::set_x_axis_label(const QString& text){
  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(text.toStdString());
  }
  this->update();
}


void Chart_matrix_widget::set_y_axis_label(const QString& text){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::LEFT)->SetTitle(text.toStdString());
  }

  this->update();
}
void Chart_matrix_widget::set_title(const QString& text){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->SetTitle(text.toStdString().c_str());
  }

  this->update();
}
void Chart_matrix_widget::set_legend(bool on){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->SetShowLegend(on);
  }
  
  this->update();
}

void Chart_matrix_widget::set_grid(bool on){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(0)->SetGridVisible(on);
    chart->GetAxis(1)->SetGridVisible(on);
  }

  this->update();
}

void Chart_matrix_widget::set_x_grid(bool on){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(0)->SetGridVisible(on);
  }

  this->update();
}

void Chart_matrix_widget::set_y_grid(bool on){
  
  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(1)->SetGridVisible(on);
  } 

  this->update();
}

void Chart_matrix_widget::set_xaxis_min(double min){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::BOTTOM)->SetUnscaledMinimum(min);
    //chart->GetAxis(vtkAxis::BOTTOM)->SetUnscaledMinimumLimit(min);
  }

  this->update();
}
void Chart_matrix_widget::set_xaxis_max(double max){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::BOTTOM)->SetUnscaledMaximum(max);
   // chart->GetAxis(vtkAxis::BOTTOM)->SetUnscaledMaximumLimit(max);
  }

  this->update();
}
void Chart_matrix_widget::set_xaxis_precision(int digits){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::BOTTOM)->SetPrecision(digits);
  }
  this->update();
}
void Chart_matrix_widget::set_xaxis_nticks(int nticks){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::BOTTOM)->SetNumberOfTicks(nticks);
  }

  this->update();
}
void Chart_matrix_widget::set_xaxis_logscale(bool on){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::BOTTOM)->SetLogScale(on);
    //chart_control_->send_axis_signals();
  }

  
  this->update();
  //this->update_chart_axis_display_control();
}
void Chart_matrix_widget::set_xaxis_autoscale(){


  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::BOTTOM)->AutoScale();
    chart->Update();
  }
 
  this->update();
  //this->update_chart_axis_display_control();
}

void Chart_matrix_widget::set_yaxis_min(double min){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::LEFT)->SetUnscaledMinimum(min);
    //chart->GetAxis(vtkAxis::LEFT)->SetUnscaledMinimumLimit(min);
  }

  this->update();
}
void Chart_matrix_widget::set_yaxis_max(double max){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::LEFT)->SetUnscaledMaximum(max);
    //chart->GetAxis(vtkAxis::LEFT)->SetUnscaledMinimumLimit(max);
  }

  this->update();
}
void Chart_matrix_widget::set_yaxis_precision(int digits){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::LEFT)->SetPrecision(digits);
    chart->Update();
  }

  this->update();
}
void Chart_matrix_widget::set_yaxis_nticks(int nticks){
  
  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::LEFT)->SetNumberOfTicks(nticks);
    chart->Update();

  }
  
  this->update();
}

void Chart_matrix_widget::set_yaxis_logscale(bool on){
 
  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::LEFT)->SetLogScale(on);
    //chart_control_->send_axis_signals();
    chart->Update();
  }
  
  this->update();
//  this->update_chart_axis_display_control();
}
void Chart_matrix_widget::set_yaxis_autoscale(){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::LEFT)->AutoScale();
    chart->Update();
  }


  this->update();
  //this->update_chart_axis_display_control();
}

void Chart_matrix_widget::set_x_axis_font_size(int size){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::BOTTOM)->GetLabelProperties()->SetFontSize(size);
  }

  this->update();
}
void Chart_matrix_widget::set_y_axis_font_size(int size){

  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::LEFT)->GetLabelProperties()->SetFontSize(size);
  }

  this->update();
}
void Chart_matrix_widget::set_x_label_font_size(int size){
  
  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::BOTTOM)->GetTitleProperties()->SetFontSize(size);
  }
  
  this->update();
}
void Chart_matrix_widget::set_y_label_font_size(int size){
  
  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetAxis(vtkAxis::LEFT)->GetTitleProperties()->SetFontSize(size);
  } 

  this->update();
}
void Chart_matrix_widget::set_legend_font_size(int size){
  
  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetLegend()->SetLabelSize(size);
  }  

  this->update();
}
void Chart_matrix_widget::set_title_font_size(int size){
  
  int n_charts = chart_matrix_->GetNumberOfItems();
  for(int i=0; i< n_charts; ++i) {
    vtkChartXY* chart = vtkChartXY::SafeDownCast(chart_matrix_->GetItem( i ));
    chart->GetTitleProperties()->SetFontSize(size);
  } 
  
  this->update();
}


void Chart_matrix_widget::reset_axis(){

}

void Chart_matrix_widget::save_figure() {

  QString filename = 
    QFileDialog::getSaveFileName( this,"Save figure",chart_control_->get_title());

  if(filename.isEmpty()) return;

  if( !filename.endsWith(".png",Qt::CaseInsensitive) ||
      !filename.endsWith(".jpeg",Qt::CaseInsensitive) ||
      !filename.endsWith(".jpg",Qt::CaseInsensitive) ) 
  {
    filename.append(".png");
  }

  this->save_figure(filename);

}

void Chart_matrix_widget::save_figure(QString& filename,QSize plot_size ) {

  QApplication::setOverrideCursor( Qt::WaitCursor );
  
  QSize original_size = this->size();
  if(!plot_size.isEmpty()) {
    this->resize(plot_size);
  }


  vtkSmartPointer<vtkWindowToImageFilter> w2i = vtkSmartPointer<vtkWindowToImageFilter>::New();
  w2i->SetInput(context_view_->GetRenderWindow());
  w2i->Modified();
  
  QByteArray ba_filename = filename.toLatin1();


  if(  filename.endsWith( "PNG", Qt::CaseInsensitive ) ) {
    
    vtkSmartPointer<vtkPNGWriter> png = vtkSmartPointer<vtkPNGWriter>::New();
    png->SetInputConnection(w2i->GetOutputPort());
    png->SetFileName(ba_filename.data());
    png->Write();
  }

  if( filename.endsWith( "JPEG", Qt::CaseInsensitive ) || filename.endsWith( "JPG", Qt::CaseInsensitive ) ) {

    vtkSmartPointer<vtkJPEGWriter> jpeg = vtkSmartPointer<vtkJPEGWriter>::New();
    jpeg->SetInputConnection(w2i->GetOutputPort());
    jpeg->SetFileName(ba_filename.data());
    jpeg->Write();
  }

  if(!plot_size.isEmpty()) {
    this->resize(original_size);
  }

  QApplication::restoreOverrideCursor();

}

// ----------------------

Chart_scatter_matrix_widget::Chart_scatter_matrix_widget( QWidget *parent)
  :  Chart_matrix_widget(parent)
{


  context_view_->GetScene()->RemoveItem( chart_matrix_ );
	chart_matrix_ = vtkSmartPointer<vtkScatterPlotMatrix>::New();
	context_view_->GetScene()->AddItem(chart_matrix_);  

}

