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

#ifndef __IO_ACTIONS_H__ 
#define __IO_ACTIONS_H__ 
 
#include <filters/common.h>
#include <appli/action.h> 
#include <filters/filter.h> 
#include <grid/geostat_grid.h> 
#include <grid/property_copier.h>

#include <qstring.h>
 
#include <string> 
#include <set>
 

class GsTL_project; 
class Error_messages_handler;
 

 
class FILTERS_DECL Load_object_from_file : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Load_object_from_file() {} 
  virtual ~Load_object_from_file(); 
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
 
 protected:
   bool find_filter( const std::string& filename );

 protected: 
  std::string file_name_; 
  SmartPtr<Input_filter> filter_; 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 
 
 
class FILTERS_DECL Save_geostat_grid : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Save_geostat_grid() {}  // TL modified
  virtual ~Save_geostat_grid() {}; 
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
 
 protected: 
  std::string file_name_; 
  SmartPtr<Geostat_grid> grid_; 
  SmartPtr<Output_filter> filter_; 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
}; 
 





class FILTERS_DECL Load_project : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Load_project() {} 
  virtual ~Load_project(); 
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 
 
 protected: 
  QString dirname_; 
  GsTL_project* proj_; 
  Error_messages_handler* errors_;
};  

 
/*
loop over all the objects to be saved and call their respective outfilter
*/
class FILTERS_DECL Save_project : public Action {
 public:
  static Named_interface* create_new_interface( std::string& );

 public:
  Save_project(){}
  virtual ~Save_project() {}

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors );

  virtual bool exec(){return true;}

private :

};




class FILTERS_DECL Load_distribution_action :  public Action
{
public:

  static Named_interface* create_new_interface( std::string& );

  Load_distribution_action(void);
  virtual ~Load_distribution_action(void);

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();

private:

};

class FILTERS_DECL Save_distribution_action :  public Action
{
public:

  static Named_interface* create_new_interface( std::string& );

  Save_distribution_action(void);
  virtual ~Save_distribution_action(void);

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();

private:

};



#endif 
