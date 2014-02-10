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



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "actions" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/


#include <filters/io_actions.h>
#include <appli/action.h>
#include <utils/gstl_messages.h>
#include <utils/string_manipulation.h>
#include <utils/error_messages_handler.h>
#include <utils/manager_repository.h>
#include <appli/project.h>
#include <filters/filter.h>
#include <filters/save_project_objects.h>
#include <grid/geostat_grid.h>
#include <grid/cartesian_grid.h>
#include <grid/rgrid.h>
#include <grid/grid_categorical_property.h>
#include <filters/distribution_filter.h>

#include <GsTL/math/math_functions.h>

// these 3 Qt files are needed by Load_project
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QByteArray>


#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <list>

#include <grid/reduced_grid.h>


Named_interface* Load_object_from_file::create_new_interface( std::string& ) {
  return new Load_object_from_file();
}

Named_interface* Save_geostat_grid::create_new_interface( std::string& ) {
  return new Save_geostat_grid();
}

Named_interface* Load_project::create_new_interface( std::string& ) {
  return new Load_project();
}


//=========================================
Load_object_from_file::~Load_object_from_file() {
}

bool Load_object_from_file::init( std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() != 2 )
    return false;


  file_name_ = params[0];

  if( params[1] == "All" ) {
    if( !find_filter( file_name_ ) ) {
      errors->report( "Cannot find appropriate filter to read " + file_name_ );
      return false;
    }
    return true;
  }

  SmartPtr<Named_interface> ni =
    Root::instance()->new_interface( params[1], topLevelInputFilters_manager + "/" );

  if( ni.raw_ptr() == 0 )
    return false;

  Input_filter* filter = dynamic_cast<Input_filter*>( ni.raw_ptr() );
  appli_assert( filter );

  filter_ = SmartPtr<Input_filter>(filter);

  return true;
}


bool Load_object_from_file::exec() {
  std::string filter_errors;

  Named_interface* ni = filter_->read( file_name_, &filter_errors );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>(ni);

  // Notify the project that a new object was added. To do so, we need to 
  // get the name of the new grid from the grid manager.
  if( grid )  {
	  SmartPtr<Named_interface> dir = Root::instance()->interface( gridModels_manager );
	  Manager* mng = dynamic_cast<Manager*>( dir.raw_ptr() );
	  appli_assert( mng );

	  std::string grid_name = mng->name( dynamic_cast<Named_interface*>(grid) ); 

	  proj_->new_object(grid_name);
  }
  
  if( filter_errors.empty() ) return true;

  errors_->report( filter_errors );
  return false;
}



bool Load_object_from_file::find_filter( const std::string& filename ) {
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( topLevelInputFilters_manager );
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( dir );

  Manager::type_iterator begin = dir->begin();
  Manager::type_iterator end = dir->end();

  for( ; begin != end ; ++begin ) {
    SmartPtr<Named_interface> filter_ni = 
      Root::instance()->new_interface( *begin, topLevelInputFilters_manager + "/" );
    Input_filter* filter = dynamic_cast<Input_filter*>( filter_ni.raw_ptr() );  
    appli_assert( filter );
    if( filter->can_handle( filename ) ) {
      filter_ = SmartPtr<Input_filter>( filter );
      return true;
    }
  }

  return false;
}




//=========================================

bool Save_geostat_grid::init( std::string& parameters, 
			      GsTL_project* proj, Error_messages_handler* errors ) {
 proj_ = proj;
  errors_ = errors;

  //TL modified
  std::list<std::string> write_list;
  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

 
  // Get the grid from the manager

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  appli_assert( grid );
  grid_ = SmartPtr<Geostat_grid>( grid );

  // Get the filter from the manager
  SmartPtr<Named_interface> filter_ni =
	  Root::instance()->new_interface( params[2], 
	  outfilters_manager + "/" );

  if( filter_ni.raw_ptr() == 0 )
	  return false;

  Output_filter* filter = dynamic_cast<Output_filter*>( filter_ni.raw_ptr() );
  appli_assert( filter );

  //TL modified
  if (params.size() == 3)
    filter->setWriteMode(false);
  else {
    if (params[3] == "0")
		filter->setWriteMode(false);
	else 
		filter->setWriteMode(true);
	for (int i = 4; i < params.size(); ++i)
	  write_list.push_back(params[i]);
  }

  //if properties not specified, take entire grid
  if (write_list.empty())
	  write_list = grid->property_list();

  // Initialize the file name
  file_name_ = params[1];
  std::string filter_errors = "";

  filter->setWriteList(write_list);
  filter_ = SmartPtr<Output_filter>(filter);
  return filter_->write( file_name_, grid_.raw_ptr(), &filter_errors);
}




