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



#include <charts/chart_continuous_histogram.h>
#include <utils/manager_repository.h>
#include <grid/grid_region_temp_selector.h>
#include <qtplugins/color_delegate.h>
#include <charts/region_weight_delegate.h>
#include <charts/histogram_style_delegate.h>
#include <charts/histogram_item_tree_view.h>
#include <charts/histogram_statistics.h>
#include <math/continuous_distribution.h>

#include <vtkDescriptiveStatistics.h> 
#include <vtkOrderStatistics.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkAxis.h>
#include <vtkColorSeries.h>
#include <vtkColor.h>
#include <vtkTextProperty.h>
#include <vtkChartLegend.h>
#include <QVTKInteractor.h>
#include <vtkPlotLine.h>

#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QAbstractItemView>

Chart_continuous_histogram::Chart_continuous_histogram(int nbins, QWidget *parent)
  : nbins_(nbins), Chart_base(parent)
{
  this->setWindowTitle("Continuous Histogram");
  this->setWindowIcon(QIcon(":/icons/appli/Pixmaps/histo-bars.svg"));


    //Add the tree the control the selection
  QSplitter* mainSplitter = new QSplitter(Qt::Vertical, this);
	
  // Top part of the view
  QSplitter* chartSplitter = new QSplitter(Qt::Horizontal, mainSplitter);
  Histogram_item_tree_view* tree = new Histogram_item_tree_view(this);
  model_ = new Histogram_proxy_model(this);
  tree->setModel(model_);

  Color_delegate* color_delegate = new Color_delegate (this);
  tree->setItemDelegateForColumn(2,color_delegate);

  Region_weight_delegate* region_weight_delegate = new Region_weight_delegate (this);
  tree->setItemDelegateForColumn(1,region_weight_delegate);

  Continuous_histo_style_delegate* chart_style_delegate = new Continuous_histo_style_delegate (this);
  tree->setItemDelegateForColumn(3,chart_style_delegate);

  chartSplitter->addWidget(tree);

  //Add the vtk rendering window
  chart_widget_ = new Chart_widget( chartSplitter );

  chart_widget_->chart()->GetAxis(0)->SetNumberOfTicks(10);
  chart_widget_->chart()->GetAxis(1)->SetNumberOfTicks(10);

  chartSplitter->addWidget(chart_widget_);

  mainSplitter->addWidget(chartSplitter);

  descriptive_stats_view_ = vtkSmartPointer<vtkQtTableView>::New();
  ordered_stats_view_ = vtkSmartPointer<vtkQtTableView>::New();

  information_view_ = new QTabWidget(mainSplitter);
  chart_control_ = new Chart_display_control(this);
  information_view_->addTab(chart_control_,"Chart controls");

  information_view_->addTab(descriptive_stats_view_->GetWidget(),"Descriptive statistics");
  information_view_->addTab(ordered_stats_view_->GetWidget(),"Ordered statistics");



  mainSplitter->addWidget(information_view_);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mainSplitter);
  this->setLayout(layout);
    
  histo_table_ = vtkSmartPointer<vtkTable>::New();

  descriptive_stats_table_ = vtkSmartPointer<vtkTable>::New();
  ordered_stats_table_ = vtkSmartPointer<vtkTable>::New();

  this->intialize_stats_tables();
  this->initialize_default_colors();

  descriptive_stats_view_->SetRepresentationFromInput(descriptive_stats_table_);
  ordered_stats_view_->SetRepresentationFromInput(ordered_stats_table_);

  chart_widget_->set_controler(chart_control_);

  connect( model_, SIGNAL(data_added(Histogram_item*)), this, SLOT(add_data(Histogram_item*)) );
  connect( model_, SIGNAL(data_removed(Histogram_item*)), this, SLOT(remove_data(Histogram_item*)) );
  connect( model_, SIGNAL(data_filter_changed(Histogram_item*)), this, SLOT(set_data_filter(Histogram_item*)) );
  connect( model_, SIGNAL(data_display_changed(Histogram_item*)), this, SLOT(update_data_display(Histogram_item*)) );
  connect( model_, SIGNAL(data_color_changed(Histogram_item*)), this, SLOT(set_color(Histogram_item*)) );
  connect( model_, SIGNAL(data_visibility_changed(Histogram_item*)), this, SLOT(set_visibility(Histogram_item*)) );
  connect( model_, SIGNAL(display_format_changed(Histogram_item*)), this, SLOT(set_data_display_style(Histogram_item*)) );

  chart_control_->send_axis_signals();

}


