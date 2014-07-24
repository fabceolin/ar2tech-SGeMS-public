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


#ifndef __TIE_BREAKING_ACTIONS_H__ 
#define __TIE_BREAKING_ACTIONS_H__ 
 
#include <actions/common.h>
#include <appli/action.h> 
#include <grid/geostat_grid.h> 


#include <qstring.h>
 
#include <string> 
#include <set>
 
; 
class GsTL_project; 
class Error_messages_handler;

 

class ACTIONS_DECL Break_ties_random : public Action { 
 public: 
   static Named_interface* create_new_interface( std::string& );
 
 public: 
  Break_ties_random(): grid_(0),region_(0),initial_property_(0),tiebroken_property_(0) {}
  virtual ~Break_ties_random() {}
  virtual bool init( std::string& parameters, GsTL_project* proj,Error_messages_handler* errors ); 
  virtual bool exec(); 

protected: 
   Geostat_grid* grid_;
   Grid_region* region_;
   Grid_continuous_property* initial_property_;
   Grid_continuous_property* tiebroken_property_;
}; 

class ACTIONS_DECL Break_ties_spatial : public Action { 
 public: 
   static Named_interface* create_new_interface( std::string& );
 
 public: 
  Break_ties_spatial(): grid_(0),region_(0),initial_property_(0),tiebroken_property_(0) {}
  virtual ~Break_ties_spatial() {}
  virtual bool init( std::string& parameters, GsTL_project* proj,Error_messages_handler* errors ); 
  virtual bool exec(); 

protected: 
   Geostat_grid* grid_;
   Grid_region* region_;
   Grid_continuous_property* initial_property_;
   Grid_continuous_property* tiebroken_property_;
   float radius1_;
   float radius2_;
   float radius3_;
   float azimuth_;
   float dip_;
   float rake_;
   int max_neighbors_;
}; 


class ACTIONS_DECL Break_ties_with_secondary_property : public Action { 
 public: 
   static Named_interface* create_new_interface( std::string& );
 
 public: 
  Break_ties_with_secondary_property(): grid_(0),region_(0),initial_property_(0),secondary_property_(0),tiebroken_property_(0) {}
  virtual ~Break_ties_with_secondary_property() {}
  virtual bool init( std::string& parameters, GsTL_project* proj,Error_messages_handler* errors ); 
  virtual bool exec(); 

protected: 
   Geostat_grid* grid_;
   Grid_region* region_;
   Grid_continuous_property* initial_property_;
   Grid_continuous_property* secondary_property_;
   Grid_continuous_property* tiebroken_property_;
}; 



#endif 
