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


#include <actions/tie_breaking_actions.h>
#include <appli/action.h>
#include <math/random_numbers.h>
#include <utils/gstl_messages.h>
#include <utils/string_manipulation.h>
#include <utils/error_messages_handler.h>
#include <utils/manager_repository.h>
#include <appli/project.h>
#include <grid/geostat_grid.h>
#include <grid/grid_path.h>
#include <geostat/utilities.h>
#include <math/angle_convention.h>

#include <limits>

#if defined (RELEASE_PYTHON_IN_DEBUG) && defined (_DEBUG)
  #undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
  #include <Python.h>
#endif


#include <GsTL/math/math_functions.h>


//format GridName::InputProp[::OutputProp::RegionName]
// if OutputProp not specified the suffix "tie_broken" is added to the input name
bool Break_ties_random::init( std::string& parameters, GsTL_project* proj,Error_messages_handler* errors ) {
  std::vector< std::string > params = String_Op::decompose_string(parameters,Actions::separator,Actions::unique );

  if( params.size() < 2 ) return false;

  std::string grid_name = params[0];

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + grid_name );
  grid_ = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid_ ) {
    std::ostringstream message;
    message << "No grid called \"" << grid_name << "\" was found";
    errors->report( message.str() );
    return false;
  }

  initial_property_ = grid_->property( params[1] );
  if(initial_property_ == 0) {
    errors->report( "No property called "+params[1] );
    return false;
  }


  std::string out_name;
  if(params.size() >= 3) {
    out_name = params[2];
  };
  
  if(out_name.empty()){
    out_name = initial_property_->name() + "-tie-broken-random";
  }
  tiebroken_property_ = grid_->add_property( out_name );
  /*
  if( tiebroken_property_ == 0 ) {
    errors->report( "A property named "+out_name+" already exist" );
    return false;
  }
  */
  while(!tiebroken_property_) {
    out_name.append("_0");
    tiebroken_property_ = grid_->add_property( out_name );
  }

  region_ = 0;
  if(params.size()==4) {
    region_ = grid_->region(params[3]);
    if( region_ == 0) {
      errors->report( "No region called "+params[3] );
      return false;
    }
  } 

  return true;
}

bool Break_ties_random::exec(  ) {

  std::vector<std::pair<float, int> > z_nodeid;
  int ncells = initial_property_->size();
  z_nodeid.reserve(ncells);
  
  std::vector<float> z_value_diff;
  z_value_diff.reserve(ncells);

  // Create pair with all valid grid block property values and their cell index
  for(int i=0; i<initial_property_->size(); ++i) {
    if(region_  && !region_->is_inside_region(i)) continue;
    if(!initial_property_->is_informed(i)) continue;
    z_nodeid.push_back(  std::make_pair( initial_property_->get_value_no_check(i) ,i) );
  }
  
  std::sort(z_nodeid.begin(),z_nodeid.end());
  int ninformedcells=z_nodeid.size();

  // Find the most appropriate epsilon value
  bool allvaluesareequal=true;
  for (int ii=1;ii<ninformedcells;++ii){
    float diff = z_nodeid[ii].first-z_nodeid[ii-1].first;
    if (diff>0)
    {
      z_value_diff.push_back(diff);
      allvaluesareequal=false;
    }
  }
  float epsilon(100*std::numeric_limits<float>::epsilon()); // Machine precision epsilon;
  if (!allvaluesareequal){
    // in the frequent case where values are not equal
    std::vector<float>::iterator minpos;
    minpos = std::min_element(z_value_diff.begin(),z_value_diff.end());
    epsilon = std::max(100*std::numeric_limits<float>::epsilon(),(float)(*minpos)/(ncells+1)); // divided by ncells+1 : this ensures rank preservation
  }
  
  // Create the new pair with randomly perturbed z values
  std::vector<std::pair<float, int> > z_randomly_perturbed_pairs;
  z_randomly_perturbed_pairs.reserve(ninformedcells);

  Global_random_number_generator::instance()->seed( 12127317 );
  STL_generator gen;

  // Now scan each unique value and add random_integer[0,number_of_ties]*epsilon to its ties
  std::vector<std::pair<float,int> >::iterator up,it;
  it=z_nodeid.begin();
  while(it != z_nodeid.end()) {
    std::pair<float,int> last_pair(it->first,ncells);
    up  = std::lower_bound(it,z_nodeid.end(),last_pair); // up is the last position of the value we look for (here: last_pair)
    int number_of_ties = up-it;
    if (number_of_ties==1){
      // No multiplets
      z_randomly_perturbed_pairs.push_back( std::make_pair( it->first, it->second ) );
    }else{
      // We have multiplets
      float z = it->first;

      // Create a randomly shuffled array of integers that will be multiplied by epsilon and added to z
      std::vector<int> random_integers;
      
      random_integers.reserve(number_of_ties);
      for (int i=0; i<number_of_ties; ++i) random_integers.push_back(i); //  0 1 2 3 ... number_of_ties
      // using a std built-in random generator (CHECK IF THIS IS OK OR IF WE WANT TO USE A SEED)
      std::random_shuffle ( random_integers.begin(), random_integers.end(),gen );
      // Now build the tie-broken z_nodeid pairs
      for (int i=0  ; it != up ; ++it, ++i ){
        // reconstruct pair
        z_randomly_perturbed_pairs.push_back( std::make_pair( z+random_integers[i]*epsilon, it->second ) );
      }
      // move on to the next unique value:
    }
    it = up;
  }

  // Now add the new pair as a new property on the indicated grid region:
  for (it=z_randomly_perturbed_pairs.begin();it!=z_randomly_perturbed_pairs.end();++it){
    tiebroken_property_->set_value(it->first,it->second);
  }

  return true;

}

