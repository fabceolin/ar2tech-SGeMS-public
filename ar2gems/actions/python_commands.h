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

#ifndef __GSTLAPPLI_ACTIONS_PYTHON_COMMANDS_H__ 
#define __GSTLAPPLI_ACTIONS_PYTHON_COMMANDS_H__ 
 
#include <actions/common.h>
#include <actions/python_wrapper.h>
#include <appli/action.h>
#include <utils/gstl_messages.h>
#include <utils/string_manipulation.h>
#include <utils/error_messages_handler.h>
#include <utils/manager_repository.h>
#include <appli/project.h>
#include <grid/geostat_grid.h>
#include <grid/rgrid.h>
#include <grid/cartesian_grid.h>
#include <grid/point_set.h>
#include <grid/grid_property.h>
#include <grid/grid_categorical_property.h>

#include <math/non_parametric_distribution.h>

#if defined (RELEASE_PYTHON_IN_DEBUG) && defined (_DEBUG)
  #undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
  #include <Python.h>
#endif

#include <string> 
#include <algorithm>
#include <fstream>


static PyObject* sgems_execute(PyObject *self, PyObject *args)
{
  char* command_str;
  if( !PyArg_ParseTuple(args, "s:execute", &command_str) )
    return NULL;
  
  std::string command( command_str );
  String_Op::string_pair sgems_command =
    String_Op::split_string( command, " ", false );

  Error_messages_handler error_messages;
  bool ok = 
    Python_project_wrapper::project()->execute( sgems_command.first, sgems_command.second,
				       &error_messages );
  if( !ok ) {
    std::ostringstream message;
    message << "Error executing SGeMS command \"" << sgems_command.first
            << "\": " ;
    if( !error_messages.empty() )
      message << error_messages.errors() ; 
    
    *GsTLAppli_Python_cerr::instance() << message.str() << gstlIO::end;
  }
  PyObject* success = Py_BuildValue("b", ok);
  Py_INCREF(success);
  return success;
}

static Grid_continuous_property* get_coordinates( Geostat_grid* grid, int coord ) {
  Grid_continuous_property* prop = new Grid_continuous_property(grid->size(), "coord" );
  for( int i=0; i < grid->size() ; i++ ) {
    Geostat_grid::location_type loc = grid->location( i );
    prop->set_value( loc[coord], i );
  }
  return prop;
}

static PyObject* sgems_get_property( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;
 
  if( !PyArg_ParseTuple(args, "ss", &obj_str, &prop_str) )
    return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );
  
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  bool delete_prop = false;
  Grid_continuous_property* prop = 0;
  if( prop_name == "_X_" ) {
    prop = get_coordinates( grid, 0 );
    delete_prop = true;
  } else if (prop_name == "_Y_" ) {
    prop = get_coordinates( grid, 1 );
    delete_prop = true;
  } else if (prop_name == "_Z_" ) {
    prop = get_coordinates( grid, 2 );
    delete_prop = true;
  } else {
    prop = grid->property( prop_name );
  }


  if( !prop ) {
    *GsTLAppli_Python_cerr::instance() << "Grid \"" << object 
                << "\" does not have a property "
                << "called \"" << prop_name << "\"" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }



  PyObject *list = PyList_New(prop->size());
  
  for ( int i = 0; i < prop->size(); i++) {
	  float val = Grid_continuous_property::no_data_value;
	  if( prop->is_informed( i ) )
		  val = prop->get_value( i );
	  PyObject* item = Py_BuildValue("f", val);
	  if (!item) {
		  Py_DECREF(list);
		  list = NULL;
		  break;
	  }
	  PyList_SetItem(list, i, item);
  }

  if( delete_prop ) delete prop;
  return list;  
}

