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



#include <actions/add_property_to_grid.h>
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
#include <grid/grid_downscaler.h>
#include <grid/grid_path.h>

#include <geostat/utilities.h>

#if defined (RELEASE_PYTHON_IN_DEBUG) && defined (_DEBUG)
  #undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
  #include <Python.h>
#endif


#include <GsTL/math/math_functions.h>

#include <qdir.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <QByteArray>


#include <stdlib.h>
#include <stdio.h>

#include <memory.h>
#include <list>

// TL modified
#include <actions/python_wrapper.h>
#include <grid/reduced_grid.h>
#include <qapplication.h>



//================================================
/* AddPropertyToGridFromBinaryFile grid_name::propName::filename::CategoricalFlag
*/
bool Add_property_to_grid_from_binary_file::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params =
    String_Op::decompose_string( parameters, Actions::separator,
                                                   Actions::unique );

  if( params.size() < 3 ) return true;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() );
    return false;
  }

  std::string prop_name = params[1];
  if( grid->property(prop_name) != 0 ) {
    errors->report( "The grid already contains a property named "+prop_name);
    return false;
  }

  std::string filename = params[2];
  std::ifstream infile(filename);
  if(!infile.is_open()) {
      errors->report( "Could not open the file "+filename);
      return false;
  }
  infile.close();

  bool is_categorical;
  if( params.size() == 3) {
      is_categorical = false;
  }
  else {
      is_categorical = params[3] == "1";
  }

  if(is_categorical) {
    grid->add_categorical_property_from_disk(prop_name,filename);
  }
  else {
    grid->add_property_from_disk(prop_name,filename);
  }

  return true;
}


bool Add_property_to_grid_from_binary_file::exec() {
  return true;
}


Named_interface* Add_property_to_grid_from_binary_file::create_new_interface( std::string& ) {
  return new Add_property_to_grid_from_binary_file;
}



//================================================
/* AddPropertyToGridFromTextFile grid_name::propName::filename::CategoricalFlag
*/
bool Add_property_to_grid_from_text_file::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params =
    String_Op::decompose_string( parameters, Actions::separator,
                                                   Actions::unique );

  if( params.size() < 3 ) return true;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() );
    return false;
  }

  std::string prop_name = params[1];
  if( grid->property(prop_name) != 0 ) {
    errors->report( "The grid already contains a property named "+prop_name);
    return false;
  }

  std::string filename = params[1];
  bool is_categorical;
  if( params.size() == 3) {
      is_categorical = false;
  }
  else {
      is_categorical = params[1] == "1";
  }

  std::ifstream infile(filename);
  if(!infile.is_open()) {
      errors->report( "Error openining file "+filename);
      return false;
  }

  std::vector<float> values;
  values.reserve(grid->size());
  while (true) {
      float x;
      infile >> x;
      if( infile.eof() ) break;
      values.push_back(x);
  }

  if(values.size() != grid->size()) {
      std::stringstream err_msg;
      err_msg<<"Expected "<<grid->size()<<" but found "<<values.size()<<" entries in the file";
      errors->report( err_msg.str() );
      return false;
  }

  if(is_categorical) {
    Grid_continuous_property* cprop = grid->add_categorical_property(prop_name);
    for(int i=0; i< grid->size(); ++i) {
        cprop->set_value(static_cast<int>(values[i]), i);
    }
  }
  else {
      Grid_continuous_property* prop = grid->add_property(prop_name);
      for(int i=0; i< grid->size(); ++i) {
          prop->set_value(values[i], i);
      }
  }

  return true;
}


bool Add_property_to_grid_from_text_file::exec() {
  return true;
}


Named_interface* Add_property_to_grid_from_text_file::create_new_interface( std::string& ) {
  return new Add_property_to_grid_from_text_file;
}
