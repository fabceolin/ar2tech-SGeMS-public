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


#ifndef __NEAREST_NEIGHBOR_ESTIMATOR__ 
#define __NEAREST_NEIGHBOR_ESTIMATOR__ 

#include <geostat/common.h>
#include <grid/geostat_grid.h>
#include <grid/grid_property.h>
#include <grid/grid_region_temp_selector.h> 
#include <grid/neighborhood.h> 
#include <geostat/geostat_algo.h>


class GEOSTAT_DECL Nearest_neighbor_estimator : public Geostat_algo
{
public:
  Nearest_neighbor_estimator(void);
  virtual ~Nearest_neighbor_estimator(void);

	  virtual bool initialize( const Parameters_handler* parameters,
				   Error_messages_handler* errors );
	  virtual int execute( GsTL_project* proj=0 );
	  virtual std::string name() const { return "nearest-neighbor"; }

	 public:
	  static Named_interface* create_new_interface( std::string& );

private :
  int power_;
  Geostat_grid* grid_;
  Geostat_grid* hd_grid_;
  const Grid_continuous_property* hd_prop_;
  std::string property_name_;

  SmartPtr<Neighborhood> neighborhood_; 

  Grid_region* target_grid_region_;
  Grid_region* hd_region_;

};

#endif
