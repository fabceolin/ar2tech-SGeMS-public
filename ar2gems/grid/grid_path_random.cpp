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

#include <grid/grid_path_random.h>
#include <math/random_numbers.h>


Grid_random_path::iterator
Grid_random_path::begin() const
{
  return Grid_random_path::iterator( grid_, prop_, 
	                  		       0, grid_path_.size(),  
		                           TabularMapIndex(&grid_path_) );
}

Grid_random_path::iterator
Grid_random_path::end() const
{
  return Grid_random_path::iterator( grid_, prop_, 
	                  		       grid_path_.size(), grid_path_.size(),  
		                           TabularMapIndex(&grid_path_) );
}

GsTLInt 
Grid_random_path::size() const
{
  return grid_path_.size();
}

GsTLInt 
Grid_random_path::node_id( GsTLInt _path_index ) const
{
  return grid_->node_id( grid_path_[_path_index] );
}

Geovalue 
Grid_random_path::geovalue( GsTLInt _path_index ) const
{
  GsTLInt node_id = grid_->node_id( grid_path_[_path_index] );
  return Geovalue(grid_, prop_, node_id);
}

Grid_random_path::Grid_random_path(Geostat_grid * _grid, Grid_continuous_property * _prop, Grid_region* _region)
{
  if ( !_grid || !_prop ) return;
  this->grid_ = _grid;
  this->prop_ = _prop;
  
  // init random path
  if(_region == 0) {
    grid_path_.resize( grid_->size() );
    for( int i = 0; i < int( grid_path_.size() ); i++ ) {
      grid_path_[i] = i;
    }
  }
  else {
    grid_path_.reserve( grid_->size() );
    for( int i = 0; i < int( grid_path_.size() ); i++ ) {
      if(_region->is_inside_region(i)) grid_path_.push_back( i );
    }
  }

  STL_generator gen;
  std::random_shuffle( grid_path_.begin(), grid_path_.end(), gen );
}


Grid_random_path::Grid_random_path(Geostat_grid * _grid, Grid_region* _region){
  if ( !_grid  ) return;
  this->grid_ = _grid;
  this->prop_ = 0;
  
  // init random path
  if(_region == 0) {
    grid_path_.resize( grid_->size() );
    for( int i = 0; i < int( grid_path_.size() ); i++ ) {
      grid_path_[i] = i;
    }
  }
  else {
    grid_path_.reserve( grid_->size() );
    for( int i = 0; i < int( grid_path_.size() ); i++ ) {
      if(_region->is_inside_region(i)) grid_path_.push_back( i );
    }
  }

  STL_generator gen;
  std::random_shuffle( grid_path_.begin(), grid_path_.end(), gen );

}


Grid_random_path::~Grid_random_path(void)
{
  this->grid_ = NULL;
  this->prop_ = NULL;
}

bool Grid_random_path::set_property(std::string prop_name){
  this->prop_ = grid_->property(prop_name);
  return prop_ != 0;
}