static PyObject* sgems_set_property( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;
  PyObject* tuple;


  if( !PyArg_ParseTuple(args, "ssO", &obj_str, &prop_str, &tuple) )
    return NULL;

  if( !PyList_Check( tuple ) ) return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );
  
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }


  Grid_continuous_property* prop = grid->property( prop_name );
  if( !prop ) {
    prop = grid->add_property( prop_name );
  }
  if( !prop ) return Py_None;


  int numPyItems  = PyList_Size( tuple );
  int size = std::min( prop->size(), numPyItems );

  for( int i=0 ; i < size ; i++ ) {
	  float val;

    PyArg_Parse( PyList_GET_ITEM( tuple, i ), "f", &val );
	  prop->set_value( val, i );

  }

  Python_project_wrapper::set_project_modified();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* sgems_get_cell_property( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;
  PyObject* tuple;
 
  if( !PyArg_ParseTuple(args, "ssO", &obj_str, &prop_str,&tuple) )
    return NULL;

  if( !PyList_Check( tuple ) ) return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );

  
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  bool delete_prop = false;
  Grid_continuous_property* prop = 0;
  if( prop_name == "_X_" ) {
    prop = get_coordinates( grid, 0 );
    delete_prop = true;
  } else if (prop_name == "_Y_" ) {
    prop = get_coordinates( grid, 1 );
    delete_prop = true;
  } else if (prop_name == "_Z_" ) {
    prop = get_coordinates( grid, 2 );
    delete_prop = true;
  } else {
    prop = grid->property( prop_name );
  }


  if( !prop ) {
    *GsTLAppli_Python_cerr::instance() << "Grid \"" << object 
                << "\" does not have a property "
                << "called \"" << prop_name << "\"" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  int numPyItems  = PyList_Size( tuple );
  int size = std::min( prop->size(), numPyItems );

  PyObject *outlist = PyList_New(size);
  

  for ( int i = 0; i < size; i++) {
	  int nodeid;
    PyArg_Parse( PyList_GET_ITEM( tuple, i ), "i", &nodeid );
    float val = prop->get_value_no_check(nodeid);
	  PyObject* item = Py_BuildValue("f", val);
	  if (!item) {
		  Py_DECREF(outlist);
		  outlist = NULL;
		  break;
	  }
	  PyList_SetItem(outlist, i, item);
  }

  return outlist;  

}

static PyObject* sgems_set_cell_property( PyObject *self, PyObject *args)
{

    char* obj_str;
  char* prop_str;
  PyObject* tuple_id;
  PyObject* tuple_val;
 
  if( !PyArg_ParseTuple(args, "ssOO", &obj_str, &prop_str,&tuple_id,&tuple_val) )
    return NULL;

  if( !PyList_Check( tuple_id ) ) return NULL;
  if( !PyList_Check( tuple_val ) ) return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  bool delete_prop = false;
  Grid_continuous_property* prop = 0;
  if( prop_name == "_X_" ) {
    prop = get_coordinates( grid, 0 );
    delete_prop = true;
  } else if (prop_name == "_Y_" ) {
    prop = get_coordinates( grid, 1 );
    delete_prop = true;
  } else if (prop_name == "_Z_" ) {
    prop = get_coordinates( grid, 2 );
    delete_prop = true;
  } else {
    prop = grid->property( prop_name );
  }

  if( !prop ) {
    *GsTLAppli_Python_cerr::instance() << "Grid \"" << object 
                << "\" does not have a property "
                << "called \"" << prop_name << "\"" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  int numPyItems  = PyList_Size( tuple_id );
  int size = std::min( prop->size(), numPyItems );

  for ( int i = 0; i < size; i++) {
	  int nodeid;
    float val;
    PyArg_Parse( PyList_GET_ITEM( tuple_id, i ), "i", &nodeid );
    PyArg_Parse( PyList_GET_ITEM( tuple_val, i ), "f", &val );
    prop->set_value(val,nodeid);
  }

  Py_INCREF(Py_None);
  return Py_None;

}

static PyObject* sgems_set_weight_property( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;
  PyObject* tuple;


  if( !PyArg_ParseTuple(args, "ssO", &obj_str, &prop_str, &tuple) )
    return NULL;

  if( !PyList_Check( tuple ) ) return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );
  
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }


  Grid_continuous_property* prop = grid->property( prop_name );
  if( !prop ) {
    prop = grid->add_weight_property( prop_name );
  }
  if( !prop ) return Py_None;


  int numPyItems  = PyList_Size( tuple );
  int size = std::min( prop->size(), numPyItems );

  for( int i=0 ; i < size ; i++ ) {
	  float val;

    PyArg_Parse( PyList_GET_ITEM( tuple, i ), "f", &val );
	  prop->set_value( val, i );

  }

  Python_project_wrapper::set_project_modified();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* sgems_get_weight_property( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;
 
  if( !PyArg_ParseTuple(args, "ss", &obj_str, &prop_str) )
    return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );
  
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  bool delete_prop = false;
  Grid_continuous_property* prop =  grid->weight_property( prop_name );

  if( !prop ) {
    *GsTLAppli_Python_cerr::instance() << "Grid \"" << object 
                << "\" does not have a weight property "
                << "called \"" << prop_name << "\"" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }



  PyObject *list = PyList_New(prop->size());
  
  for ( int i = 0; i < prop->size(); i++) {
	  float val = Grid_continuous_property::no_data_value;
	  if( prop->is_informed( i ) )
		  val = prop->get_value( i );
	  PyObject* item = Py_BuildValue("f", val);
	  if (!item) {
		  Py_DECREF(list);
		  list = NULL;
		  break;
	  }
	  PyList_SetItem(list, i, item);
  }

  if( delete_prop ) delete prop;
  return list;  
}