Chart_continuous_histogram::~Chart_continuous_histogram()
{

}


void Chart_continuous_histogram::intialize_stats_tables(){

  vtkSmartPointer<vtkStringArray>  name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkStringArray>  grid_name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkIntArray>  n_array = vtkSmartPointer<vtkIntArray>::New();
  vtkSmartPointer<vtkFloatArray>  mean_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  var_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  min_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  max_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  sum_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  skew_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  kurt_array = vtkSmartPointer<vtkFloatArray>::New();
  name_array->SetName("Data");
  grid_name_array->SetName("Grid");
  n_array->SetName("N");
  mean_array->SetName("Mean");
  var_array->SetName("Variance");
  min_array->SetName("Min");
  max_array->SetName("Max");
  sum_array->SetName("Sum");
  skew_array->SetName("Skewness");
  kurt_array->SetName("Kurtosis");
  descriptive_stats_table_->AddColumn(name_array);
  descriptive_stats_table_->AddColumn(grid_name_array);
  descriptive_stats_table_->AddColumn(n_array);
  descriptive_stats_table_->AddColumn(mean_array);
  descriptive_stats_table_->AddColumn(var_array);
  descriptive_stats_table_->AddColumn(min_array);
  descriptive_stats_table_->AddColumn(max_array);
  descriptive_stats_table_->AddColumn(sum_array);
  descriptive_stats_table_->AddColumn(skew_array);
  descriptive_stats_table_->AddColumn(kurt_array);

  vtkSmartPointer<vtkStringArray> p_name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkStringArray> g_name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkFloatArray>  p_min_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p_max_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p10_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p20_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p30_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p40_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p50_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p60_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p70_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p80_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p90_array = vtkSmartPointer<vtkFloatArray>::New();
  p_name_array->SetName("Data");
  g_name_array->SetName("Grid");
  p_min_array->SetName("Min");
  p_max_array->SetName("Max");
  p10_array->SetName("P10");
  p20_array->SetName("P20");
  p30_array->SetName("P30");
  p40_array->SetName("P40");
  p50_array->SetName("Median");
  p60_array->SetName("P60");
  p70_array->SetName("P70");
  p80_array->SetName("P80");
  p90_array->SetName("P90");
  ordered_stats_table_->AddColumn(p_name_array);
  ordered_stats_table_->AddColumn(g_name_array);
  ordered_stats_table_->AddColumn(p_min_array);
  ordered_stats_table_->AddColumn(p10_array);
  ordered_stats_table_->AddColumn(p20_array);
  ordered_stats_table_->AddColumn(p30_array);
  ordered_stats_table_->AddColumn(p40_array);
  ordered_stats_table_->AddColumn(p50_array);
  ordered_stats_table_->AddColumn(p60_array);
  ordered_stats_table_->AddColumn(p70_array);
  ordered_stats_table_->AddColumn(p80_array);
  ordered_stats_table_->AddColumn(p90_array);
  ordered_stats_table_->AddColumn(p_max_array);

}

void Chart_continuous_histogram::initialize_default_colors(){


  vtkSmartPointer<vtkColorSeries> colors = vtkSmartPointer<vtkColorSeries>::New();
  colors->SetColorScheme(vtkColorSeries::WILD_FLOWER);
  for( int i=0; i<colors->GetNumberOfColors(); ++i ) {
    vtkColor3ub c3ub	= colors->GetColor (i);
    const unsigned char r = c3ub.GetRed();
    const unsigned char g = c3ub.GetGreen();
    const unsigned char b = c3ub.GetBlue();

    default_colors_.append(QColor(int(r),int(g),int(b)));
  }

  
  colors->SetColorScheme(vtkColorSeries::CITRUS);
  for( int i=0; i<colors->GetNumberOfColors(); ++i ) {
    vtkColor3ub c3ub	= colors->GetColor (i);
    const unsigned char r = c3ub.GetRed();
    const unsigned char g = c3ub.GetGreen();
    const unsigned char b = c3ub.GetBlue();

    default_colors_.append(QColor((int)r,int(g),int(b)));
  }

  colors->SetColorScheme(vtkColorSeries::SPECTRUM);

  for( int i=0; i<colors->GetNumberOfColors(); ++i ) {
    vtkColor3ub c3ub	= colors->GetColor (i);
    const unsigned char r = c3ub.GetRed();
    const unsigned char g = c3ub.GetGreen();
    const unsigned char b = c3ub.GetBlue();

    default_colors_.append(QColor((int)r,int(g),int(b)));
  }

  default_color_id_ = 0;
  max_index_default_colors_ = default_colors_.size()-1;
 

}


