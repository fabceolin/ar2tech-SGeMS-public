
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

#include "inverse_distance_estimator.h"

#include <geostat/parameters_handler.h>
#include <utils/gstl_messages.h>
#include <utils/string_manipulation.h>
#include <utils/error_messages_handler.h>
#include <grid/gval_iterator.h>
#include <utils/manager_repository.h>
#include <grid/grid_region.h>
#include <grid/point_set.h>
#include <grid/grid_path.h>
#include <geostat/utilities.h>
#include <appli/utilities.h>


Named_interface* Inverse_distance_estimator::create_new_interface( std::string& ){
  return new Inverse_distance_estimator;
}


Inverse_distance_estimator::Inverse_distance_estimator(void)
{
}


Inverse_distance_estimator::~Inverse_distance_estimator(void)
{
}


bool Inverse_distance_estimator::initialize( const Parameters_handler* parameters,
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
  target_grid_region_ = grid_->region(parameters->value( "Grid_Name.region" ));


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
  hd_grid_region_ = hd_grid_->region(harddata_region_name);


  int max_neigh = 
    String_Op::to_number<int>( parameters->value( "Max_Conditioning_Data.value" ) );

  min_neigh_ = 
    String_Op::to_number<int>( parameters->value( "Min_Conditioning_Data.value" ) );
  errors->report( min_neigh_ >= max_neigh, 
		  "Min_Conditioning_Data", "Min must be less than Max" );

  power_ = String_Op::to_number<int>( parameters->value( "Power.value" ) );

/*
  // set-up the covariance
  bool init_cov_ok =
    geostat_utils::initialize_covariance( &covar_, "Variogram", 
	                        		            parameters, errors );
  if( !init_cov_ok ) return false;
*/

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
  neighborhood_ = SmartPtr<Neighborhood>(
    hd_grid_->neighborhood( ellips_ranges, ellips_angles, 0, true, hd_grid_region_ ) );
  } 
  else {
    neighborhood_ =  SmartPtr<Neighborhood>(
      hd_grid_->neighborhood( ellips_ranges, ellips_angles, 0, false, hd_grid_region_ ));
  }
  neighborhood_->select_property( harddata_property_name );
  neighborhood_->max_size( max_neigh );


  geostat_utils::set_advanced_search(neighborhood_.raw_ptr(), 
                      "AdvancedSearch", parameters, errors);

}


int Inverse_distance_estimator::execute( GsTL_project* proj ){

  // Set up a progress notifier	
  int total_steps = grid_->size();
  int frequency = std::max( total_steps / 20, 1 );
  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running Inverse Distance", 
			    total_steps, frequency );

  // create the property
  appli_message("creating new property: " << property_name_ << "..." );
  Grid_continuous_property* prop = 
    geostat_utils::add_property_to_grid( grid_, property_name_ );
  prop->set_parameters(parameters_);
  grid_->select_property( prop->name() );
  
  Grid_path path(grid_, prop, target_grid_region_);
  Grid_path::iterator begin =path.begin();
  Grid_path::iterator end = path.end();
  
  for( ; begin != end; ++begin ) {
    if( !progress_notifier->notify() ) {
      grid_->remove_property(property_name_);
      return 1;
    }

    if( begin->is_informed() ) continue;
      
    neighborhood_->find_neighbors( *begin );
    
    if( neighborhood_->size() < min_neigh_ )  continue;
    if(!neighborhood_->is_valid()) continue;

    Neighborhood::const_iterator it = neighborhood_->begin();
    float sum_inv_dist = 0;
    float sum_inv_dist_value = 0;
    for( ; it != neighborhood_->end(); ++it) {
      if( !it->is_informed()) continue;
      GsTLCoordVector delta_loc = it->location() - begin->location();
      float dist = std::sqrt(delta_loc.x()*delta_loc.x() + delta_loc.y()*delta_loc.y() + delta_loc.z()*delta_loc.z());
      float inv_dist = 1.0/std::pow(dist,power_);
      sum_inv_dist += inv_dist;
      sum_inv_dist_value += inv_dist*it->property_value();
    }
    begin->set_property_value(sum_inv_dist_value/sum_inv_dist);
  }

  return 0;
}