static PyObject* sgems_set_categorical_property_alpha( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;
  PyObject* tuple;
  char* category_str;

  if( !PyArg_ParseTuple(args, "sss0", &obj_str, &prop_str, &category_str, &tuple) ) {
    *GsTLAppli_Python_cerr::instance() << "Need 4 inputs paramters: gridName, PropName, catDefName, alphaProperty"<< gstlIO::end;
    return NULL;
  }


  if( !PyList_Check( tuple ) ) return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );

// Check if the grid exist
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }
// Check if the catgegorical definition exist
  std::string cat_def_name(category_str);
  ni = Root::instance()->interface( categoricalDefinition_manager+"/"+cat_def_name  );
  CategoricalPropertyDefinitionName* cat_def = 
    dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());


  Grid_categorical_property* prop = grid->categorical_property( prop_name );
  if( !prop ) {
    prop = grid->add_categorical_property( prop_name );
  }
  else {
	  prop = dynamic_cast<Grid_categorical_property*>(prop);
  }
  if( !prop ) return Py_None;


  int numPyItems  = PyList_Size( tuple );
  int size = std::min( prop->size(), numPyItems );

  if(!cat_def) {
    //No catDef exist with that input name: need to create one
    ni = Root::instance()->new_interface( "categoricaldefinition://"+cat_def_name,
                                           categoricalDefinition_manager +"/"+cat_def_name );
	//  ni = Root::instance()->new_interface( categoricalDefinition_manager, cat_def_name);
    cat_def = dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());
    std::set<std::string> cat_names;
	  for( int i=0 ; i < size ; i++ ) {
		  char* code;
	    PyArg_Parse( PyList_GET_ITEM( tuple, i ), "s", &code );
	    std::string code_str(code);
      cat_names.insert(code_str);
	  }

    std::set<std::string>::iterator it =  cat_names.begin();
    for(int i=0 ; it != cat_names.end(); ++it, ++i) {
      cat_def->add_category(i, *it);
    }
  }

  prop->set_category_definition(cat_def->name());

  for( int i=0 ; i < size ; i++ ) {
	  char* code;
    PyArg_Parse( PyList_GET_ITEM( tuple, i ), "s", &code );
    std::string code_str(code);
  	prop->set_value( code_str, i );
  }

  Python_project_wrapper::set_project_modified();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* sgems_set_categorical_property_integer( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;
  PyObject* tuple;
  char* category_str;
  std::string cat_def_name;
  bool cat_def_provided = false;

  if( PyArg_ParseTuple(args, "ssO", &obj_str, &prop_str, &tuple) ){
    cat_def_name = "Default";
  }
  else if( PyArg_ParseTuple(args, "sssO", &obj_str, &prop_str, &category_str, &tuple ) ) {
	  cat_def_provided = true;
    cat_def_name = std::string(category_str);
  }
//  else if( PyArg_ParseTuple(args, "ssO", &obj_str, &prop_str, &tuple) ){
//    cat_def_name = "Default";
//  }
  else {
    *GsTLAppli_Python_cerr::instance() << "Need at least 3 inputs parameters: gridName, PropName, property"<< gstlIO::end;
    return NULL;
  }


  if( !PyList_Check( tuple ) ) return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );

// Check if the grid exist
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }
// Check if the categorical deifniton exist
  ni = Root::instance()->interface( categoricalDefinition_manager+"/"+cat_def_name  );
  CategoricalPropertyDefinition* cat_def = 
    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  if( !cat_def ) {
    *GsTLAppli_Python_cerr::instance() << "No categorical definition called \"" << cat_def_name
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  Grid_categorical_property* prop = grid->categorical_property( prop_name );
  if( !prop ) {
    prop = grid->add_categorical_property( prop_name );
  }
  else {
	  prop = dynamic_cast<Grid_categorical_property*>(prop);
  }
  if( !prop ) return Py_None;
  prop->set_category_definition(cat_def_name);

  int numPyItems  = PyList_Size( tuple );
  int size = std::min( prop->size(), numPyItems );

  for( int i=0 ; i < size ; i++ ) {

	  float val;

    PyArg_Parse( PyList_GET_ITEM( tuple, i ), "f", &val );
	  prop->set_value( static_cast<int>(val), i );

  }

  Python_project_wrapper::set_project_modified();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* sgems_get_categorical_definition( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* prop_str;

  if( !PyArg_ParseTuple(args, "ss", &obj_str, &prop_str) )
    return NULL;

  std::string object( obj_str );
  std::string prop_name( prop_str );

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  Grid_categorical_property* prop = grid->categorical_property( prop_name );

  if( !prop ) {
    *GsTLAppli_Python_cerr::instance() << "Grid \"" << object
                << "\" does not have a categorical property "
                << "called \"" << prop_name << "\"" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  //CategoricalPropertyDefinition* cat_def = prop->get_category_definition();
  const CategoricalPropertyDefinitionName* cat_def = static_cast<const CategoricalPropertyDefinitionName*>(prop->get_category_definition());
  if(cat_def == 0) {
	Py_INCREF(Py_None);
	return Py_None;
  }


  PyObject *list = PyList_New(cat_def->number_of_category());

  for ( int i = 0; i < cat_def->number_of_category(); i++) {
	  std::string name = cat_def->get_category_name(i);
	  PyObject* item = Py_BuildValue("s", name.c_str());
	  if (!item) {
		  Py_DECREF(list);
		  list = NULL;
		  break;
	  }
	  PyList_SetItem(list, i, item);
  }

  return list;
}

static PyObject* sgems_get_region( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* region_str;

  if( !PyArg_ParseTuple(args, "ss", &obj_str, &region_str) )
    return NULL;

  std::string object( obj_str );
  std::string region_name( region_str );

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  Grid_region* region = grid->region( region_name );

  if( !region ) {
    *GsTLAppli_Python_cerr::instance() << "Grid \"" << object
                << "\" does not have a region "
                << "called \"" << region_name << "\"" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  PyObject *list = PyList_New(region->size());

  for ( int i = 0; i < region->size(); i++) {
	  PyObject* item = Py_BuildValue("b", region->is_inside_region( i ));
	  if (!item) {
		  Py_DECREF(list);
		  list = NULL;
		  break;
	  }
	  PyList_SetItem(list, i, item);
  }

  return list;
}

static PyObject* sgems_set_region( PyObject *self, PyObject *args)
{
  char* obj_str;
  char* region_str;
  PyObject* tuple;

  if( !PyArg_ParseTuple(args, "ssO", &obj_str, &region_str, &tuple) )
    return NULL;

  if( !PyList_Check( tuple ) ) return NULL;

  std::string object( obj_str );
  std::string region_name( region_str );
  
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    *GsTLAppli_Python_cerr::instance() << "No grid called \"" << object
                << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }


// First must check if the region exist
  Grid_region* region = grid->region( region_name );
  if( !region ) {
    region = grid->add_region( region_name );
  }


  int numPyItems  = PyList_Size( tuple );
  if( region->size() != numPyItems ) {
    *GsTLAppli_Python_cerr::instance() << "The region had "<<
      numPyItems<<" items; "<< region->size()<< "are necessary"
      << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  for( int i=0 ; i < region->size() ; i++ ) {
	  bool val;

		PyArg_Parse( PyList_GET_ITEM( tuple, i ), "b", &val );
		region->set_region_value( val, i );

  }

  Python_project_wrapper::set_project_modified();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* sgems_set_active_region( PyObject *self, PyObject *args)
{
	Geostat_grid * grid;
	char * obj_str;
  char * region_str;

	if( !PyArg_ParseTuple(args, "ss", &obj_str, &region_str) )
		return NULL;

	std::string object(obj_str);
  std::string region(region_str);

	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + object );
	grid = dynamic_cast<Geostat_grid*>(grid_ni.raw_ptr());
	if( !grid ) {
		*GsTLAppli_Python_cerr::instance() << object << " does not exist" << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_None;
	}
  if( region == "None" || region == "NONE" || region == "none") grid->unselect_region(); 
  else {
    bool ok = grid->select_region(region);
    if(!ok) 
      *GsTLAppli_Python_cerr::instance() << region << " does not exist" << gstlIO::end;
  }
  Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* get_nan_value( PyObject *self, PyObject *args)
{
  PyObject* nan = Py_BuildValue("f", Grid_continuous_property::no_data_value);
  Py_INCREF(nan);
	return nan;
}

static PyObject* sgems_get_dims( PyObject *self, PyObject *args)
{
	RGrid * grid;
	char * obj_str;

	if( !PyArg_ParseTuple(args, "s", &obj_str) )
		return NULL;

	std::string object(obj_str);

	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + object );
	grid = dynamic_cast<RGrid*>( grid_ni.raw_ptr() );
	if( !grid ) {
		*GsTLAppli_Python_cerr::instance() << object << " is not a valid regular grid" << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("[]");
	}

    PyObject *list = PyList_New(3);
	PyList_SetItem(list, 0, PyInt_FromLong(grid->nx()));
	PyList_SetItem(list, 1, PyInt_FromLong(grid->ny()));
	PyList_SetItem(list, 2, PyInt_FromLong(grid->nz()));

	return list;
}

static PyObject* sgems_get_grid_size( PyObject *self, PyObject *args)
{
	Geostat_grid * grid;
	char * obj_str;

	if( !PyArg_ParseTuple(args, "s", &obj_str) )
		return NULL;

	std::string object(obj_str);

	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + object );
	grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
	if( !grid ) {
		*GsTLAppli_Python_cerr::instance() << "The grid " <<object << " does not exist." << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("i", -1);
	}

	return Py_BuildValue("i", grid->size());

}

static PyObject* sgems_get_property_list( PyObject *self, PyObject *args)
{
	Geostat_grid *grid;
	char * obj_str;

	if( !PyArg_ParseTuple(args, "s", &obj_str) )
		return NULL;

	std::string object(obj_str);

	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + object );
	grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
	if( !grid ) {
		*GsTLAppli_Python_cerr::instance() << object << " does not exist" << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("[]");
	}

  std::list<std::string> properties = grid->property_list();
  PyObject *list = PyList_New(properties.size());
  int i=0;
  for( std::list<std::string>::const_iterator it = properties.begin();
       it != properties.end() ; ++it, ++i ) {
    PyList_SetItem(list, i, PyString_FromString( it->c_str() ) );
  }

	return list;
}

static PyObject* sgems_get_property_in_group( PyObject *self, PyObject *args)
{
	Geostat_grid *grid;
	char * obj_str;
  char * group_str;

	if( !PyArg_ParseTuple(args, "ss", &obj_str, &group_str) )
		return NULL;

	std::string object(obj_str);
  std::string group_name(group_str);

	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + object );
	grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
	if( !grid ) {
		*GsTLAppli_Python_cerr::instance() << object << " does not exist" << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("[]");
	}

  Grid_property_group* group = grid->get_group(group_name);
	if( !group ) {
		*GsTLAppli_Python_cerr::instance() << group_name << " does not exist" << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("[]");
	}

  std::vector<std::string>  names = group->property_names();
  PyObject *list = PyList_New(names.size());
  int i=0;
  for( std::vector<std::string>::const_iterator it = names.begin();
       it != names.end() ; ++it, ++i ) {
    PyList_SetItem(list, i, PyString_FromString( it->c_str() ) );
  }

	return list;
}