bool Save_geostat_grid::exec() {
	return true;
}



//===========================================
Load_project::~Load_project() {
}

bool Load_project::init( std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;

  dirname_ =  QString( parameters.c_str() );
  if( !dirname_.endsWith( ".prj" ) && !dirname_.endsWith( ".prj/") ) return false;

  QFileInfo info( dirname_ );
  if( !info.isDir() ) return false;

  return true;
}


bool Load_project::exec() {

  QDir* dir = new QDir( dirname_ );
  if( !dir->cd( dirname_ ) ) {
    QByteArray tmp = dirname_.toAscii();
    GsTLcerr << "Could not load project " << tmp.constData() << gstlIO::end;
    return false;
  }

  // if there is already a project, close it
  if( !proj_->is_empty() )
    proj_->clear();
  
  proj_->name( std::string( dirname_.toAscii() ) );

  QFileInfoList files_info = 
    dir->entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot );
  QFileInfoList::iterator it= files_info.begin();

  // ensure that the grid are loaded first
  for(int i=0; i< files_info.size(); ++i) {
    if( files_info.at(i).fileName().endsWith(".grid") ) {
      files_info.move(i,0);
    }
  }


  while( it != files_info.end() ) {
    QByteArray tmp = (*it).absoluteFilePath().toAscii();
    std::string param( std::string( tmp.constData() ) + 
                       Actions::separator + "All" );
    bool ok = proj_->execute( "LoadObjectFromFile", param, errors_ );
    if( !ok ) {
      QByteArray tt = (*it).fileName().toAscii();
      errors_->report( "... this error occurred while loading \"" + 
                             std::string( tt.constData()) +"\"" );
    }

    ++it;
  }

  proj_->reset_change_monitor();
  return true;
}



/*
 ------------------------
 */

Named_interface* Save_project::create_new_interface( std::string& ){
  return new Save_project;
}

bool Save_project::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  QString dirname = QString::fromStdString(parameters);
  if( !dirname.endsWith( ".prj" ) && !dirname.endsWith( ".prj/" ) ) 
    dirname.append( ".prj" );

//  preferences_.project_name = dirname;
  proj->name( std::string( dirname.toStdString() ) ); 
  //QDir* dir = new QDir( dirname );
  QDir dir( dirname );

  // If the directory already exists, erase all the files. 
  // Possible improvements: currently all the files are erased sequentially, and
  // if erasing fails, the functions is aborted. But that results in a partially
  // deleted project, and no new project saved. A better solution would be to 
  // check beforehand that we will be able to erase all the files (not difficult
  // with QFileInfo).
  if( dir.exists( dirname ) ) {
    bool ok = dir.cd( dirname );
    QStringList files = dir.entryList( QDir::Files );
    for( QStringList::iterator it = files.begin(); it != files.end(); ++it ) {
      QString tmp = *it;
      if (dir.isRelativePath(*it))
	        tmp = dir.filePath(*it);

      if( !dir.remove( tmp ) )
        errors->report("Project "+ dirname.toStdString() +" already exists and can not be "+
                        "overwritten");
    }
  }
  else {
    bool created = dir.mkdir( dirname );
    if( !created ) {
      errors->report("Could not create project "+dirname.toStdString());
    }
  }
  if(!errors->empty()) return false;
  

  if( !dir.cd( dirname ) ) {
    errors->report("Could not create project "+dirname.toStdString());
    return false;
  }

