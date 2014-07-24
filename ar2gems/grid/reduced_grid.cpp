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
** Author: Ting Li
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

#include <grid/reduced_grid.h>
#include <grid/geostat_grid.h>
#include <grid/mgrid_neighborhood.h>
//#include <grid/maskedgridcursor.h>

Named_interface* create_reduced_grid( std::string& name_and_size_str){
  if( name_and_size_str.empty() ) return 0;


  String_Op::string_pair params = String_Op::split_string(name_and_size_str, "::",true);
  if( params.second.empty() ) {
	  return new Reduced_grid(params.first);
  }
  else {
	  int size = String_Op::to_number<int>( params.second );
	  appli_assert( size >= 0 );
	  return new Reduced_grid( params.first,size );
  }


}

Reduced_grid::Reduced_grid(std::string name, int size) :
	Cartesian_grid(name), active_size_(size),mgrid_cursor_(0)
{
  property_manager_.set_prop_size( size );
  region_manager_.set_region_size( size );
}

Reduced_grid::Reduced_grid() : active_size_(0),mgrid_cursor_(0)  {}

Reduced_grid::Reduced_grid(std::string name)
	: Cartesian_grid(name),active_size_(0),mgrid_cursor_(0)  {}

Reduced_grid::~Reduced_grid(){
  delete mgrid_cursor_;
  // to vaoid grid_cursor_ of being deleted  once more by the rgrid destructor.
  grid_cursor_ = 0;
}

Neighborhood* Reduced_grid::neighborhood( double rad1, double rad2, double rad3,
				   double ang1, double ang2, double ang3,
				   const Covariance<location_type>* cov,
				   bool only_harddata,
           const Grid_region* region,
           Coordinate_mapper* coord_mapper) {


  // The ranges of the covariance of a Neighborhood must be expressed
  // in "number of cells", while they are supplied to the rgrid in 
  // "actual" unit. So do the convertion. 
  Covariance<location_type>* cov_copy = 0;

  if( cov ) {
    cov_copy = new Covariance<location_type>( *cov );
    for( int i=0; i < cov_copy->structures_count() ; i++ ) {
      double R1,R2,R3;
      cov_copy->get_ranges( i, R1,R2,R3 );
      cov_copy->set_ranges( i,
		       R1/geom_->cell_dims()[0],
		       R2/geom_->cell_dims()[1],
		       R3/geom_->cell_dims()[2] );
    }
  }

  if( only_harddata )
    return new MgridNeighborhood_hd( this, 
					     property_manager_.selected_property(),
					     rad1,rad2,rad3, ang1,ang2,ang3,
               this->size(), cov_copy, region );
  else
    return new MgridNeighborhood( this, 
					  property_manager_.selected_property(),
					  rad1,rad2,rad3, ang1,ang2,ang3,
					  this->size(), cov_copy, region  );

  delete cov_copy;


  delete cov_copy;
}



Neighborhood* Reduced_grid::neighborhood( const GsTLTripletTmpl<double>& dim,
				   const GsTLTripletTmpl<double>& angles,
				   const Covariance<location_type>* cov,
				   bool only_harddata,
           const Grid_region* region,
           Coordinate_mapper* coord_mapper) 
{
    return  this->neighborhood(dim.x(),dim.y(), dim.z(), angles.x(), angles.y(), angles.z(), 
                                cov, only_harddata, region, coord_mapper);
}

Window_neighborhood* Reduced_grid::window_neighborhood( const Grid_template& templ) {
  return new MgridWindowNeighborhood( templ, this,
					property_manager_.selected_property() );
}
  

// returns nx*ny*nz
GsTLInt Reduced_grid::rgrid_size() const {
	return RGrid::size();
}

const int Reduced_grid::full2reduced(int idInFullGrid)	const {
	if(idInFullGrid >= rgrid_size() ) return -1;
	return original2reduced_[idInFullGrid];
//	std::map<int,int>::const_iterator itr = original2reduced_.find(idInFullGrid);
//	if (itr == original2reduced_.end()) return -1;
//	return itr->second;
}

