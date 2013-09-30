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

#ifndef __GSTLAPPLI_POINTSET_H__ 
#define __GSTLAPPLI_POINTSET_H__ 
 
 
 
#include <grid/common.h>
#include <grid/gval_iterator.h> 
#include <math/gstlpoint.h> 
#include <grid/geostat_grid.h> 
#include <grid/grid_property_manager.h> 
#include <grid/grid_region_manager.h>  
#include <grid/grid_property_set.h> 
#include <string> 
#include <vector> 
 


class Grid_continuous_property;
class Grid_categorical_property;
class Point_set_neighborhood; 
 
 
 
class GRID_DECL Point_set : public Geostat_grid { 

 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  //typedef Gval_iterator<LinearMapIndex> iterator; 
  Point_set():Geostat_grid(){}
  Point_set(std::string name, int size);
   
  virtual ~Point_set(); 
   
  virtual void point_locations( const std::vector<location_type>& locations ); 
  const std::vector<location_type>& point_locations() const { return point_loc_;}
  const std::vector<location_type>& xyz_point_locations() const { return xyz_point_loc_;}

  virtual void set_coordinate_mapper(Coordinate_mapper* coord_mapper);
 
  virtual std::pair<Geostat_grid::location_type, Geostat_grid::location_type> bounding_box() const {return bbox_;}

  // Returns the most specific name of the current class 
  virtual std::string classname() const { return "Point_set"; } 

  
  
  //-------------------------------- 
  // neighborhood definition 
  virtual Neighborhood* neighborhood( double x, double y, double z, 
                                      double ang1, double ang2, double ang3, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false, const Grid_region* region = 0,
              Coordinate_mapper* coord_mapper=0); 
 
  virtual Neighborhood* neighborhood( const GsTLTripletTmpl<double>& dim, 
                                      const GsTLTripletTmpl<double>& angles, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false, const Grid_region* region = 0,
              Coordinate_mapper* coord_mapper=0); 
 
 
  /** Computes the location of a node, given its node_id. 
   */ 
  virtual location_type location( int node_id ) const; 
  virtual location_type xyz_location( int node_id ) const; 
 
  virtual GsTLInt node_id( GsTLInt index ) const { return index; } 
   
  virtual GsTLInt size() const {return xyz_point_loc_.size();} 
 
  Geovalue geovalue(int node_id); 
 
  virtual GsTLInt closest_node( const location_type& P ) const { 
    appli_warning( "NOT YET IMPLEMENTED"); 
    return -1; 
  } 
 
// GsTL_object_item implementation
  virtual QString item_type() const;
  virtual GsTL_object_item *child(int row);
  virtual const GsTL_object_item *child(int row) const;
  virtual int childCount() const;
  virtual int columnCount() const;
  virtual QVariant item_data(int column) const;

  virtual GsTL_object_item* regions_item(){
	  return &region_manager_;
  }


protected:
  std::vector<location_type> xyz_point_loc_; 
  std::vector<location_type> point_loc_;

  std::pair<location_type,location_type> bbox_;
 
 private: 
  Point_set( const Point_set& ); 
  Point_set& operator = ( const Point_set& ); 
};  
 
 
 
 
//========================= 
 
inline 
Geovalue Point_set:: geovalue(int node_id) 
{ 
  Geovalue temp(this,property_manager_.selected_property(),node_id);
      return temp; 
} 
 
inline 
Point_set::location_type Point_set::location( int node_id ) const { 
     appli_assert( node_id >= 0 && node_id <(signed int)( point_loc_.size()) ); 
     return point_loc_[node_id]; 
  } 
 
inline 
Point_set::location_type Point_set::xyz_location( int node_id ) const { 
     appli_assert( node_id >= 0 && node_id <(signed int)( xyz_point_loc_.size()) ); 
     return xyz_point_loc_[node_id]; 
  }  
 
 
#endif 
