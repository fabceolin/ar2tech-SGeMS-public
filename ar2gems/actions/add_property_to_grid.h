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


#ifndef __ADD_PROPERRTY_TO_GRID_H__
#define __ADD_PROPERRTY_TO_GRID_H__
 
#include <actions/common.h>
#include <appli/action.h> 
#include <grid/geostat_grid.h> 
#include <grid/grid_property.h>
#include <grid/grid_categorical_property.h>

#include <qstring.h>
 
#include <string> 
#include <set>
 

class Input_filter; 
class GsTL_project; 
class Error_messages_handler;
class Cartesian_grid;
 
 


class ACTIONS_DECL Add_property_to_grid_from_binary_file : public Action {
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Add_property_to_grid_from_binary_file() {}
  virtual ~Add_property_to_grid_from_binary_file() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 


}; 


class ACTIONS_DECL Add_property_to_grid_from_text_file : public Action {
 public:
  static Named_interface* create_new_interface( std::string& );

 public:
  Add_property_to_grid_from_text_file() {}
  virtual ~Add_property_to_grid_from_text_file() {}

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors );
  virtual bool exec();


};

class ACTIONS_DECL Add_property_to_grid_with_nodeid_from_text_file : public Action {
 public:
  static Named_interface* create_new_interface( std::string& );

 public:
  Add_property_to_grid_with_nodeid_from_text_file() {}
  virtual ~Add_property_to_grid_with_nodeid_from_text_file() {}

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors );
  virtual bool exec();


};

#endif 
