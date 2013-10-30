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
** This file is part of the "grid" module of the Geostatistical Earth
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

#include <utils/manager.h>

#include <grid/point_set.h>
#include <grid/grid_property_manager.h>
#include <grid/point_set_neighborhood.h>
#include <utils/string_manipulation.h>
#include <math/random_numbers.h>

#include <GsTL/math/math_functions.h>

#include <algorithm>


Named_interface* Point_set::create_new_interface( std::string& name_and_size_str ) {

  if( name_and_size_str.empty() )
    return 0;

  String_Op::string_pair params = String_Op::split_string(name_and_size_str, "::",true);
  int size = String_Op::to_number<int>( params.second );
  appli_assert( size >= 0 );
  return new Point_set( params.first,size );

}


Point_set::Point_set(std::string name, int size)
  : Geostat_grid(name), xyz_point_loc_( size ), point_loc_( size ) {
  
//	point_prop_ = new Grid_property_manager();
  property_manager_.set_prop_size(size);
  property_manager_.set_parent_item(this);
  region_manager_.set_region_size( size );
  region_manager_.set_parent_item(this);
  group_manager_.set_parent_item(this);
}


Point_set::~Point_set()
{
//	delete point_prop_;
   
}

void Point_set::point_locations( const std::vector<location_type>& locations ) { 
    xyz_point_loc_ = locations; 
    GsTLCoord minx  = locations[0].x();
    GsTLCoord miny  = locations[0].y();
    GsTLCoord minz  = locations[0].z();
    GsTLCoord maxx  = locations[0].x();
    GsTLCoord maxy  = locations[0].y();
    GsTLCoord maxz  = locations[0].z();
    
    GsTLCoord x,y,z;
    for(int i=1; i< xyz_point_loc_.size(); ++i) {
      x =  xyz_point_loc_[i].x();
      y =  xyz_point_loc_[i].y();
      z =  xyz_point_loc_[i].z();
      if(x < minx ) minx = x;
      if(y < miny ) miny = y;
      if(z < minz ) minz = z;
      if(x > maxx ) maxx = x;
      if(y > maxy ) maxy = y;
      if(z > maxz ) maxz = z;
    }
    point_loc_.clear();
    point_loc_.insert(point_loc_.begin(), xyz_point_loc_.begin(), xyz_point_loc_.end() );

    bbox_ = std::make_pair( location_type(minx,miny,minz), location_type(maxx,maxy,maxz) );

  } 


Neighborhood* Point_set::neighborhood( double x, double y, double z,
				       double ang1, double ang2, double ang3,
				       const Covariance<location_type>* cov,
				       bool only_harddata,
               const Grid_region* region,
               Coordinate_mapper* coord_mapper) {
  const int max_neighbors = 20;
  return new Point_set_neighborhood( x,y,z, ang1,ang2,ang3,
				     max_neighbors, this, 
				     property_manager_.selected_property(),
				     cov, only_harddata, region,
             coord_mapper);
    
}


Neighborhood* Point_set::neighborhood( const GsTLTripletTmpl<double>& dim,
				       const GsTLTripletTmpl<double>& angles,
				       const Covariance<location_type>* cov,
				       bool only_harddata,
               const Grid_region* region,
               Coordinate_mapper* coord_mapper) {
  const int max_neighbors = 20;
  return new Point_set_neighborhood( dim[0], dim[1], dim[2],
				     angles[0], angles[1], angles[2], 
				     max_neighbors, this,
				     property_manager_.selected_property(),
				     cov, only_harddata, region, 
             coord_mapper );
}



QString Point_set::item_type() const{
	return QString(this->classname().c_str());
}

GsTL_object_item *Point_set::child(int row){
	if(row == 0) {
		return &property_manager_;
	}
	else if(row < group_manager_.size() +1) {
		std::map<std::string, Grid_property_group*>::iterator  it = group_manager_.begin_group();
		for(int i=1; i<row; ++i, ++it){}
		return it->second;
	}
	else return &region_manager_;

}

const GsTL_object_item *Point_set::child(int row) const {
	if(row == 0) {
		return &property_manager_;
	}
	else if(row < group_manager_.size() +1) {
		std::map<std::string, Grid_property_group*>::const_iterator  it = group_manager_.begin_group();
		for(int i=1; i<row; ++i, ++it){}
		return it->second;
	}
	else return &region_manager_;

}

int Point_set::childCount() const{
	return group_manager_.size()+2;

}

int Point_set::columnCount() const{
	return 2;

}

QVariant Point_set::item_data(int column) const{
	if(column ==0)
		return QString(name_.c_str());
	if(column ==1)
		return this->item_type();

	return QVariant();

}

void Point_set::set_coordinate_mapper(Coordinate_mapper* coord_mapper){

 if(coord_mapper == 0 && coord_mapper_ == 0) return;
 if(coord_mapper_ == coord_mapper ) return;  // potential problem if coord_mapper was updated, then should first set 0 then reset the coord mapper

 coord_mapper_ = coord_mapper;
 point_loc_.clear();
 
 if(coord_mapper == 0) {
   point_loc_.insert(point_loc_.begin(), xyz_point_loc_.begin(),xyz_point_loc_.end());
 }
 else {
   point_loc_.reserve(xyz_point_loc_.size());
   for(int i=0; i<xyz_point_loc_.size(); ++i ) {
     point_loc_.push_back( coord_mapper_->uvw_coords(xyz_point_loc_[i]) );
   }
 }
}


