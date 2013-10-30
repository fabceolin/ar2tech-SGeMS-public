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
/*
#define _SECURE_SCL 0 
#ifdef _DEBUG
#define _HAS_ITERATOR_DEBUGGING 0
#endif
*/

#include <GsTL/geometry/geometry_algorithms.h>
#include <grid/point_set_neighborhood.h>
#include <grid/common.h>

#include <GsTL/geometry/covariance.h>
//#include <GsTL/utils/ann_kd.h>

#include <algorithm>


// Comparison of 2 geovalues g1 and g2 at locations u1 and u2:
// g1 < g2 iff cov(u1,u) < cov(u2,u)  for a given location u.
// Covariance_distance_( g1, g2 ) is true if g1 > g2
class GRID_DECL Covariance_distance_ {
public:
  Covariance_distance_( const GsTLPoint& center,
			const Covariance<GsTLPoint>& cov )
    : center_(center ),
      cov_( cov ) {}

  bool operator () ( const Geovalue& P1, const Geovalue& P2 ) {
    return cov_(P1.location(),center_) > cov_(P2.location(),center_);
  }
  
private:
  GsTLPoint center_;
  Covariance<GsTLPoint> cov_;
};



Point_set_neighborhood::Point_set_neighborhood( double x,double y,double z,
						double ang1,double ang2,double ang3,
						int max_neighbors,
						Point_set* pset,
						Grid_continuous_property* property,
						const Covariance<location_type>* cov, bool only_harddata,
            const Grid_region* region,
            Coordinate_mapper* coord_mapper)
  : pset_( pset ),
    property_( property ),
    max_neighbors_( max_neighbors ),
    norm_( x,y,z, ang1, ang2, ang3 ),
    only_harddata_(only_harddata),
    use_n_closest_(only_harddata){

    region_ = region;

   // use_only_informed_node_ = false;
    
    appli_assert((x>=y)&&(y>=z)&&(x>=z));
    appli_assert((x>0)&&(y>0)&&(z>0));
    
    a_=x; b_=y; c_=z;
    ang1_=ang1; ang2_=ang2; ang3_=ang3;

    //cov_ = 0;
	// cov_ must be initiated
	if( cov ) 
      cov_ = new Covariance<location_type>( *cov );
  else {

    cov_ = new Covariance<GsTLPoint>();
    int id = cov_->add_structure( "Spherical" );
    
    cov_->set_geometry( id,x, y, z, ang1,ang2,ang3);

  }

	coord_transform_ = new CoordTransform(a_,b_,c_,ang1_,ang2_,ang3_);


  //use_only_informed_node_ =  pset_->selected_property() == property_;
  //use_only_informed_node_ = true;
  int nInformed = 0; 
  if( (use_n_closest_)) {
//    int nInformed = 0;
    for(int i=0; i<pset_->size(); ++i) {
      if(property_->is_informed(i) ) {
        if( region_ && !region_->is_inside_region(i)) continue;
        nInformed++;
      }
    }
//    coords_ = new boost::multi_array<GsTLCoord,2>(boost::extents[nInformed][3]);
//    idx_.reserve( nInformed );
  }
  else {
//    int nInformed = 0;
    for(int i=0; i<pset_->size(); ++i) {
      if( region_ && !region_->is_inside_region(i)) continue;
      nInformed++;
    }
//    coords_ = new boost::multi_array<GsTLCoord,2>(boost::extents[nInformed][3]);
//    idx_.reserve( nInformed );
  }
  coords_ = new boost::multi_array<GsTLCoord,2>(boost::extents[nInformed][3]);
  idx_.reserve( nInformed );

  const std::vector<GsTLPoint>& locs = pset_->point_locations();
  
  //If no change of coordinates is required
  if(coord_mapper == 0) {
    if( use_n_closest_ ) {
      index ii = 0;
      for( int i=0; i < locs.size() ; i++ ) {
        if( !property_->is_informed(i) ) continue;
        if( region_ && !region_->is_inside_region(i)) continue;
		    location_type loc = (*coord_transform_)(locs[i]);
        (*coords_)[ii][0] = loc[0];
        (*coords_)[ii][1] = loc[1];
        (*coords_)[ii][2] = loc[2];
        ii++;
        idx_.push_back(i);
  //          coords[i][3] = i;
	    }
    } else  {
        index ii = 0;
        for( int i=0; i < locs.size() ; i++ ) {
          if( region_ && !region_->is_inside_region(i)) continue;
		        location_type loc = (*coord_transform_)(locs[i]);
            (*coords_)[ii][0] = loc[0];
            (*coords_)[ii][1] = loc[1];
            (*coords_)[ii][2] = loc[2];
            idx_.push_back(i);
            ii++;
        }
	  }
  } 
  else { // Need a change of coordinate.  Splitted in two to avoid duplicate memory when no changes are needed
    geol_coords_.reserve(nInformed);

    if( use_n_closest_ ) {
      index ii = 0;
      for( int i=0; i < locs.size() ; i++ ) {
        if( !property_->is_informed(i) ) continue;
        if( region_ && !region_->is_inside_region(i)) continue;
        geol_coords_.push_back( coord_mapper->uvw_coords(locs[i]) );
        location_type loc = (*coord_transform_)(geol_coords_.back());
        (*coords_)[ii][0] = loc[0];
        (*coords_)[ii][1] = loc[1];
        (*coords_)[ii][2] = loc[2];
        ii++;
        idx_.push_back(i);
  //          coords[i][3] = i;
	    }
    } else  {
        index ii = 0;
        for( int i=0; i < locs.size() ; i++ ) {
          if( region_ && !region_->is_inside_region(i)) continue;
            geol_coords_.push_back( coord_mapper->uvw_coords(locs[i]) );
            location_type loc = (*coord_transform_)(geol_coords_.back());
            (*coords_)[ii][0] = loc[0];
            (*coords_)[ii][1] = loc[1];
            (*coords_)[ii][2] = loc[2];
            idx_.push_back(i);
        }
	  }

  }



  kdtree_ = new kdtree2(*coords_, property_, &idx_, true, 3 );
  kdtree_->sort_results = true;
  //kdtree_->set_informatation_property(property_, &idx_);


}