static PyObject* sgems_get_location( PyObject *self, PyObject *args)
{
	Geostat_grid *grid;
	char * obj_str;
  int  nodeid;

	if( !PyArg_ParseTuple(args, "si", &obj_str, &nodeid) )
		return NULL;

	std::string object(obj_str);

	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + object );
	grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
	if( !grid ) {
		*GsTLAppli_Python_cerr::instance() << object << " does not exist" << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("[]");
	}
  if(nodeid <0 || nodeid >= grid->size())	{
    *GsTLAppli_Python_cerr::instance() << "The nodeid must be between 0 and "<<grid->size() << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("[]");
	}
  Geostat_grid::location_type loc = grid->location( nodeid );

  PyObject *list = PyList_New(3);
  PyList_SetItem(list, 0, PyFloat_FromDouble(loc.x()) );
  PyList_SetItem(list, 1, PyFloat_FromDouble(loc.y()) );
  PyList_SetItem(list, 2, PyFloat_FromDouble(loc.z()) );

	return list;
}

static PyObject* sgems_get_nodeid( PyObject *self, PyObject *args)
{
	Geostat_grid *grid;
	char * obj_str;
  double  x;
  double  y;
  double  z;

	if( !PyArg_ParseTuple(args, "sddd", &obj_str, &x,&y,&z) )
		return NULL;

	std::string object(obj_str);

	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + object );
	grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
	if( !grid ) {
		*GsTLAppli_Python_cerr::instance() << object << " does not exist" << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("[]");
	}
  Cartesian_grid* cgrid = dynamic_cast<Cartesian_grid*>(grid);
  int nodeid  = -1;
  if(cgrid) {
    // TODO this code will return the closest node.  Need exact match
    GsTLPoint origin = cgrid->origin();
    int i = (x -origin.x() )/cgrid->geometry()->cell_dims()[0];
    int j = (y -origin.y() )/cgrid->geometry()->cell_dims()[1];
    int k = (z -origin.z() )/cgrid->geometry()->cell_dims()[2];
//    nodeid = k*cgrid->geometry()->dim(0)*cgrid->geometry()->dim(1) + j*cgrid->geometry()->dim(0) + i;
    cgrid->cursor()->coords(nodeid,i,j,k);
    if(nodeid >= 0) {  //Check if the location matches
      Geostat_grid::location_type loc = cgrid->location(nodeid);
      if( !GsTL::equals_with_epsilon(loc.x(),x) || !GsTL::equals_with_epsilon(loc.y(),y) || !GsTL::equals_with_epsilon(loc.z(),z)) {
        nodeid=-1;
      }
    }
  
  }
  else { // we have a point set
    for( int i = 0; i<grid->size(); i++) {
      Geostat_grid::location_type loc = grid->location(i);
      if( loc.x() == x && loc.y() == y && loc.z() == z ) {
        nodeid = i;
        break;
      }
    }
  }