void Chart_continuous_histogram::load_data(QModelIndexList indexes){
      //Check if a region has been selected for a specific grid

  std::map<GsTL_object_item*,Grid_region*> grid_to_region;

  typedef std::multimap<GsTL_object_item*,Grid_weight_property*> map_grid_to_weights;
  typedef std::multimap<GsTL_object_item*,Grid_weight_property*>::iterator map_iterator_grid_to_weights;
  map_grid_to_weights grid_to_weights;
  QModelIndex index;
  foreach(index, indexes) {
//  for(int i=0; i<indexes.size(); ++i) {
  //  QModelIndex index =     indexes.at(i);
    GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
    if(item->item_type() == "Region") {
      Grid_region* region = static_cast<Grid_region*>(index.internalPointer());
      grid_to_region[region->parent()->parent()] = region;
    }
    else if(item->item_type() == "WeightsProperty") {
      Grid_weight_property* weigth = static_cast<Grid_weight_property*>(index.internalPointer());
      grid_to_weights.insert(std::make_pair(weigth->parent()->parent(),weigth));
    }
  }


//  Send the data to the chart
//  foreach(index, indexes) {
  for(int i=0; i<indexes.size(); ++i) {
    QModelIndex index = indexes.at(i);
    GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
    //if(item->item_type() == "ContinuousProperty" || item->item_type() == "WeightsProperty") {
    if(item->item_type() == "ContinuousProperty" ) {
      Grid_continuous_property* prop = static_cast<Grid_continuous_property*>(item->data_pointer());
      GsTL_object_item* parent_grid = prop->parent()->parent();
  
      std::map<GsTL_object_item*,Grid_region*>::iterator it = grid_to_region.find(parent_grid);
      Grid_region* region = 0;
      if(it != grid_to_region.end()) region = it->second;
      model_->insert_row(prop,region, default_colors_.at(default_color_id_%max_index_default_colors_));
      default_color_id_++;

      if(grid_to_weights.count(parent_grid) != 0) {
        //load one set of property per weights
        std::pair<map_iterator_grid_to_weights,map_iterator_grid_to_weights> it_pair = grid_to_weights.equal_range(parent_grid);
        for( ; it_pair.first != it_pair.second; ++ it_pair.first ) {

          model_->insert_row(prop,it_pair.first->second, default_colors_.at(default_color_id_%max_index_default_colors_));
          default_color_id_++;

        }

      }


      //this->add_data(prop,region);
    }
    else if(item->item_type().contains("Group:")) {
      Grid_property_group* group = static_cast<Grid_property_group*>(index.internalPointer());
      std::map<GsTL_object_item*,Grid_region*>::iterator it = grid_to_region.find(group->parent());
      Grid_region* region = 0;
      if(it != grid_to_region.end()) region = it->second;
      model_->insert_row(group,region,default_colors_.at(default_color_id_%max_index_default_colors_));
      default_color_id_++;
    }
    else if(item->item_type() == "ContinuousDistribution") {
      Continuous_distribution* dist = static_cast<Continuous_distribution*>(index.internalPointer());
      model_->insert_row(dist,default_colors_.at(default_color_id_%max_index_default_colors_));
      default_color_id_++;
    }
  }
}