const int Reduced_grid::reduced2full(int idInReducedGrid) const{
	if(idInReducedGrid >= active_size_) return -1;
	return reduced2original_[idInReducedGrid];
}


inline Geostat_grid::location_type Reduced_grid::location( int reduced_node_id ) const {
	GsTLInt node_id =reduced2full(reduced_node_id);
	GsTLInt max_nxy = cgrid_geometry_->dim(0)*cgrid_geometry_->dim(1);
	GsTLInt inxy = node_id % max_nxy; 
	GsTLInt k = (node_id - inxy)/max_nxy; 
	GsTLInt j = (inxy - node_id%cgrid_geometry_->dim(0))/cgrid_geometry_->dim(0); 
	GsTLInt i = inxy%cgrid_geometry_->dim(0);

	return cgrid_geometry_->coordinates( i,j,k ); 
} 

//==============================================

//==============================================
// The following override the functions inherited
// This is re-implemnted since we cannot set the size of the
// property and region at this point
void Reduced_grid::set_geometry(RGrid_geometry* geom) {
	if( geom_ != geom ) {
		delete geom_;
		geom_ = geom->clone();
		connection_is_updated_ = false;
	}

  cgrid_geometry_ = dynamic_cast<Cartesian_grid_geometry*>( geom_ );

}


GsTLInt Reduced_grid::size() const { 
	return active_size_; 
} 


inline  
const SGrid_cursor* Reduced_grid::cursor() const { 
  return dynamic_cast<const SGrid_cursor*>(mgrid_cursor_); 
} 

inline  
SGrid_cursor* Reduced_grid::cursor() { 
  return dynamic_cast<SGrid_cursor*>(mgrid_cursor_); 
} 

void Reduced_grid::set_dimensions( int nx, int ny, int nz, 
		      double xsize, double ysize, double zsize,
          double origin_x, double origin_y, double origin_z,
          const std::vector<bool>& mask )
{
  Cartesian_grid::set_dimensions( nx, ny, nz, xsize, ysize, zsize, origin_x, origin_y, origin_z );

  mask_ = mask;
  mgrid_cursor_ = new MaskedGridCursor(nx, ny, nz);
  grid_cursor_ = dynamic_cast<SGrid_cursor*>(mgrid_cursor_);

  build_ijkmap_from_mask();
	mgrid_cursor_->set_mask(&original2reduced_, &reduced2original_, &mask_ );
  
  // Need to reset the size of the property and region
  active_size_ = mgrid_cursor_->max_index();
	property_manager_.set_prop_size( mgrid_cursor_->max_index() );
  region_manager_.set_region_size( mgrid_cursor_->max_index() );

}

void Reduced_grid::set_dimensions( int nx, int ny, int nz, 
		      double xsize, double ysize, double zsize,
          double origin_x, double origin_y, double origin_z,
          double rotation_angle_z, 
          double rot_pivot_x, double rot_pivot_y, double rot_pivot_z,
          const std::vector<bool>& mask )
{
  Cartesian_grid::set_dimensions( nx, ny, nz, xsize, ysize, zsize, origin_x, origin_y, origin_z,
                                  rotation_angle_z, rot_pivot_x, rot_pivot_y, rot_pivot_z);

  mask_ = mask;
  mgrid_cursor_ = new MaskedGridCursor(nx, ny, nz);
  grid_cursor_ = dynamic_cast<SGrid_cursor*>(mgrid_cursor_);

  build_ijkmap_from_mask();
  mgrid_cursor_->set_mask(&original2reduced_, &reduced2original_, &mask_ );
  
  // Need to reset the size of the property and region
  active_size_ = mgrid_cursor_->max_index();
  property_manager_.set_prop_size( mgrid_cursor_->max_index() );
  region_manager_.set_region_size( mgrid_cursor_->max_index() );
}