Point_set_neighborhood::~Point_set_neighborhood() {
  delete cov_;

  delete kdtree_;
  delete coords_;

}

void Point_set_neighborhood::max_size( int s ) {
  max_neighbors_ = std::min( s, pset_->size() );

} 



bool Point_set_neighborhood::
select_property( const std::string& prop_name ) {
  if(property_ && prop_name == property_->name()) return true;
  property_ = pset_->property( prop_name );

  if(!use_n_closest_) kdtree_->set_property(property_);
  
  else {
    delete kdtree_;
    delete coords_;
    idx_.clear();
    int nInformed = 0;
    for(int i=0; i<pset_->size(); ++i) {
      if( region_ && !region_->is_inside_region(i)) continue;
      if(property_->is_informed(i)) nInformed++;
    }
    coords_ = new boost::multi_array<GsTLCoord,2>(boost::extents[nInformed][3]);
    idx_.reserve( nInformed );
    index ii = 0;
    const std::vector<GsTLPoint>& locs = pset_->point_locations();
    for( int i=0; i < locs.size() ; i++ ) {
      if( !property_->is_informed(i) ) continue;
      if( region_ && !region_->is_inside_region(i)) continue;
		    location_type loc = (*coord_transform_)(locs[i]);
        (*coords_)[ii][0] = loc[0];
        (*coords_)[ii][1] = loc[1];
        (*coords_)[ii][2] = loc[2];
        ii++;
        idx_.push_back(i);
    }
    kdtree_ = new kdtree2(*coords_, property_, &idx_, true, 3 );
    kdtree_->sort_results = true;
  }


  return ( property_ != 0 );
}



