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


#include <gui/utils/tie_breaking_dialog.h>
#include <qtplugins/selectors.h>
#include <qtplugins/ellipsoid_input.h>
#include <utils/gstl_messages.h>
#include <utils/error_messages_handler.h>
#include <actions/common.h>
#include <appli/action.h>
#include <grid/geostat_grid.h>
#include <utils/manager_repository.h>
#include <appli/project.h>

#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QButtonGroup>


Tie_breaking_random_dialog::
Tie_breaking_random_dialog( GsTL_project* proj,
                    QWidget* parent )
{

  project_ = proj;
  
  QVBoxLayout* main_layout = new QVBoxLayout( this);
  main_layout->setMargin(9);
  main_layout->setSpacing(-1);

  prop_selector_ = new PropertySelector( this );
  output_name_ = new QLineEdit(this);

  main_layout->addWidget( prop_selector_ );
  main_layout->addWidget( new QLabel("New property name",this) );
  main_layout->addWidget( output_name_ );
  main_layout->addStretch();



  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  bottom_layout->setSpacing(9);
  QPushButton* ok = new QPushButton( "Create", this);
  QPushButton* close = new QPushButton( "Create and Close", this);
  QPushButton* cancel = new QPushButton( "Cancel", this);

  bottom_layout->addStretch();
  bottom_layout->addWidget( ok );
  bottom_layout->addWidget( close );
  bottom_layout->addWidget( cancel );


  main_layout->addStretch();
  main_layout->addLayout( bottom_layout );


  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( create_property() ) );
  QObject::connect( close, SIGNAL( clicked() ),
                    this, SLOT( create_property_and_close() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );

  setSizeGripEnabled( true );
}

QString 
Tie_breaking_random_dialog::selected_grid() const {
  return prop_selector_->selectedGrid();
}

QString 
Tie_breaking_random_dialog::selected_property() const{
  return prop_selector_->selectedProperty();
}

QString 
Tie_breaking_random_dialog::selected_region() const{
  return prop_selector_->selectedRegion();
}

bool Tie_breaking_random_dialog::create_property(){
	  QString grid_name = this->selected_grid();
	  QString prop_name = this->selected_property();
    QString region_name = this->selected_region();
    QString out_name = output_name_->text();

	  if( grid_name.isEmpty() || prop_name.isEmpty()) return false;

    QApplication::setOverrideCursor( Qt::WaitCursor );

	  QString sep = Actions::separator.c_str();
	  QStringList list;
	  list.append( grid_name );
    list.append( prop_name );
    list.append( out_name );
	  list.append(region_name);
	  
	  std::string parameters = list.join( sep ).toStdString();
	  if( parameters.empty() ) {
	    GsTLcerr << "Errors with the parameters selected";
	    GsTLcerr << gstlIO::end;
      QApplication::restoreOverrideCursor();
		  return false;
	  }

	  // call the DeleteObjectProperties action
	  Error_messages_handler error_messages;

	  bool ok = project_->execute( "BreakTiesRandom", parameters, &error_messages );

	  if( !ok ) {
	    GsTLcerr << "Command BreakTiesRandom could not be performed. \n";
	    if( !error_messages.empty() ) {
	      GsTLcerr << error_messages.errors();
	    }
	    GsTLcerr << gstlIO::end;
      QApplication::restoreOverrideCursor();
		  return false;
	  }

	  QApplication::restoreOverrideCursor();
	  project_->update();
	  return true;

}

void Tie_breaking_random_dialog::create_property_and_close(){
	bool ok = this->create_property();
	if(ok) this->accept();
}


// ----------------------
// **************************



Tie_breaking_spatial_dialog::
Tie_breaking_spatial_dialog( GsTL_project* proj,
                    QWidget* parent )
{

  project_ = proj;
  
  QVBoxLayout* main_layout = new QVBoxLayout( this);
  main_layout->setMargin(9);
  main_layout->setSpacing(-1);

  prop_selector_ = new PropertySelector( this );
  output_name_ = new QLineEdit(this);
  ellipsoid_widget_ = new EllipsoidInput( this );

  main_layout->addWidget( prop_selector_ );
  main_layout->addWidget( ellipsoid_widget_ );
  main_layout->addWidget( new QLabel("New property name",this) );
  main_layout->addWidget( output_name_ );
  main_layout->addStretch();

  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  bottom_layout->setSpacing(9);
  QPushButton* ok = new QPushButton( "Create", this);
  QPushButton* close = new QPushButton( "Create and Close", this);
  QPushButton* cancel = new QPushButton( "Cancel", this);

  bottom_layout->addStretch();
  bottom_layout->addWidget( ok );
  bottom_layout->addWidget( close );
  bottom_layout->addWidget( cancel );


  main_layout->addStretch();
  main_layout->addLayout( bottom_layout );


  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( create_property() ) );
  QObject::connect( close, SIGNAL( clicked() ),
                    this, SLOT( create_property_and_close() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );

  setSizeGripEnabled( true );
}

QString 
Tie_breaking_spatial_dialog::selected_grid() const {
  return prop_selector_->selectedGrid();
}

QString 
Tie_breaking_spatial_dialog::selected_property() const{
  return prop_selector_->selectedProperty();
}

QString 
Tie_breaking_spatial_dialog::selected_region() const{
  return prop_selector_->selectedRegion();
}

bool Tie_breaking_spatial_dialog::create_property(){
	  QString grid_name = this->selected_grid();
	  QString prop_name = this->selected_property();
    QString region_name = this->selected_region();
    QString out_name = output_name_->text();

    double r1,r2,r3, azimuth, dip,rake;

    ellipsoid_widget_->ranges(r1,r2,r3);
    ellipsoid_widget_->angles(azimuth,dip,rake);

	  if( grid_name.isEmpty() || prop_name.isEmpty()) return false;

    QApplication::setOverrideCursor( Qt::WaitCursor );



	  QString sep = Actions::separator.c_str();
	  QStringList list;
    list<<grid_name<<prop_name;
    list<<QString("%1").arg(r1)<<QString("%1").arg(r2)<<QString("%1").arg(r3);
    list<<QString("%1").arg(azimuth)<<QString("%1").arg(dip)<<QString("%1").arg(rake)<<"100000";  // max number of samples
    list<<out_name<<region_name;

  
	  std::string parameters = list.join( sep ).toStdString();
	  if( parameters.empty() ) {
	    GsTLcerr << "Errors with the parameters selected";
	    GsTLcerr << gstlIO::end;
      QApplication::restoreOverrideCursor();
		  return false;
	  }

	  // call the DeleteObjectProperties action
	  Error_messages_handler error_messages;

	  bool ok = project_->execute( "BreakTiesSpatial", parameters, &error_messages );

	  if( !ok ) {
	    GsTLcerr << "Command BreakTiesSpatial could not be performed. \n";
	    if( !error_messages.empty() ) {
	      GsTLcerr << error_messages.errors();
	    }
	    GsTLcerr << gstlIO::end;
      QApplication::restoreOverrideCursor();
		  return false;
	  }

	  QApplication::restoreOverrideCursor();
	  project_->update();
	  return true;

}

void Tie_breaking_spatial_dialog::create_property_and_close(){
	bool ok = this->create_property();
	if(ok) this->accept();
}