void Reduced_grid::set_dimensions( int nx, int ny, int nz,
    double xsize, double ysize, double zsize, 
    double origin_x, double origin_y, double origin_z)
{

  Cartesian_grid::set_dimensions(nx, ny, nz, xsize, ysize, zsize, origin_x, origin_y, origin_z);

  mgrid_cursor_ = new MaskedGridCursor(nx, ny, nz);
  grid_cursor_ = dynamic_cast<SGrid_cursor*>(mgrid_cursor_);

  original2reduced_.clear();
  original2reduced_.assign(nx*ny*nz,-1);
}


void Reduced_grid::set_dimensions( int nx, int ny, int nz, 
		      double xsize, double ysize, double zsize,
          double origin_x, double origin_y, double origin_z,
          double rotation_angle_z, 
          double rot_pivot_x, double rot_pivot_y, double rot_pivot_z )
{
  Cartesian_grid::set_dimensions( nx, ny, nz, xsize, ysize, zsize, origin_x, origin_y, origin_z,
                                  rotation_angle_z, rot_pivot_x, rot_pivot_y, rot_pivot_z);

  mgrid_cursor_ = new MaskedGridCursor(nx, ny, nz);
  grid_cursor_ = dynamic_cast<SGrid_cursor*>(mgrid_cursor_);

  original2reduced_.clear();
  original2reduced_.assign(nx*ny*nz,-1);
}

void Reduced_grid::mask( const std::vector<GsTLGridNode>& ijkCoords)
{

  build_mask_from_ijk(ijkCoords);
  mgrid_cursor_->set_mask(&original2reduced_, &reduced2original_, &mask_ );

  active_size_ = mgrid_cursor_->max_index();
	property_manager_.set_prop_size( mgrid_cursor_->max_index() );
  region_manager_.set_region_size( mgrid_cursor_->max_index() );
}

void Reduced_grid::mask( const std::vector<location_type>& xyzCoords)
{

  build_mask_from_xyz(xyzCoords);
  mgrid_cursor_->set_mask(&original2reduced_, &reduced2original_, &mask_ );

  active_size_ = mgrid_cursor_->max_index(); 
	property_manager_.set_prop_size( mgrid_cursor_->max_index() );
  region_manager_.set_region_size( mgrid_cursor_->max_index() );
}


void Reduced_grid::mask( const std::vector<bool>& grid_mask)
{

  mask_.clear();
  mask_.insert(mask_.begin(),grid_mask.begin(),grid_mask.end());
  //mask_ = grid_mask;

  build_ijkmap_from_mask();
	mgrid_cursor_->set_mask(&original2reduced_, &reduced2original_, &mask_ );
  
  active_size_ = mgrid_cursor_->max_index();
	property_manager_.set_prop_size( mgrid_cursor_->max_index() );
  region_manager_.set_region_size( mgrid_cursor_->max_index() );
}



void Reduced_grid::set_dimensions( int nx, int ny, int nz, 
		      double xsize, double ysize, double zsize,
          double origin_x, double origin_y, double origin_z,
          const std::vector<GsTLGridNode>& ijkCoords )
{
  
  Cartesian_grid::set_dimensions( nx, ny, nz, xsize, ysize, zsize, origin_x, origin_y, origin_z);

  mgrid_cursor_ = new MaskedGridCursor(nx, ny, nz);
  grid_cursor_ = dynamic_cast<SGrid_cursor*>(mgrid_cursor_);

  build_mask_from_ijk(ijkCoords);
	mgrid_cursor_->set_mask(&original2reduced_, &reduced2original_, &mask_ );

  active_size_ = mgrid_cursor_->max_index();
	property_manager_.set_prop_size( mgrid_cursor_->max_index() );
  region_manager_.set_region_size( mgrid_cursor_->max_index() );

}

void Reduced_grid::set_dimensions( int nx, int ny, int nz, 
		      double xsize, double ysize, double zsize,
          double origin_x, double origin_y, double origin_z,
          double rotation_angle_z, 
          double rot_pivot_x, double rot_pivot_y, double rot_pivot_z,
          const std::vector<GsTLGridNode>& ijkCoords )

