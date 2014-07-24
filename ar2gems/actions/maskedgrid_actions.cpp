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



#include <actions/maskedgrid_actions.h>
#include <appli/action.h>
#include <utils/gstl_messages.h>
#include <utils/string_manipulation.h>
#include <utils/error_messages_handler.h>
#include <utils/manager_repository.h>
#include <appli/project.h>
#include <filters/filter.h>
#include <grid/geostat_grid.h>
#include <grid/cartesian_grid.h>
#include <grid/reduced_grid.h>

Named_interface* Create_mgrid_from_cgrid::create_new_interface( std::string& ) {
  return new Create_mgrid_from_cgrid();
}

  /** The parameters for this function are:
   * - the name of the new grid
   * - the name of the existing cgrid
   * - the name of the region
   */
bool
Create_mgrid_from_cgrid::init( std::string& parameters, GsTL_project* proj,
                                     Error_messages_handler* errors ){
  proj_ = proj ;
  errors_ = errors;

  std::vector< std::string > params =
      String_Op::decompose_string( parameters, Actions::separator,
                                   Actions::unique );

  if( params.size() <3 )
    return false;

  cgrid_name_ = params[0];
  mgrid_name_ = params[1];
  region_names_.insert(region_names_.begin(),params.begin()+2,params.end());

  return true;

}
bool Create_mgrid_from_cgrid::exec(){
  // Get the grid from the manager
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + cgrid_name_ );
  if( ni.raw_ptr() == 0 ) {
    errors_->report( "Object " + cgrid_name_ + " does not exist." );
    return false;
  }

  Cartesian_grid* cgrid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
  if( cgrid == 0) {
    errors_->report( "Object " + cgrid_name_ + " is not a cartesian grid." );
    return false;
  }
 // Get the region
  std::vector<Grid_region*> regions;
  std::vector<std::string>::iterator it =  region_names_.begin();
  for( ; it!= region_names_.end(); ++it) {
    Grid_region* region = cgrid->region( *it );
    if(!region) {
      errors_->report( "Region " + (*it) + " does not exist."  );
      return false;
    }
    regions.push_back(region);
  }

// Create the new masked_grid

  ni =
      Root::instance()->new_interface("reduced_grid://"+mgrid_name_,
      gridModels_manager + "/" + mgrid_name_);

  if( ni.raw_ptr() == 0 ) {
    errors_->report( "Object " + mgrid_name_ + " already exists. Use a different name." );
    return false;
  }

  Reduced_grid* grid = dynamic_cast<Reduced_grid*>( ni.raw_ptr() );
  GsTLPoint rotation_point = cgrid->geometry()->rotation_point();
  GsTLPoint origin = cgrid->origin();

  //Create the grid
  if( regions.size() == 1 ) {  //avoid a copy of the region array

    grid->set_dimensions(
      cgrid->geometry()->dim(0),cgrid->geometry()->dim(1),cgrid->geometry()->dim(2),
      cgrid->cell_dimensions()[0],cgrid->cell_dimensions()[1],cgrid->cell_dimensions()[2],
      origin.x(),origin.y(),origin.z(),
      cgrid->rotation_z(),
      rotation_point.x(),rotation_point.y(),rotation_point.z(),
      regions[0]->data());
  } else {
    int mask_size = regions[0]->size();
    std::vector<bool> mask( mask_size );
    for( int i=0; i< mask_size; ++i ) {
      bool ok = false;
      for( int j = 0; j<regions.size(); j++ ) {
        ok == ok || regions[j]->is_inside_region(i);
      }
      mask[i] = ok;
    }
    grid->set_dimensions(
      cgrid->geometry()->dim(0),cgrid->geometry()->dim(1),cgrid->geometry()->dim(2),
      cgrid->cell_dimensions()[0],cgrid->cell_dimensions()[1],cgrid->cell_dimensions()[2],
      origin.x(),origin.y(),origin.z(),
      cgrid->rotation_z(),
      rotation_point.x(),rotation_point.y(),rotation_point.z(),
      mask);
  }

  proj_->new_object( mgrid_name_ );
  return true;
}