Named_interface* 
Break_ties_random::create_new_interface( std::string& ) {
  return new Break_ties_random; 
}


//format GridName::InputProp::radius1::radius2::radius3::azimuth::dip::rake[::max_neighborhood_nodes::OutputProp::RegionName]
// if OutputProp not specified the suffix "tie_broken" is added to the input name
bool Break_ties_spatial::init( std::string& parameters, GsTL_project* proj,Error_messages_handler* errors ) {
  std::vector< std::string > params = String_Op::decompose_string(parameters,Actions::separator,Actions::unique );

  if( params.size() < 8 ) return false;

  std::string grid_name = params[0];

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + grid_name );
  grid_ = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid_ ) {
    std::ostringstream message;
    message << "No grid called \"" << grid_name << "\" was found";
    errors->report( message.str() );
    return false;
  }

  initial_property_ = grid_->property( params[1] );
  if(initial_property_ == 0) {
    errors->report( "No property called "+params[1] );
    return false;
  }

  radius1_ = String_Op::to_number<float>(params[2]);
  radius2_ = String_Op::to_number<float>(params[3]);
  radius3_ = String_Op::to_number<float>(params[4]);
  if( radius1_ <=0 || radius2_ <=0  || radius3_ <=0 ) {
    errors->report( "Radius must be greater than zero");
    return false;
  }

  azimuth_ = degree_to_radian( String_Op::to_number<float>(params[5]) );
  dip_ = degree_to_radian( String_Op::to_number<float>(params[6]) );
  rake_ = degree_to_radian( String_Op::to_number<float>(params[7]) );
  convert_to_math_standard_angles_rad( azimuth_, dip_, rake_ );

  if(params.size()>=9) {
      max_neighbors_ = String_Op::to_number<int>(params[8]);
      if( max_neighbors_ <=0 ) {
          errors->report( "Maximum number of neighbors to consider must be an integer greater than zero");
          return false;
      }
  }else{
    max_neighbors_=10e5;
  }


  std::string out_name;
  if(params.size() >= 10) {
    out_name = params[9];
  }
  if(out_name.empty()) {
    out_name = initial_property_->name() + "-tie-broken-spatial-radius" + params[2] + "-maxneigh" + std::to_string( max_neighbors_ );
  }
  tiebroken_property_ = grid_->add_property( out_name );
  while(!tiebroken_property_) {
    out_name.append("_0");
    tiebroken_property_ = grid_->add_property( out_name );
  }


  region_ = 0;
  if(params.size()==11) {
    region_ = grid_->region(params[10]);
    if( region_ == 0) {
      errors->report( "No region called "+params[10] );
      return false;
    }
  } 

  return true;
}