{
  
  Cartesian_grid::set_dimensions( nx, ny, nz, xsize, ysize, zsize, origin_x, origin_y, origin_z,
                                  rotation_angle_z, rot_pivot_x, rot_pivot_y, rot_pivot_z);

  mgrid_cursor_ = new MaskedGridCursor(nx, ny, nz);
  grid_cursor_ = dynamic_cast<SGrid_cursor*>(mgrid_cursor_);

  build_mask_from_ijk(ijkCoords);
	mgrid_cursor_->set_mask(&original2reduced_, &reduced2original_, &mask_ );

  active_size_ = mgrid_cursor_->max_index();
	property_manager_.set_prop_size( mgrid_cursor_->max_index() );
  region_manager_.set_region_size( mgrid_cursor_->max_index() );
}

void Reduced_grid::set_dimensions( int nx, int ny, int nz, 
		      double xsize, double ysize, double zsize,
          double origin_x, double origin_y, double origin_z,
          const std::vector<location_type>& xyzCoords )
{
  Cartesian_grid::set_dimensions( nx, ny, nz, xsize, ysize, zsize, origin_x, origin_y, origin_z);

  mgrid_cursor_ = new MaskedGridCursor(nx, ny, nz);
  grid_cursor_ = dynamic_cast<SGrid_cursor*>(mgrid_cursor_);

  build_mask_from_xyz(xyzCoords);
	mgrid_cursor_->set_mask(&original2reduced_, &reduced2original_, &mask_ );

  active_size_ = mgrid_cursor_->max_index(); 
	property_manager_.set_prop_size( mgrid_cursor_->max_index() );
  region_manager_.set_region_size( mgrid_cursor_->max_index() );
}

void Reduced_grid::set_dimensions( int nx, int ny, int nz, 
		      double xsize, double ysize, double zsize,
          double origin_x, double origin_y, double origin_z,
          double rotation_angle_around_z, 
          double rot_pivot_x, double rot_pivot_y, double rot_pivot_z,
          const std::vector<location_type>& xyzCoords )
{
  Cartesian_grid::set_dimensions( nx, ny, nz, xsize, ysize, zsize, origin_x, origin_y, origin_z,
                                  rotation_angle_around_z, rot_pivot_x, rot_pivot_y, rot_pivot_z);

  mgrid_cursor_ = new MaskedGridCursor(nx, ny, nz);
  grid_cursor_ = dynamic_cast<SGrid_cursor*>(mgrid_cursor_);

  build_mask_from_xyz(xyzCoords);
	mgrid_cursor_->set_mask(&original2reduced_, &reduced2original_, &mask_ );

  active_size_ = mgrid_cursor_->max_index(); 
	property_manager_.set_prop_size( mgrid_cursor_->max_index() );
  region_manager_.set_region_size( mgrid_cursor_->max_index() );
}

void Reduced_grid::build_ijkmap_from_mask() {
	int i,j,k;
  int index = 0;
  int m_index = 0;

  original2reduced_.clear();
  original2reduced_.assign(mask_.size(),-1);

  std::vector<bool>::const_iterator it = mask_.begin();
  GsTLInt num_active_cells = 0;
  for( ; it!= mask_.end(); ++it) {
  	if(*it) num_active_cells++;
  }

  reduced2original_.clear();
  reduced2original_.reserve(num_active_cells);

  for(it = mask_.begin() ; it!= mask_.end(); ++it, ++index) {
    if(!(*it) ) continue;
    mgrid_cursor_->cartesian_coords(index,i,j,k);
//    reduced2original_[m_index] = index;
    reduced2original_.push_back(index);
    original2reduced_[index] = m_index;
    m_index++;

  }
}

void Reduced_grid::build_mask_from_ijk(
    const std::vector<GsTLGridNode>& ijkCoords)
{

  std::vector<GsTLGridNode>::const_iterator it = ijkCoords.begin();
  mask_.clear();
  mask_.insert(mask_.begin(),cgrid_geometry_->size(),false);
  for( ; it != ijkCoords.end() ; ++it ) {
    int index = mgrid_cursor_->cartesian_node_id(it->x(),it->y(),it->z());
    if(index >= 0) mask_[index] = true;
  }
  build_ijkmap_from_mask();
}