void Point_set_neighborhood::find_neighbors(const Geovalue& center) {
  neighbors_.clear();
  candidates_.clear();
  neigh_filter_->clear();
  if( !property_ ) return;

  center_ = center;
  center_.set_property_array( property_ );


  location_type loc = (*coord_transform_)(center.location());
  double a2 = a_*a_;
  std::vector<GsTLCoord> center_node;
  center_node.push_back(loc[0]);
  center_node.push_back(loc[1]);
  center_node.push_back(loc[2]);

  kdtree2_result_vector nearest;

  if(use_n_closest_) {
 
    if(includes_center_) {
      kdtree_->n_nearest(center_node, max_neighbors_, nearest);

      for (unsigned int i=0; i<nearest.size(); i++) {
        if(nearest[i].dis < a2 ) 
           neighbors_.push_back( Geovalue( pset_, property_, idx_[nearest[i].idx] ) );
          if(!geol_coords_.empty()) {
            neighbors_.back().set_cached_location(geol_coords_[nearest[i].idx]);
          }
      }
    }
    else {
      kdtree_->n_nearest(center_node, max_neighbors_+1, nearest); // in case there is the center

      for (unsigned int i=0; i<nearest.size(); i++) {
        if(nearest[i].dis < a2 && nearest[i].dis > 0) 
           neighbors_.push_back( Geovalue( pset_, property_, idx_[nearest[i].idx] ) );

          //If the coord_mapper was used must cache the geological coordinates
          if(!geol_coords_.empty()) {
            neighbors_.back().set_cached_location(geol_coords_[nearest[i].idx]);
          }
      }
    }

  }
  else {

    kdtree_->r_nearest(center_node, a2, nearest);
    int n_neigh = 0;
    for (unsigned int i=0; i<nearest.size()&& n_neigh < max_neighbors_; i++) {
      if( !includes_center_ && nearest[i].dis == 0) continue;
      int node_id = idx_[nearest[i].idx];
  //    if(property_->is_informed(id) ) {
        Geovalue gval( pset_, property_, node_id );
        if(neigh_filter_->is_admissible(gval, center)) {
          //If the coord_mapper was used must cache the geological coordinates
          if(!geol_coords_.empty()) {
            gval.set_cached_location(geol_coords_[nearest[i].idx]);
          }
          neighbors_.push_back( gval );
          n_neigh++;
        }
   //     neighbors_.push_back( Geovalue( pset_, property_, node_id ) );
   //     n_neigh++;
    //  }
    }
  }

  std::sort(neighbors_.begin(), neighbors_.end(),
          Covariance_distance_( center.location(), *cov_ ) );

  if(!includes_center_ && neighbors_.size() > max_neighbors_) {
    neighbors_.pop_back();
  }
  
/*  
  if( neigh_filter_->is_neighborhood_valid() ) 
    std::sort(neighbors_.begin(), neighbors_.end(),
          Covariance_distance_( center.location(), *cov_ ) );
  else
    neighbors_.clear();
*/


}


void 
Point_set_neighborhood::find_neighbors( const Geovalue& center , Neighbors & neighbors ) const 
{
  neighbors.clear();
  neighbors.center_is(center);
  std::vector<int> candidates;
  candidates.clear();
  neigh_filter_->clear();
  if( !property_ ) return;


  location_type loc = (*coord_transform_)(center.location());
  double a2 = a_*a_;
  std::vector<GsTLCoord> center_node;
  center_node.push_back(loc[0]);
  center_node.push_back(loc[1]);
  center_node.push_back(loc[2]);

  kdtree2_result_vector nearest;

  if(use_n_closest_) {
 
    if(includes_center_) {
      kdtree_->n_nearest(center_node, max_neighbors_, nearest);

      for (unsigned int i=0; i<nearest.size(); i++) {
        if(nearest[i].dis < a2 ) 
           neighbors.push_back( Geovalue( pset_, property_, idx_[nearest[i].idx] ) );
          if(!geol_coords_.empty()) {
            neighbors.back().set_cached_location(geol_coords_[nearest[i].idx]);
          }
      }
    }
    else {
      kdtree_->n_nearest(center_node, max_neighbors_+1, nearest); // in case there is the center

      for (unsigned int i=0; i<nearest.size(); i++) {
        if(nearest[i].dis < a2 && nearest[i].dis > 0) 
           neighbors.push_back( Geovalue( pset_, property_, idx_[nearest[i].idx] ) );

          //If the coord_mapper was used must cache the geological coordinates
          if(!geol_coords_.empty()) {
            neighbors.back().set_cached_location(geol_coords_[nearest[i].idx]);
          }
      }
    }

  }
  else {

    kdtree_->r_nearest(center_node, a2, nearest);
    int n_neigh = 0;
    for (unsigned int i=0; i<nearest.size()&& n_neigh < max_neighbors_; i++) {
      if( !includes_center_ && nearest[i].dis == 0) continue;
      int node_id = idx_[nearest[i].idx];
  //    if(property_->is_informed(id) ) {
        Geovalue gval( pset_, property_, node_id );
        if(neigh_filter_->is_admissible(gval, center)) {
          //If the coord_mapper was used must cache the geological coordinates
          if(!geol_coords_.empty()) {
            gval.set_cached_location(geol_coords_[nearest[i].idx]);
          }
          neighbors.push_back( gval );
          n_neigh++;
        }
   //     neighbors.push_back( Geovalue( pset_, property_, node_id ) );
   //     n_neigh++;
    //  }
    }
  }

  std::sort(neighbors.begin(), neighbors.end(),
          Covariance_distance_( center.location(), *cov_ ) );

  if(!includes_center_ && neighbors.size() > max_neighbors_) {
    neighbors.pop_back();
  }

  neighbors.set_valid(neigh_filter_->is_neighborhood_valid());
  
/*  
  if( neigh_filter_->is_neighborhood_valid() ) 
    std::sort(neighbors.begin(), neighbor_.end(),
          Covariance_distance_( center.location(), *cov_ ) );
  else
    neighbors.clear();
*/


}