/*
  if(nodeid <0 )	{
    *GsTLAppli_Python_cerr::instance() << "Could not find location ("<<x <<","<<y<<","<<z<<")"<<gstlIO::end;
		Py_INCREF(Py_None);
	}
*/
	return Py_BuildValue("i",nodeid);
}

static PyObject* sgems_get_closest_nodeid( PyObject *self, PyObject *args)
{
	Geostat_grid *grid;
	char * obj_str;
  double  x;
  double  y;
  double  z;

	if( !PyArg_ParseTuple(args, "sddd", &obj_str, &x,&y,&z) )
		return NULL;

	std::string object(obj_str);

	SmartPtr<Named_interface> grid_ni =
		Root::instance()->interface( gridModels_manager + "/" + object );
	grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
	if( !grid ) {
		*GsTLAppli_Python_cerr::instance() << object << " does not exist" << gstlIO::end;
		Py_INCREF(Py_None);
		return Py_BuildValue("[]");
	}
  int nodeid = grid->closest_node(Geostat_grid::location_type(x,y,z));
/*
  if(nodeid <0 )	{
    *GsTLAppli_Python_cerr::instance() << "Could not find closest location to ("<<x <<","<<y<<","<<z<<")"<<gstlIO::end;
		Py_INCREF(Py_None);
	}
*/
	return Py_BuildValue("i",nodeid);
}