void Reduced_grid::build_mask_from_xyz(
    const std::vector<location_type>& xyzCoords)
{
  std::vector<location_type>::const_iterator it = xyzCoords.begin();
  mask_.clear();
  mask_.insert(mask_.begin(),cgrid_geometry_->size(),false);

  GsTLGridNode ijk;
  for( ; it != xyzCoords.end() ; ++it ) {
    cgrid_geometry_->grid_coordinates(ijk,*it);
    int index = mgrid_cursor_->cartesian_node_id(ijk[0],ijk[1],ijk[2]);
    if(index >= 0) mask_[index] = true;
  }
  build_ijkmap_from_mask();
}

bool Reduced_grid::add_location(int i, int j, int k)
{
  if(active_size_ >= this->size()) return false;
  const SGrid_cursor* cursor = RGrid::cursor();
  int index = cursor->node_id(i,j,k);
  if(index < 0) return false ;
  mask_[index] = true;
  active_size_++;
  return true;
}

bool Reduced_grid::add_location(int CartesianGridNodeId)
{
  if(active_size_ >= this->size()) return false;
  if(CartesianGridNodeId < 0) return false ;
  mask_[CartesianGridNodeId] = true;
  active_size_++;
  return true;
}

bool Reduced_grid::add_location(GsTLCoord x, GsTLCoord y, GsTLCoord z)
{
  if(active_size_ >= this->size()) return false;
  const SGrid_cursor* cursor = RGrid::cursor();
  int i = x/cgrid_geometry_->dim(0);
  int j = y/cgrid_geometry_->dim(1);
  int k = z/cgrid_geometry_->dim(2);
  int index = cursor->node_id(i,j,k);
  if(index < 0) return false; 
  mask_[index] = true;
  active_size_++;
  return true;
}


const std::vector<bool>& Reduced_grid::mask() const{
  return mask_;
}




GsTLInt Reduced_grid::closest_node( const location_type& P ) const {
/*
  int node_id = Cartesian_grid::closest_node(P);
  if(node_id < 0 || !is_inside_mask(node_id) ) {
    return -1;
  }
  else {
    return node_id;
  }
*/

  location_type origin = cgrid_geometry_->origin();
  location_type P0;
  P0.x() = P.x() - origin.x();
  P0.y() = P.y() - origin.y();
  P0.z() = P.z() - origin.z();

  GsTLCoordVector cell_sizes = cgrid_geometry_->cell_dims();
  int spacing = grid_cursor_->multigrid_spacing();

  // Account for the multi-grid spacing
  cell_sizes.x() = cell_sizes.x() * spacing;
  cell_sizes.y() = cell_sizes.y() * spacing;
  cell_sizes.z() = cell_sizes.z() * spacing;


  GsTLInt i = std::max( GsTL::floor( P0.x()/cell_sizes.x() + 0.5 ), 0 );
  GsTLInt j = std::max( GsTL::floor( P0.y()/cell_sizes.y() + 0.5 ), 0 );
  GsTLInt k = std::max( GsTL::floor( P0.z()/cell_sizes.z() + 0.5 ), 0 );

  // The function will return a valid node even if P is outside the
  // grid's bounding box
  if( i >= grid_cursor_->max_iter( SGrid_cursor::X ) )
    i = grid_cursor_->max_iter( SGrid_cursor::X ) - 1;
  if( j >= grid_cursor_->max_iter( SGrid_cursor::Y ) )
    j = grid_cursor_->max_iter( SGrid_cursor::Y ) - 1;
  if( k >= grid_cursor_->max_iter( SGrid_cursor::Z ) )
    k = grid_cursor_->max_iter( SGrid_cursor::Z ) - 1;

  // Need to check if the i,j,k is within the active_region of the grid

//  SGrid_cursor* cursor = dynamic_cast<SGrid_cursor*>(mgrid_cursor_);
//  int id = cursor->node_id(i,j,k);
//  return id;
  return mgrid_cursor_->node_id( i, j, k );
}