void Chart_continuous_histogram::add_data( Grid_continuous_property* prop){
  model_->insert_row(prop, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( Grid_continuous_property* prop, Grid_weight_property* weigths){
  model_->insert_row(prop,weigths, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( Grid_continuous_property* prop, Grid_region* region){
  model_->insert_row(prop,region, default_colors_.at(default_color_id_%max_index_default_colors_) );
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( Grid_property_group* group){
  model_->insert_row(group, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( Grid_property_group* group, Grid_weight_property* weigths){
  model_->insert_row(group,weigths, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( Grid_property_group* group, Grid_region* region){
  model_->insert_row(group,region, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( Continuous_distribution* dist){
  model_->insert_row(dist, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data(Histogram_item* item){
    if(item->type() == "Property") {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(item);
      this->initialize_data(prop_item);
      this->initialize_plot(prop_item);

      std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
      std::cout<<"set color and style"<<std::endl;
      it->second.plot_bar->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
      it->second.plot_line->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());

    }
    else if(item->type() == "Group") {
      Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
        this->add_data(prop_item);
      }
    }
    /*
    else if(item->type() == "ContinuousDistribution") {
      Histogram_distribution_item* dist_item = dynamic_cast<Histogram_distribution_item*>(item);
      this->initialize_data(dist_item);
      this->initialize_plot(dist_item);

      std::map<int, histo_data>::iterator it = data_stats_.find(dist_item->id());
      it->second.plot_bar->SetColor( dist_item->color().red(),dist_item->color().green(),dist_item->color().blue(), dist_item->color().alpha());
      it->second.plot_line->SetColor( dist_item->color().red(),dist_item->color().green(),dist_item->color().blue(), dist_item->color().alpha());

    }
    */
}



void Chart_continuous_histogram::update_data_display(Histogram_item* item){
    if(item->type() == "Property" || item->type() == "ContinuousDistribution") {
      //Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(item);

      std::map<int, histo_data>::iterator it = data_stats_.find(item->id());
      it->second.plot_bar->SetColor( item->color().red(),item->color().green(),item->color().blue(), item->color().alpha());
      it->second.plot_line->SetColor( item->color().red(),item->color().green(),item->color().blue(), item->color().alpha());
      
      if( !item->is_visible() ) {
        it->second.plot_bar->SetVisible(false);
        it->second.plot_line->SetVisible(false);
      }
      else {
        this->manage_plot_display(it->second, item->display_format());
      }
    }
    else if(item->type() == "Group") {
      Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
        this->update_data_display(prop_item);

      }
    }
    chart_widget_->update();
}
/*
void Chart_continuous_histogram::initialize_data(Histogram_distribution_item* item){

  std::map<int, histo_data>::iterator it = data_stats_.find(item->id());

  histo_data data;
  data.name = item->item_name().toStdString();

  if(it == data_stats_.end()) {
    data.id = -1;
    data_stats_.insert(std::make_pair(item->id(),data));
    it = data_stats_.find(item->id());
  }

  this->compute_stats_from_distribution(it->second);


  this->add_data_to_stats_table(it->second);

}
*/

void Chart_continuous_histogram::initialize_data(Histogram_property_item* item){
  
  std::map<int, histo_data>::iterator it = data_stats_.find(item->id());

  histo_data data;
  data.name = item->prop()->name();
  data.grid = static_cast<Geostat_grid*>(item->prop()->parent()->parent());

  if(it == data_stats_.end()) {
    data.prop = item->prop();
    data.region = item->region();
    data.weight =item->weights();
    data.id = -1;
    data_stats_.insert(std::make_pair(item->id(),data));
    it = data_stats_.find(item->id());
  }
  else {  // Assume that we need to recompute
    it->second.region = item->region();
    it->second.weight = item->weights();
  }

  //Find how to compute the stats
  if( it->second.weight ==0 ) {
    if(data.region !=0 ) {
      it->second.name += " ("+data.region->name()+")";
    }
    this->compute_stats(it->second);
  }
  else {
    it->second.name += " ("+data.weight->name()+")";
    this->compute_stats_with_weights(it->second);
  }

  this->add_data_to_stats_table(it->second);

  //TODO: Need to add the line format and color
 // data_stats_[item->id()] = data;
}

void Chart_continuous_histogram::initialize_plot(Histogram_item* item){

  std::map<int, histo_data>::iterator it = data_stats_.find(item->id());

  it->second.plot_bar = chart_widget_->chart()->AddPlot(vtkChart::BAR); // LINE, POINTS, BAR, STACKED
  it->second.plot_line = chart_widget_->chart()->AddPlot(vtkChart::LINE);
  it->second.plot_bar->GetYAxis()->SetMinimumLimit(0.00);
  it->second.plot_line->GetYAxis()->SetMinimumLimit(0.00);
  it->second.plot_bar->GetYAxis()->SetMaximumLimit(1.00);
  it->second.plot_line->GetYAxis()->SetMaximumLimit(1.00);

//  this->manage_plot_display(it->second, item->display_format());

  it->second.plot_bar->SetInputData(it->second.stats->get_histogram_table(), 0, 1);
  it->second.plot_line->SetInputData(it->second.stats->get_histogram_width_table(), 0, 1);
  QColor color = default_colors_.at( default_color_id_%max_index_default_colors_ );
  it->second.plot_bar->SetColor(color.redF(), color.greenF(), color.blueF());
  it->second.plot_line->SetColor(color.redF(), color.greenF(), color.blueF());
  it->second.plot_bar->Update();
  it->second.plot_line->Update();

  this->update_chart_display_control();
}

void Chart_continuous_histogram::remove_data( int id){

  std::map<int, histo_data>::iterator it = data_stats_.find(id);

  chart_widget_->chart()->RemovePlotInstance(it->second.plot_bar);
  chart_widget_->chart()->RemovePlotInstance(it->second.plot_line);
  histo_table_->RemoveColumnByName (it->second.name.c_str());
  data_stats_.erase(it);
}


void Chart_continuous_histogram::process_data(histo_data& data){

}


void Chart_continuous_histogram::compute_stats(histo_data& data){

  delete data.stats;
  data.stats = build_histogram_table(50,data.prop,data.region,0,0,0);

}


void Chart_continuous_histogram::compute_stats_with_weights(histo_data& data){
  
  delete data.stats;
  data.stats = build_histogram_table(50,data.prop,data.weight,data.region,0,false,0,0);

}

/*
void Chart_continuous_histogram::compute_stats_from_distribution(histo_data& data){

}
*/
void Chart_continuous_histogram::add_data_to_stats_table(histo_data& data){

  //check if the entry already exist in the table
  int irow = data.id; 
  bool create_row = false;
  if(data.id == -1) {
      irow = descriptive_stats_table_->InsertNextBlankRow();
      data.id= irow;
      create_row = true;
  }
  descriptive_stats_table_->SetRow(irow, data.stats->get_descriptive_statistics());
  descriptive_stats_table_->Modified();


  //check if the entry already exist in the table
  if(create_row) {
      ordered_stats_table_->InsertNextBlankRow();
  }
  ordered_stats_table_->SetRow(irow, data.stats->get_quantile_statistics());
  ordered_stats_table_->Modified();

  // Does not update by iteslf;  have to reset the Representation, something strange here
  descriptive_stats_view_->SetRepresentationFromInput(descriptive_stats_table_);
  descriptive_stats_view_->Update();
  QAbstractItemView* desc_view = dynamic_cast<QAbstractItemView*>(descriptive_stats_view_->GetWidget());
  desc_view->reset();
  
  ordered_stats_view_->SetRepresentationFromInput(ordered_stats_table_);
  ordered_stats_view_->Update();
  ordered_stats_view_->GetWidget()->update();


}


void Chart_continuous_histogram::remove_data_to_stats_table(histo_data& data){

}



void Chart_continuous_histogram::set_visibility( Histogram_item* item){
  if(item->type() == "Property") {
    Histogram_property_item* prop_item = dynamic_cast< Histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    it->second.plot_bar->SetVisible(prop_item->is_visible());
    it->second.plot_line->SetVisible(prop_item->is_visible());

  }
  else if(item->type() == "Group") {
    Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
      this->set_visibility(prop_item);
    }
  }
  chart_widget_->update();
}
void Chart_continuous_histogram::set_color( Histogram_item* item){
  if(item->type() == "Property") {
    Histogram_property_item* prop_item = dynamic_cast< Histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    it->second.plot_bar->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
    it->second.plot_line->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
    it->second.plot_bar->Update();
    it->second.plot_line->Update();
  }
  else if(item->type() == "Group") {
    Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
      std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
      if( it == data_stats_.end() ) return;
      it->second.plot_bar->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
      it->second.plot_line->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
      it->second.plot_bar->Update();
      it->second.plot_line->Update();
    }
  }
}

void Chart_continuous_histogram::set_data_filter(Histogram_item* item){

  if(item->type() == "Property") {
    Histogram_property_item* prop_item = dynamic_cast< Histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    bool changed = false;
    if( it->second.region != item->region() ) {
      it->second.region = item->region();
      changed = true;
    }
    if( it->second.weight != item->weights() ) {
      it->second.weight = item->weights();
      changed = true;
    }
    // Get the stats recomputed
    if(changed) {
    if(it->second.weight==0) {
      if(it->second.region == 0) {
        it->second.name = it->second.prop->name();
      } else {
        it->second.name = it->second.prop->name() + " ("+it->second.region->name()+")";
      }
      this->compute_stats(it->second);
    }
    else {  // has weights
      it->second.name = it->second.prop->name() + " ("+it->second.weight->name()+")";
      this->compute_stats_with_weights(it->second);

    }
    this->add_data_to_stats_table(it->second);

    it->second.plot_bar->SetInputData(it->second.stats->get_histogram_table(), 0, 1);
    it->second.plot_bar->Update();
    it->second.plot_line->SetInputData(it->second.stats->get_histogram_width_table(), 0, 1);
    it->second.plot_line->Update();
    chart_widget_->update();

    }

  }
  else if(item->type() == "Group") {
    Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
      this->set_data_filter(prop_item);
    }
  }

}

void Chart_continuous_histogram::set_data_display_style(Histogram_item* item){

  if(item->type() == "Property") {
    Histogram_property_item* prop_item = dynamic_cast< Histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    this->manage_plot_display(it->second, prop_item->display_format());

  }
  else if(item->type() == "Group") {
    Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
      this->set_data_display_style(prop_item);
    }
  }

}

void Chart_continuous_histogram::manage_plot_display(histo_data& data, QString display_style ){
  bool changed = false;
  if(display_style == "Bars") {
    data.plot_line->SetVisible(false);
    data.plot_bar->SetVisible(true);
    changed = true;
  }
  else if(display_style == "Lines") {
    data.plot_line->SetVisible(true);
    data.plot_bar->SetVisible(false);
    changed = true;
  }
  else if(display_style == "Bars and Lines") {
    data.plot_line->SetVisible(true);
    data.plot_bar->SetVisible(true);
    changed = true;
  }
  if(changed) {
    this->update_chart_display_control();
    chart_widget_->update();
  }
}

void Chart_continuous_histogram::update_chart_display_control(){
  vtkAxis* xaxis = chart_widget_->chart()->GetAxis(vtkAxis::BOTTOM);
  chart_control_->set_xaxis_min(xaxis->GetMinimum());
  chart_control_->set_xaxis_max(xaxis->GetMaximum());
  chart_control_->set_xaxis_precision(xaxis->GetPrecision());
  chart_control_->set_xaxis_nticks(xaxis->GetNumberOfTicks());

  vtkAxis* yaxis = chart_widget_->chart()->GetAxis(vtkAxis::LEFT);
  chart_control_->set_yaxis_min(yaxis->GetMinimum());
  chart_control_->set_yaxis_max(yaxis->GetMaximum());
  chart_control_->set_yaxis_precision(yaxis->GetPrecision());
  chart_control_->set_yaxis_nticks(yaxis->GetNumberOfTicks());
}

void Chart_continuous_histogram::set_numbers_of_bins(int nbins){

}


void Chart_continuous_histogram::dragEnterEvent(QDragEnterEvent *event)
{

	if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
		event->acceptProposedAction();
	}
}

void Chart_continuous_histogram::dropEvent(QDropEvent *event)
{
	QByteArray encoded = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);

	QStringList objectNames;
	while (!stream.atEnd())
	{
	  int row, col;
	  QMap<int,  QVariant> roleDataMap;
	  stream >> row >> col >> roleDataMap;

    QString name = roleDataMap.value(0).toString();
    QString type = roleDataMap.value(1).toString();

    Geostat_grid* grid = 0;
    if(type != "ContinuousDistribution") {
      QString grid_name = roleDataMap.value(2).toString();
      QString s4 = roleDataMap.value(3).toString();

      grid = dynamic_cast<Geostat_grid*>( 
		              Root::instance()->interface( 
                  gridModels_manager + "/" + grid_name.toStdString()
					        ).raw_ptr()
		  );
      if(grid == 0) continue;
    }

    

    if(type == "ContinuousProperty" || type == "WeightProperty" ) {
      this->add_data( grid->property(name.toStdString()) );
    }
    else if(type.startsWith("Group:")) {
      this->add_data( grid->get_group(name.toStdString()) );
    }
    else if(type == "ContinuousDistribution") {
      Continuous_distribution* dist = dynamic_cast<Continuous_distribution*>( 
		              Root::instance()->interface( 
                  continuous_distributions_manager + "/" + name.toStdString()
					        ).raw_ptr());
      if(dist == 0) continue;
      this->add_data(  dist );
    }
	}
  
}