//  dir->cdUp();
//  preferences_.last_save_directory = dir->absolutePath();
//  dir->cd( dirname );

  //Get the folder to save
  SmartPtr<Named_interface> project_filter_ni = Root::instance()->interface(project_filters_manager+"/project_filters");
  Save_project_objects* project_filter = dynamic_cast<Save_project_objects*>(project_filter_ni.raw_ptr());
  if(project_filter==0) {
    errors->report("Could not get the filters manager for the project");
    return false;
  }

  std::map<std::string, Save_project_objects::object_to_parameters_mapT >::const_iterator it_dir = project_filter->directory_to_save().begin();

  // Loop over all the directory to save
  for( ; it_dir!= project_filter->directory_to_save().end(); ++it_dir) {
    SmartPtr<Named_interface> dir_object_ni = Root::instance()->interface(it_dir->first);  
    Manager* dir_object = dynamic_cast<Manager*>( dir_object_ni.raw_ptr() );
    if(dir_object==0) continue;

    //Get all the object in the directory
    Manager::interface_iterator it_object = dir_object->begin_interfaces();
    for( ; it_object != dir_object->end_interfaces(); ++it_object ) {

      Save_project_objects::Output_filter_param out_param = project_filter->filter_path(it_dir->first, (*it_object)->type() );

      SmartPtr<Named_interface> out_filter_ni = Root::instance()->new_interface( out_param.filter_param , out_param.filter_name);

      Output_filter* out_filter = dynamic_cast<Output_filter*>(out_filter_ni.raw_ptr());
      if(out_filter==0) continue;
      std::string error_filter;
      bool ok = out_filter->write(dirname.toStdString()+"/"+it_object->raw_ptr()->name(), it_object->raw_ptr(),  &error_filter );

      if(!ok) {
        errors->report("Error writing the project.  Operation aborted.  May result in loss of data.  Try saving in another directory\n"+error_filter);
        return false;
      }

    }
  }

  return true;


}


// --------------------------------------------



/*
  ---------------------
  parameters: filename
*/

Named_interface* Load_distribution_action::create_new_interface( std::string& ){
  return new Load_distribution_action;
}

Load_distribution_action::Load_distribution_action(void){

}
Load_distribution_action::~Load_distribution_action(void){

}

bool Load_distribution_action::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ) 
{
  SmartPtr<Named_interface> ni = Root::instance()->new_interface("Distribution", topLevelInputFilters_manager+"/" );
  
  Distribution_input_filter* input_filter = dynamic_cast<Distribution_input_filter*>(ni.raw_ptr());

  std::string error_str;
  input_filter->read(parameters,&error_str);

  if(!error_str.empty())
    errors->report(error_str);

  return error_str.empty();

}

bool Load_distribution_action::exec(){
  return true;
}



/*
  ---------------------
  parameters: filename[::dist1::dist2]
  default all the distributions are saved
  
*/

Named_interface* Save_distribution_action::create_new_interface( std::string& ){
  return new Save_distribution_action;
}

Save_distribution_action::Save_distribution_action(void){

}
Save_distribution_action::~Save_distribution_action(void){

}

bool Save_distribution_action::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ) 
{
  
  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if(params.size() <  1) {
    errors->report("Need at least one parameter (filename)");
    return false;
  }


  SmartPtr<Named_interface> ni = Root::instance()->new_interface("Distribution", outfilters_manager+"/" );
  Distribution_output_filter* out_filter = dynamic_cast<Distribution_output_filter*>(ni.raw_ptr());
  if(out_filter == 0) {
    errors->report("Could not find the filter to write continuous distribution to disk");
    return false;
  }
  

  std::string error_str;
  if(params.size() == 1) {
    return out_filter->write(params[0],&error_str);
  }
  else if(params.size() == 2) {
    Named_interface* ni_dist = Root::instance()->interface( continuous_distributions_manager+"/"+params[1] ).raw_ptr();
    if(ni_dist==0) return false;
    return out_filter->write(params[0],ni_dist,&error_str);
  }
  else {
    std::vector<const Named_interface*> dists_ni;
    for(int i=1; i<params.size(); ++i) {
      Named_interface* ni_dist = Root::instance()->interface( continuous_distributions_manager+"/"+params[i] ).raw_ptr();
      dists_ni.push_back(ni_dist);
    }  
    return out_filter->write(params[0],dists_ni, &error_str);
  }

  return false;

}

bool Save_distribution_action::exec(){
  return true;
}