void Point_set_neighborhood::
set_neighbors( const_iterator begin, const_iterator end ) {
  neighbors_.clear();
  const_iterator it = begin;
  for( int i =0 ; it != end ; ++it, ++i ) {
    const Geostat_grid* g_grid = it->grid();
    if( g_grid != pset_ ) continue;

    neighbors_.push_back( *it );
    neighbors_[i].set_property_array( property_ );
  }
}



/*
 ------------------------------------
*/

Point_set_rectangular_neighborhood::Point_set_rectangular_neighborhood( double x,double y,double z,
						double ang1,double ang2,double ang3,
						int max_neighbors,
						Point_set* pset,
						Grid_continuous_property* prop,
						const Covariance<location_type>* cov, bool only_harddata ) 
: Point_set_neighborhood( std::sqrt(x*x+y*y+x*z)/2, std::sqrt(x*x+y*y+x*z)/2, std::sqrt(x*x+y*y+x*z)/2,
                          ang1, ang2, ang3, max_neighbors, pset,prop, cov, only_harddata ),
  rx_(x), ry_(y), rz_(z)
{

}

// Need to filter the neighbors and only keep those inside the box
void Point_set_rectangular_neighborhood::find_neighbors(const Geovalue& center){
  Point_set_neighborhood::find_neighbors(center);

  std::vector<Geovalue> rect_neighbors;

  std::vector<Geovalue>::iterator it = neighbors_.begin();
  GsTLCoord cx = center_.location().x();
  GsTLCoord cy = center_.location().y();
  GsTLCoord cz = center_.location().z();
  
  for( ; it != neighbors_.end(); ++it) {
    Geostat_grid::location_type loc = it->location();
    if( std::fabs(loc.x() - cx) < rx_  &&
        std::fabs(loc.y() - cy) < rx_ &&
        std::fabs(loc.z() - cz) < rz_ ) {
        
          rect_neighbors.push_back(*it);
    }
  }
  neighbors_.clear();
  neighbors_.insert(neighbors_.begin(),rect_neighbors.begin(),rect_neighbors.begin() );
}

void
Point_set_rectangular_neighborhood::find_neighbors(const Geovalue& center, Neighbors & neighbors) const
{
  Point_set_neighborhood::find_neighbors(center , neighbors);

  std::vector<Geovalue> rect_neighbors;

  std::vector<Geovalue>::iterator it = neighbors.begin();
  GsTLCoord cx = center.location().x();
  GsTLCoord cy = center.location().y();
  GsTLCoord cz = center.location().z();
  
  for( ; it != neighbors.end(); ++it) {
    Geostat_grid::location_type loc = it->location();
    if( std::fabs(loc.x() - cx) < rx_  &&
        std::fabs(loc.y() - cy) < rx_ &&
        std::fabs(loc.z() - cz) < rz_ ) {
        
          rect_neighbors.push_back(*it);
    }
  }
  neighbors.clear();
  neighbors.insert(neighbors.begin(), rect_neighbors.begin(), rect_neighbors.begin() );
}