bool Break_ties_spatial::exec(  ) {

  std::vector<std::pair<float, int> > z_nodeid;
  int ncells = initial_property_->size();
  z_nodeid.reserve(ncells);
  
  Neighborhood* neigh = grid_->neighborhood(radius1_,radius2_,radius3_,azimuth_,dip_,rake_,0,false,region_);
  neigh->select_property(initial_property_->name());
  neigh->max_size(max_neighbors_);

  // Create pair with all valid grid block property values and their cell index
  for(int i=0; i<ncells; ++i) {
    if(region_  && !region_->is_inside_region(i)) continue;
    if(!initial_property_->is_informed(i)) continue;
    z_nodeid.push_back( std::make_pair(initial_property_->get_value_no_check(i),i) );
  }
  
  std::sort(z_nodeid.begin(),z_nodeid.end());
  int ninformedcells=z_nodeid.size();

  // Machine precision epsilon
  float epsilon = 100*std::numeric_limits<float>::epsilon();

  Global_random_number_generator::instance()->seed( 12127317 );
  STL_generator gen;

  std::vector<std::pair<float,int> >::iterator up,it;
  it=z_nodeid.begin();
  int n_unique(0);
  int n_multiplets(0);
  
  while(it != z_nodeid.end()) {
    std::pair<float,int> last_pair(it->first,ncells);
    up  = std::lower_bound(it,z_nodeid.end(),last_pair); // up is the last position of the value we look for (here: last_pair)
    int number_of_ties = up-it;
    if (number_of_ties==1){
      tiebroken_property_->set_value(it->first,it->second);
      // No multiplets
    }else{

      // We have multiplets
      std::vector<std::pair<float, int> > local_average_nodeid;
      std::vector<float> local_average_val;
      std::vector<std::pair<float,int> >::iterator it_ties=it;
      for ( ; it_ties!=up ; ++it_ties){
        Neighbors neighbors;
        Geovalue gval(grid_,initial_property_,it_ties->second);
        neigh->find_neighbors(gval,neighbors);
        float sum = 0;
        for(Neighbors::iterator it_neigh=neighbors.begin(); it_neigh!=neighbors.end(); ++it_neigh ) {
          sum += it_neigh->property_value();
        }
        sum/=neighbors.size();
        local_average_val.push_back(sum);
        local_average_nodeid.push_back(std::make_pair(sum,it_ties->second) );
      }

      std::sort(local_average_nodeid.begin(),local_average_nodeid.end());
      std::sort(local_average_val.begin(),local_average_val.end());

      // Count the number of unique average values (and thus the fraction really sorted using space vs. randomly)
      std::vector<float>::iterator unique_it;
      unique_it = std::unique(local_average_val.begin(),local_average_val.end());
      n_unique += std::distance(local_average_val.begin(),unique_it); // increment the number of unique values
      n_multiplets+=local_average_val.size(); // increment the total number of multiplets (unique and non-unique)

      // Break ties by adding i times epsilon, using the order determined by neighboring values
      for(int i=0; i<local_average_nodeid.size(); ++i) {
        int nodeid = local_average_nodeid[i].second;
        float val = initial_property_->get_value_no_check(nodeid)+i*epsilon;
        tiebroken_property_->set_value(val,nodeid);
      }
    }
    // move on to the next unique value:
    it = up;
  }

  float fraction_sorted_through_spatial_differences = 100.0*(float)(n_unique)/n_multiplets;
  GsTLcout <<"Using neighboring values as a sorting key, we resolved " << fraction_sorted_through_spatial_differences << "% of the ties. The other ties were resolved randomly." << gstlIO::end;

  return true;

}

Named_interface* 
Break_ties_spatial::create_new_interface( std::string& ) {
  return new Break_ties_spatial; 
}



//format GridName::InputProp::SecondaryProp[::OutputProp::RegionName]
// if OutputProp not specified the suffix "tie_broken" is added to the input name
bool Break_ties_with_secondary_property::init( std::string& parameters, GsTL_project* proj,Error_messages_handler* errors ) {
  std::vector< std::string > params = String_Op::decompose_string(parameters,Actions::separator,Actions::unique );

  if( params.size() < 3 ) return false;

  std::string grid_name = params[0];

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + grid_name );
  grid_ = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid_ ) {
    std::ostringstream message;
    message << "No grid called \"" << grid_name << "\" was found";
    errors->report( message.str() );
    return false;
  }

  initial_property_ = grid_->property( params[1] );
  if(initial_property_ == 0) {
    errors->report( "No property called "+params[1] );
    return false;
  }

  secondary_property_ = grid_->property( params[2] );
  if(initial_property_ == 0) {
    errors->report( "No property called "+params[2] );
    return false;
  }

  std::string out_name;
  if(params.size() >= 4) {
    out_name = params[3];
  } else {
    out_name = initial_property_->name() + "-tie-broken-with-" + params[2];
  }
  tiebroken_property_ = grid_->add_property( out_name );
  while(!tiebroken_property_) {
    out_name.append("_0");
    tiebroken_property_ = grid_->add_property( out_name );
  }


  region_ = 0;
  if(params.size()==5) {
    region_ = grid_->region(params[4]);
    if( region_ == 0) {
      errors->report( "No region called "+params[4] );
      return false;
    }
  } 

  return true;
}

