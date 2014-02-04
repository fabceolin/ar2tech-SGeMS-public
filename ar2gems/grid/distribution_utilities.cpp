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

// to remove multiple values
#include <algorithm>    // std::lower_bound, std::upper_bound, std::sort

#include <grid/distribution_utilities.h>
#include <utils/manager.h>
#include <utils/manager.h>
#include <utils/manager_repository.h>

Non_parametric_distribution* 
  build_non_param_distribution( std::string name, Grid_continuous_property* prop, 
    const Tail_interpolator low_interp,
		const Linear_interpol mid_interp,
		const Tail_interpolator up_interp	)  
{
  Non_parametric_distribution* dist;
  if( name.empty() ) {
    dist = new Non_parametric_distribution();
  }
  else {
    Named_interface* ni = Root::instance()->new_interface("Non-parametric",continuous_distributions_manager+"/"+name).raw_ptr();
    dist = dynamic_cast<Non_parametric_distribution*>(ni);
    if(dist == 0) return 0;
  }

  std::vector<float> values(prop->begin(), prop->end());
  std::sort(values.begin(), values.end());

  int n = values.size();
  std::vector<float> cdf;
  cdf.reserve(n);
  n += 1;
  for(int i=1; i<=values.size(); ++i) {
    cdf.push_back( float(i)/n );
  }

  dist->initialize(values.begin(),values.end(),cdf.begin(), low_interp, mid_interp, up_interp);

  return dist;

};


Non_parametric_distribution* 
  build_non_param_distribution( std::string name,
  const Grid_continuous_property* prop, const Grid_region* region, 
  const Tail_interpolator low_interp,
	const Linear_interpol mid_interp,
	const Tail_interpolator up_interp	)  
{

  Non_parametric_distribution* dist;
  if( name.empty() ) {
    dist = new Non_parametric_distribution();
  }
  else {
    Named_interface* ni = Root::instance()->new_interface("Non-parametric",continuous_distributions_manager+"/"+name).raw_ptr();
    dist = dynamic_cast<Non_parametric_distribution*>(ni);
    if(dist == 0) return 0;
  }

  std::vector<float> values;
 
  values.reserve(region->active_size());
  double p = 0.0; 
  for(int i=0; i<prop->size(); ++i) {
    if( prop->is_informed(i) &&  region->is_inside_region(i)) {
      values.push_back( prop->get_value(i) );
    }
  }
  std::sort(values.begin(), values.end());
  int n = values.size();
  std::vector<float> cdf;
  cdf.reserve(n);
  n += 1;
  for(int i=1; i<=values.size(); ++i) {
    cdf.push_back( float(i)/n );
  }

  dist->initialize(values.begin(),values.end(),cdf.begin(), low_interp, mid_interp, up_interp);

  return dist;
};


Non_parametric_distribution* 
  build_non_param_distribution( std::string name,const Grid_continuous_property* prop, const Grid_weight_property* weight, 
  const Tail_interpolator low_interp,
	const Linear_interpol mid_interp,
	const Tail_interpolator up_interp	)  
{

  Non_parametric_distribution* dist;
  if( name.empty() ) {
    dist = new Non_parametric_distribution();
  }
  else {
    Named_interface* ni = Root::instance()->new_interface("Non-parametric",continuous_distributions_manager+"/"+name).raw_ptr();
    dist = dynamic_cast<Non_parametric_distribution*>(ni);
    if(dist == 0) return 0;
  }

  typedef std::pair<float, double> z_p_pair_type;
  std::vector<z_p_pair_type> values,clean_values;
  for(int i=0; i<prop->size(); ++i) {
    if( !prop->is_informed(i) || !weight->is_informed(i) ) continue;
    double w = weight->get_value(i);
    if(w == 0) continue;
    values.push_back(  std::make_pair(prop->get_value(i), w) );
  }


  initialize_data_for_distribution(values,clean_values);

  dist->initialize(clean_values, low_interp, mid_interp, up_interp);

  bool ok = dist->is_valid_distribution();

  return dist;

};


/// \brief Sort and remove multiplets from a vector of pairs, using the first value of the pair as a key. Sum-aggregate to 1 the second values for a given key.
/// \param Vector of pairs made of a float and a double
/// \param[out] Vector of pairs made of a float and a double
template <
  typename z_type,
  typename p_type 
>
 void initialize_data_for_distribution ( std::vector<std::pair<z_type,p_type> >& input_pair, std::vector<std::pair<z_type,p_type> >& output_pair) {

  output_pair.clear();
  output_pair.reserve(input_pair.size());
  std::sort(input_pair.begin(), input_pair.end());    // Sorting on pairs works on the first element

  std::vector<std::pair<z_type,p_type> >::iterator up,it;

  p_type sum_p = 0.0;
  it=input_pair.begin();
  while(it != input_pair.end()) {
    std::pair<z_type,p_type> last_pair(it->first,1.0);
    up  = std::lower_bound(it,input_pair.end(),last_pair); // up is the last position of the value we look for (here: last_pair)
    z_type z = it->first;
    // cumulative sum of weights:
    for (  ; it != up ; ++it ){
      sum_p +=  it->second;
    }
    it = up;
    
    // reconstruct pair
    output_pair.push_back(  std::make_pair(z, sum_p) );
  }
  
  // Impose 1.00 as last cumulative sum value of weights/probabilities:
  // (should maybe be rewritten for a division by max, instead of plateau at 1)
  if(output_pair.back().second > 1.00 ) output_pair.back().second = 1.00;


};