
#include <charts/log_data_chart_creator.h>
#include <charts/log_data_chart.h>

#include <appli/manager_repository.h>
#include <grid/log_data_grid.h>
#include <grid/grid_filter.h>

#include <QDialog>
#include <QTreeView>
#include <QToolBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QModelIndexList>
#include <QGroupBox>
#include <QLabel>
#include <QDockWidget>
#include <QToolBar>
#include <QAction>
#include <QMdiSubWindow>
#include <QTextEdit>

Log_data_chart_creator::Log_data_chart_creator(Chart_mdi_area* mdi_area,QWidget *parent):Chart_creator(mdi_area,parent){

    
  QTabWidget* tab_widget = new QTabWidget(this);
  
  QWidget* param_container = new QWidget(this);
  QVBoxLayout* param_layout = new QVBoxLayout(param_container);
  grid_selector_ = new GridSelector(param_container);
  grid_selector_->setToolTip("Select grid with holes");

  // add the log_data selector
  log_data_selector_ = new SingleLogDataSelector(param_container);


  value_prop_selector_ = new SinglePropertySelector(param_container);
  value_prop_selector_->setToolTip("Select property with values");

  param_layout->addWidget(grid_selector_);
  param_layout->addWidget(log_data_selector_);
  param_layout->addWidget(new QLabel("Property",param_container));
  param_layout->addWidget(value_prop_selector_);
  param_layout->addStretch();

  param_container->setLayout(param_layout);
  tab_widget->addTab(param_container, QIcon(),"Parameters");

  tab_widget->addTab(this->build_help_page(), QIcon(),"Help");

  QVBoxLayout* main_layout = new QVBoxLayout(this);
  main_layout->setContentsMargins(0,0,0,0);
  main_layout->addWidget(tab_widget);
  QPushButton* show_button = new QPushButton("Display",this);
  main_layout->addWidget(show_button);
  this->setLayout(main_layout);

  connect( grid_selector_, SIGNAL(activated( const QString&)),value_prop_selector_, SLOT(show_properties( const QString&))   );
  connect( grid_selector_, SIGNAL(activated( const QString&)),log_data_selector_, SLOT(show_log_data( const QString&))   );
  connect( show_button, SIGNAL(clicked()),this, SLOT(show_log_data_display ())   );

}

void Log_data_chart_creator::show_log_data_display(){

  QString grid_name = grid_selector_->selectedGrid();
  QString value_name = value_prop_selector_->currentText();

  if(grid_name.isEmpty() || value_name.isEmpty() ) return;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + grid_name.toStdString() );
  Log_data_grid* log_grid = dynamic_cast<Log_data_grid*>( grid_ni.raw_ptr() );
  
  if(log_grid == 0) return;


  //TODO built the log data selector
  QString log_data_name = log_data_selector_->currentText();
  //QString log_data_name = QString::fromStdString(log_grid->get_log_name(0));

  int log_id = log_grid->get_log_id( log_data_name.toStdString() );
  if( log_id < 0 ) return;

  Log_data hole = log_grid->get_log_data( log_data_name.toStdString());

  Grid_continuous_property* value_prop = log_grid->property(value_name.toStdString());

  if(value_prop == 0 ) return;

  QString region_name = grid_selector_->selectedRegion();
  Grid_region* region = log_grid->region(region_name.toStdString());

  Grid_filter_region* region_filter = 0;
  if(region) {
    region_filter = new Grid_filter_region(region);
  } 

  Log_data_chart* chart = new Log_data_chart(&hole, value_prop, region_filter, this );

  QMdiSubWindow* sub_window = mdi_area_->addSubWindow(chart);
  sub_window->setGeometry(0,0,700,700);
  sub_window->setAttribute( Qt::WA_DeleteOnClose );
  sub_window->show();
}




QWidget* Log_data_chart_creator::build_help_page(){

  QTextEdit* txt_edit = new  QTextEdit( this );
  QTextDocument* doc = txt_edit->document();
  QTextCursor cursor(doc);
/*
  QTextCharFormat headline1_format = Report_window::headline1_format();
  QTextCharFormat headline2_format = Report_window::headline2_format();
  QTextCharFormat headline3_format = Report_window::headline3_format();
  QTextCharFormat body_format = Report_window::body_format();
  QTextTableFormat table_format = Report_window::table_format();
  */
  QTextBlockFormat header_format;
  header_format.setAlignment(Qt::AlignCenter);

  QTextBlockFormat text_format;
  text_format.setAlignment(Qt::AlignLeft);
  text_format.setLineHeight(150,QTextBlockFormat::ProportionalHeight);
  
  cursor.insertBlock(header_format);
  //cursor.insertText(QString("Hole analysis\n(Analysis of data from drillhole geometry)\n"), headline1_format);
  cursor.insertText(QString("Hole analysis\n(Analysis of data from drillhole geometry)\n") );

  cursor.insertBlock(text_format);
  QString str("blah blah blah\n");


 //cursor.insertText(  str, body_format);
 cursor.insertText(  str );

 return txt_edit;
}