bool Break_ties_with_secondary_property::exec(  ) {

  std::vector<std::pair<float, int> > z_nodeid;
  int ncells = initial_property_->size();
  z_nodeid.reserve(ncells);

  // Create pair with all valid grid block property values and their cell index
  for(int i=0; i < ncells; ++i) {
    if(region_  && !region_->is_inside_region(i)) continue;
    if(!initial_property_->is_informed(i)) continue;
    z_nodeid.push_back( std::make_pair(initial_property_->get_value_no_check(i),i) );
  }
  
  std::sort(z_nodeid.begin(),z_nodeid.end());

  // Machine precision epsilon
  float epsilon = 100*std::numeric_limits<float>::epsilon();

  Global_random_number_generator::instance()->seed( 12127317 );
  STL_generator gen;

  std::vector<std::pair<float,int> >::iterator up,it;
  it=z_nodeid.begin();
  int n_unique(0);
  int n_multiplets(0);
  
  while(it != z_nodeid.end()) {
    std::pair<float,int> last_pair(it->first,ncells);
    up  = std::lower_bound(it,z_nodeid.end(),last_pair); // up is the last position of the value we look for (here: last_pair)
    int number_of_ties = up-it;
    if (number_of_ties==1){
      tiebroken_property_->set_value(it->first,it->second);
      // No multiplets
    }else{

      // We have multiplets
      std::vector<std::pair<float, int> > secondary_nodeid;
      std::vector<float> secondary_val;
      std::vector<std::pair<float,int> >::iterator it_ties=it;
      for ( ; it_ties!=up ; ++it_ties){
        secondary_val.push_back(secondary_property_->get_value_no_check(it_ties->second));
        secondary_nodeid.push_back(std::make_pair(  secondary_property_->get_value_no_check(it_ties->second)  ,it_ties->second) );
      }

      std::sort(secondary_nodeid.begin(),secondary_nodeid.end());
      std::sort(secondary_val.begin(),secondary_val.end());

      // Count the number of unique average values (and thus the fraction really sorted using space vs. randomly)
      std::vector<float>::iterator unique_it;
      unique_it = std::unique(secondary_val.begin(),secondary_val.end());
      n_unique += std::distance(secondary_val.begin(),unique_it); // increment the number of unique values
      n_multiplets+=secondary_val.size(); // increment the total number of multiplets (unique and non-unique)

      // Break ties by adding i times epsilon, using the order determined by neighboring values
      for(int i=0; i<secondary_nodeid.size(); ++i) {
        int nodeid = secondary_nodeid[i].second;
        float val = initial_property_->get_value_no_check(nodeid)+i*epsilon;
        tiebroken_property_->set_value(val,nodeid);
      }
    }
    // move on to the next unique value:
    it = up;
  }

  float fraction_sorted_through_spatial_differences = 100.0*(float)(n_unique)/n_multiplets;
  GsTLcout <<"Using neighboring values as a sorting key, we resolved " << fraction_sorted_through_spatial_differences << "% of the ties. The other ties were resolved randomly." << gstlIO::end;
  return true;

}

Named_interface* 
Break_ties_with_secondary_property::create_new_interface( std::string& ) {
  return new Break_ties_with_secondary_property; 
}


/*
bool Create_trend::is_coproperty_valid(std::string coproperty,Error_messages_handler* errors){
  std::set<std::string>::iterator it = coproperties_.find(coproperty);
    if(it == coproperties_.end()) {
    std::ostringstream message;
    message << "No direction \"" << coproperty << "\" is implemented \n The choices are:";
    for(it =coproperties_.begin(); it != coproperties_.end() ; it++ )
      message<<*it<<" ";
    errors->report( message.str() );
    return false;
  }
  return true;
}
*/