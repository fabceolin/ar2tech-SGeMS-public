
/* -----------------------------------------------------------------------------
** Copyright© 2012 Advanced Resources and Risk Technology, LLC
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

#include "nearest_neighbor_estimator.h"

#include <geostat/parameters_handler.h>
#include <utils/gstl_messages.h>
#include <utils/string_manipulation.h>
#include <utils/error_messages_handler.h>
#include <grid/gval_iterator.h>
#include <appli/manager_repository.h>
#include <grid/grid_region.h>
#include <grid/point_set.h>
#include <geostat/utilities.h>
#include <appli/utilities.h>


Named_interface* Nearest_neighbor_estimator::create_new_interface( std::string& ){
  return new Nearest_neighbor_estimator;
}


Nearest_neighbor_estimator::Nearest_neighbor_estimator(void) : hd_grid_(0), grid_(0)
{
}


Nearest_neighbor_estimator::~Nearest_neighbor_estimator(void)
{

    if( hd_grid_!=0 && dynamic_cast<Point_set*>(hd_grid_) ) {
      hd_grid_->set_coordinate_mapper(0);
  }
}


bool Nearest_neighbor_estimator::initialize( const Parameters_handler* parameters,
				   Error_messages_handler* errors ){

  std::string grid_name = parameters->value( "Grid_Name.value" );
  errors->report( grid_name.empty(), 
		  "Grid_Name", "No grid selected" );
  property_name_ = parameters->value( "Property_Name.value" );
  errors->report( property_name_.empty(), 
		  "Property_Name", "No property name specified" );


  // Get the simulation grid from the grid manager

  if( !grid_name.empty() ) {
    bool ok = geostat_utils::create( grid_, grid_name,
				 "Grid_Name", errors );
    if( !ok ) return false;
  }
  else 
    return false;

  gridTempRegionSelector_.set_temporary_region(
                parameters->value( "Grid_Name.region" ), grid_);


  std::string hd_grid_name = parameters->value( "Hard_Data.grid" );
  errors->report( hd_grid_name.empty(), 
		  "Hard_Data", "No hard data specified" );
  std::string harddata_property_name = parameters->value( "Hard_Data.property" );
  errors->report( harddata_property_name.empty(), 
		  "Hard_Data", "No property name specified" );

  // Get the harddata grid from the grid manager
  if( !hd_grid_name.empty() ) {
    bool ok = geostat_utils::create( hd_grid_, hd_grid_name,
				 "Hard_Data", errors );
    if( !ok ) return false;
  }
  else 
    return false;

  std::string harddata_region_name = parameters->value( "Hard_Data.region" );
  Grid_region* hd_region = hd_grid_->region(harddata_region_name);

  // If the hard data is on the same grid than the
  // estimation grid, than it cannot be set to a region others than the one
  // already set on that grid
  // The is only used if the neighborhood is to consider only data within a region
  if(hd_grid_ !=  grid_)
      hdgridTempRegionSelector_.set_temporary_region(
              parameters->value( "Hard_Data.region" ),hd_grid_ );


  hd_prop_ = hd_grid_->select_property( harddata_property_name );
  if( !hd_prop_ ) {
    std::ostringstream error_stream;
    error_stream << hd_grid_name <<  " does not have a property called " 
	            	 << harddata_property_name;
    errors->report( "Hard_Data", error_stream.str() );
  }


  //-------------
  // Set up the search neighborhood

  GsTLTriplet ellips_ranges;
  GsTLTriplet ellips_angles;
  bool extract_ok = 
    geostat_utils::extract_ellipsoid_definition( ellips_ranges, ellips_angles,
	                                    				   "Search_Ellipsoid.value",
					                                       parameters, errors );
  if( !extract_ok ) return false;

  extract_ok = geostat_utils::is_valid_range_triplet( ellips_ranges );
  errors->report( !extract_ok,
                  "Search_Ellipsoid",
                  "Ranges must verify: major range >= " 
                  "medium range >= minor range >= 0" );
  if( !extract_ok ) return false;


  hd_grid_->select_property(harddata_property_name);
  if( dynamic_cast<Point_set*>(hd_grid_) ) {
    hd_grid_->set_coordinate_mapper(grid_->coordinate_mapper());
    neighborhood_ = SmartPtr<Neighborhood>(
      hd_grid_->neighborhood( ellips_ranges, ellips_angles, 0, true, hd_region ) );
  } 
  else {
    neighborhood_ =  SmartPtr<Neighborhood>(
      hd_grid_->neighborhood( ellips_ranges, ellips_angles, 0, false, hd_region ));
  }
  neighborhood_->select_property( harddata_property_name );
  neighborhood_->max_size( 1 );


}


int Nearest_neighbor_estimator::execute( GsTL_project* proj ){

  // Set up a progress notifier	
  int total_steps = grid_->size();
  int frequency = std::max( total_steps / 20, 1 );
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running NearestNeighbor", 
			    total_steps, frequency );

  // create the property
  appli_message("creating new property: " << property_name_ << "..." );
  Grid_continuous_property* prop = 
    geostat_utils::add_categorical_property_to_grid( grid_, property_name_ );
  prop->set_parameters(parameters_);
  grid_->select_property( prop->name() );
  

  typedef Geostat_grid::iterator iterator;
  iterator begin = grid_->begin();
  iterator end = grid_->end();
  
  for( ; begin != end; ++begin ) {
    if( !progress_notifier->notify() ) {
      grid_->remove_property(property_name_);
      return 1;
    }

    if( begin->is_informed() ) continue;

    // code hard coded for structured grid
    /*
    Geovalue gval(*begin);
    Geovalue::location_type xyz_loc = begin->xyz_location();
    gval.set_cached_location(xyz_loc);
    neighborhood_->find_neighbors( gval );  
    */

    neighborhood_->find_neighbors( *begin );
    
    if( neighborhood_->is_empty() )  continue;

    begin->set_property_value(neighborhood_->begin()->property_value());

  }

  return 0;
}