static PyObject* sgems_new_point_set( PyObject *self, PyObject *args)
{
  char* obj_str;
  PyObject* tuple_x;
  PyObject* tuple_y;
  PyObject* tuple_z;


  if( !PyArg_ParseTuple(args, "sOOO", &obj_str, &tuple_x, &tuple_y, &tuple_z) )
    return NULL;

  if( !PyList_Check( tuple_x ) || 
      !PyList_Check( tuple_y ) ||
      !PyList_Check( tuple_z )) {

    *GsTLAppli_Python_cerr::instance() << "x y or z list were not provided" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  //Check that the xyz list have the same size
  int n_item_x  = PyList_Size( tuple_x );
  int n_item_y  = PyList_Size( tuple_y );
  int n_item_z  = PyList_Size( tuple_z );
  if( n_item_x*n_item_y*n_item_z == 0 ) {

    *GsTLAppli_Python_cerr::instance() << "At least one of the size of x,y or z list has zero lenght" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;

  }
  if(n_item_x != n_item_y || n_item_x!=n_item_z || n_item_y!=n_item_z) {
    *GsTLAppli_Python_cerr::instance() << "The x,y or z list have different length" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  std::string object_name( obj_str );
  if(object_name.empty()) {
    *GsTLAppli_Python_cerr::instance() << "No name was provided for the point set" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }
 
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + object_name );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( grid ) {
    *GsTLAppli_Python_cerr::instance() << "A grid called \"" << object_name
                << "\" already exist" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }


  std::string name_size_str = object_name+"::"+String_Op::to_string( n_item_x );
  grid_ni = Root::instance()->new_interface( "point_set://" + name_size_str,
				  gridModels_manager + "/" + object_name );
  Point_set* pset = dynamic_cast<Point_set*>( grid_ni.raw_ptr() );
  appli_assert( pset != 0 );

  
  std::vector< Point_set::location_type > point_locations;
  point_locations.reserve(n_item_x);
  float x,y,z;
  for(int i=0; i<n_item_x; ++i) {
    PyArg_Parse( PyList_GET_ITEM( tuple_x, i ), "f", &x );
    PyArg_Parse( PyList_GET_ITEM( tuple_y, i ), "f", &y );
    PyArg_Parse( PyList_GET_ITEM( tuple_z, i ), "f", &z );

    Point_set::location_type loc(x,y,z);
    point_locations.push_back(loc);

  }

  pset->point_locations( point_locations );

  Python_project_wrapper::set_project_modified();

  Py_INCREF(Py_None);
  return Py_None;
}



//==============================================================
//========================== HERVE =============================
//==============================================================

static PyObject* sgems_get_quantile( PyObject *self, PyObject *args)
{

  char* obj_str;
  PyObject* tuple;

  if( !PyArg_ParseTuple(args, "sO", &obj_str,  &tuple) )
    return NULL;

  if( !PyList_Check( tuple ) ) return NULL;

  std::string distr_name( obj_str );
  
  SmartPtr<Named_interface> dist_ni = Root::instance()->interface( continuous_distributions_manager + "/" + distr_name );

  Continuous_distribution* dist = dynamic_cast<Continuous_distribution*>( dist_ni.raw_ptr() );
  if( !dist ) {
    *GsTLAppli_Python_cerr::instance() << "No distribution called \"" << distr_name << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }


  int n_requestedquantiles  = PyList_Size( tuple );
  PyObject *list_quantiles = PyList_New(n_requestedquantiles);

  for( int i=0 ; i < n_requestedquantiles ; i++ ) {
	  double double_val;
    PyArg_Parse( PyList_GET_ITEM( tuple, i ), "d", &double_val );
    float q_val = dist->quantile( double_val );
	  PyObject* item = Py_BuildValue("f", q_val);
	  if (!item) {
		  Py_DECREF(list_quantiles);
		  list_quantiles = NULL;
		  break;
	  }
	  PyList_SetItem(list_quantiles, i, item);
  }

  return list_quantiles;
}

static PyObject* sgems_get_cdf( PyObject *self, PyObject *args)
{

  char* obj_str;
  PyObject* tuple;

  if( !PyArg_ParseTuple(args, "sO", &obj_str,  &tuple) )
    return NULL;

  if( !PyList_Check( tuple ) ) return NULL;

  std::string distr_name( obj_str );
  
  SmartPtr<Named_interface> dist_ni = Root::instance()->interface( continuous_distributions_manager + "/" + distr_name );

  Continuous_distribution* dist = dynamic_cast<Continuous_distribution*>( dist_ni.raw_ptr() );
  if( !dist ) {
    *GsTLAppli_Python_cerr::instance() << "No distribution called \"" << distr_name << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }


  int n_requestedquantiles  = PyList_Size( tuple );
  PyObject *outlist = PyList_New(n_requestedquantiles);

  for( int i=0 ; i < n_requestedquantiles ; i++ ) {
	  float float_val;
    PyArg_Parse( PyList_GET_ITEM( tuple, i ), "f", &float_val );
    double d_val = dist->cdf( float_val );
	  PyObject* item = Py_BuildValue("d", d_val);
	  if (!item) {
		  Py_DECREF(outlist);
		  outlist = NULL;
		  break;
	  }
	  PyList_SetItem(outlist, i, item);
  }

  return outlist;
}

static PyObject* sgems_get_pdf( PyObject *self, PyObject *args)
{

  char* obj_str;
  PyObject* tuple;

  if( !PyArg_ParseTuple(args, "sO", &obj_str,  &tuple) )
    return NULL;

  if( !PyList_Check( tuple ) ) return NULL;

  std::string distr_name( obj_str );
  
  SmartPtr<Named_interface> dist_ni = Root::instance()->interface( continuous_distributions_manager + "/" + distr_name );

  Continuous_distribution* dist = dynamic_cast<Continuous_distribution*>( dist_ni.raw_ptr() );
  if( !dist ) {
    *GsTLAppli_Python_cerr::instance() << "No distribution called \"" << distr_name << "\" was found" << gstlIO::end;
    Py_INCREF(Py_None);
    return Py_None;
  }

  int n_requestedquantiles  = PyList_Size( tuple );
  PyObject *outlist = PyList_New(n_requestedquantiles);

  for( int i=0 ; i < n_requestedquantiles ; i++ ) {
	  double d_val;
    PyArg_Parse( PyList_GET_ITEM( tuple, i ), "d", &d_val );
    double p_val = dist->pdf( d_val );
	  PyObject* item = Py_BuildValue("d", p_val);
	  if (!item) {
		  Py_DECREF(outlist);
		  outlist = NULL;
		  break;
	  }
	  PyList_SetItem(outlist, i, item);
  }

  return outlist;
}

static PyMethodDef SGemsMethods[] = {
    {"execute", sgems_execute, METH_VARARGS,
     "Return the number of arguments received by the process."}, // ?????
    {"get_property", sgems_get_property, METH_VARARGS,
     "Return a vector with property values if informed.\nUSAGE: [value_vector] = sgems.get_property(\"grid_name\",\"property_name\").\n Use _X_ or _Y_ or _Z_ for x,y,z."},
    {"set_property", sgems_set_property, METH_VARARGS,
     "Change or create a property of a grid.\nUSAGE: sgems.set_property(\"grid_name\",\"property_name\",vector of values populated in nodeid order)."},
    {"get_cell_property", sgems_get_cell_property, METH_VARARGS,
     "Return a vector with property values at specific nodeids, if informed.\nUSAGE: [value_vector] = sgems.get_cell_property(\"grid_name\",\"property_name\",[node1,node2,node3...]).\n Use _X_ or _Y_ or _Z_ for x,y,z."},
    {"set_cell_property", sgems_set_cell_property, METH_VARARGS,
     "Change or create a property of a grid at specific nodeid.\nUSAGE: sgems.set_cell_property(\"grid_name\",\"property_name\",[node1,node2,node3...],[val1,val2,val3...])."},
    {"get_dims", sgems_get_dims, METH_VARARGS, "Get dimension of a regular grid.\nUSAGE: [nx,ny,nz]=sgems.sgems_get_dims(\"grid_name\")"},
		{"get_grid_size", sgems_get_grid_size, METH_VARARGS, "Get the number of cells (nodes) of a grid.\nUSAGE: [n_cells]=sgems.sgems_get_grid_size(\"grid_name\")"},
    {"set_region", sgems_set_region, METH_VARARGS,
     "Import a region to a grid.\nUSAGE: sgems.sgems_set_region(\"grid_name\",\"region_name\",[v1,v2,v3...]"},
     {"get_region", sgems_get_region, METH_VARARGS,
      "Export a region from a grid.\nUSAGE: [v1,v2,v3...] = sgems.sgems_set_region(\"grid_name\",\"region_name\""},
    {"set_active_region", sgems_set_active_region, METH_VARARGS,
    "Select an active region on a grid (NONE unselect region).\nUSAGE: sgems.sgems_set_active_region(\"grid_name\",\"region_name\")"},
    {"nan", get_nan_value, METH_VARARGS,
    "Return the SGeMS value for NAN.\nUSAGE: [nanval] = get_nan_value()"},
    {"get_property_list", sgems_get_property_list, METH_VARARGS,
    "Return the list of property name in a grid.\nUSAGE: [prop_list]=sgems.sgems_get_property_list(\"grid_name\")"},
    {"get_location", sgems_get_location, METH_VARARGS,
    "Return the x,y,z location of a grid based on the nodeid.\nUSAGE: [x,y,z]=sgems.sgems_get_location(nodeid)."},
    {"get_nodeid", sgems_get_nodeid, METH_VARARGS,
    "Return the nodeid from a x,y,z location.\nUSAGE: [nodeid]=sgems.sgems_get_nodeid(x,y,z)."},
    {"get_closest_nodeid", sgems_get_closest_nodeid, METH_VARARGS,
    "Return the closest nodeid from a x,y,z location.\nUSAGE: [nodeid]=sgems.sgems_get_nodeid(x,y,z)."},
    {"set_categorical_property_int", sgems_set_categorical_property_integer, METH_VARARGS,
    "Set a categorical property from a list of integer"},
    {"set_categorical_property_alpha", sgems_set_categorical_property_alpha, METH_VARARGS,
    "Set a categorical property from a list of alphanumeric entries (string)"},
    {"get_categorical_definition", sgems_get_categorical_definition, METH_VARARGS,
    "Get the categorical definition from a categorical property"},
    {"get_properties_in_group", sgems_get_property_in_group, METH_VARARGS,
    "Get the name of the member property for a group.\nUSAGE: [list of strings]=sgems.sgems_get_property_in_group(\"grid_name\",\"group_name\")"},
    {"new_point_set", sgems_new_point_set, METH_VARARGS,
    "Create a new point set or append to existing point set, given a set of x,y,z coordinates.\nUSAGE: sgems.sgems_new_point_set(\"point_set_name\",x,y,z)"},
    {"quantile", sgems_get_quantile, METH_VARARGS,
    "Get the inverse cdf values (quantiles) from a list of probabilities.\nUSAGE: [quantile values list] = sgems.quantile(\"dist_name\",[.1, .2, .5,...])"},
    {"cdf", sgems_get_cdf, METH_VARARGS,
    "Get the cdf values (probability below) from a list of quantiles.\nUSAGE: [probability_below values list (doubles between 0 and 1)] = sgems.cdf(\"dist_name\",[value_1, value_2, value_3,...])"},
    {"pdf", sgems_get_pdf, METH_VARARGS,
    "Get the pdf values (probability) from a list of quantiles.\nUSAGE: [probability values list (doubles between 0 and 1)] = sgems.pdf(\"dist_name\",[value_1, value_2, value_3,...])"},
    {NULL, NULL, 0, NULL}
};

//TODO: add get categorical property in alpha mode

//==============================================================

static PyObject *sgems_cout(PyObject *self, PyObject *args) {
  char *s_line;
  if (!PyArg_ParseTuple(args, "s:sgemslog", &s_line))
    return NULL;
  
  Py_BEGIN_ALLOW_THREADS
    *GsTLAppli_Python_cout::instance() << s_line << gstlIO::end;
  Py_END_ALLOW_THREADS
    
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *sgems_cerr(PyObject *self, PyObject *args) {
  char *s_line;
  if (!PyArg_ParseTuple(args, "s:sgemslog", &s_line))
    return NULL;
  
  Py_BEGIN_ALLOW_THREADS
    *GsTLAppli_Python_cerr::instance() << s_line << gstlIO::end;
  Py_END_ALLOW_THREADS
    
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef RedirectMethods[] = {
 {"sgems_cout", (PyCFunction)sgems_cout, METH_VARARGS,
  "sgems_cout(line) writes a message to GsTLAppli_Python_cout"},
 {"sgems_cerr", (PyCFunction)sgems_cerr, METH_VARARGS,
  "sgems_cerr(line) writes a message to GsTLAppli_Python_cerr"},
 {NULL, NULL, 0, NULL}
};


#endif 
