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




#ifndef __NONPARAM_DISTRIBUTION_BUILDER_H__ 
#define __NONPARAM_DISTRIBUTION_BUILDER_H__ 

#include <math/non_parametric_distribution.h>
#include <grid/grid_property.h>
#include <grid/grid_region.h>
#include <grid/grid_weight_property.h>

#include <boost/math/distributions.hpp>

#include <GsTL/cdf/non_param_cdf.h>
#include <GsTL/cdf/interpolators.h>
#include <GsTL/univariate_stats/build_cdf.h>
#include <grid/common.h>

//If name is not specifed (empty string) then the distribution is not managed

GRID_DECL Non_parametric_distribution* 
  build_non_param_distribution( std::string name, Grid_continuous_property* prop, 
    const Tail_interpolator low_interp = Tail_interpolator( new No_TI() ),
		const Linear_interpol mid_interp  = Linear_interpol(),
		const Tail_interpolator up_interp  = Tail_interpolator( new No_TI() )
		); 


GRID_DECL Non_parametric_distribution* 
  build_non_param_distribution( std::string name, const Grid_continuous_property* prop, const Grid_region* region, 
    const Tail_interpolator low_interp = Tail_interpolator( new No_TI() ),
		const Linear_interpol mid_interp  = Linear_interpol(),
		const Tail_interpolator up_interp  = Tail_interpolator( new No_TI() )
		);


GRID_DECL Non_parametric_distribution* 
  build_non_param_distribution( std::string name, const Grid_continuous_property* prop, const Grid_weight_property* weight, 
    const Tail_interpolator low_interp = Tail_interpolator( new No_TI() ),
		const Linear_interpol mid_interp  = Linear_interpol(),
		const Tail_interpolator up_interp  = Tail_interpolator( new No_TI() )
		);


template <
  typename z_type,
  typename p_type 
>
GRID_DECL void initialize_data_for_distribution ( std::vector<std::pair<z_type,p_type> >& input_pair, std::vector<std::pair<z_type,p_type> >& output_pair);


#endif

