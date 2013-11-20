/* -----------------------------------------------------------------------------
** Copyright (c) 2013 Advanced Resources and Risk Technology, LLC
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

#include <grid/grid_path.h>
#include <math/random_numbers.h>

Grid_path::iterator
Grid_path::begin()
{
  return Grid_path::iterator( grid_, prop_, 
	                  		       0, grid_path_.size(),  
		                           TabularMapIndex(&grid_path_) );
}

Grid_path::const_iterator
Grid_path::begin() const
{
  return Grid_path::const_iterator( grid_, prop_, 
	                  		       0, grid_path_.size(),  
		                           TabularMapIndex(&grid_path_) );
}

Grid_path::iterator
Grid_path::end()
{
  return Grid_path::iterator( grid_, prop_, 
	                  		      grid_path_.size(), grid_path_.size(),  
		                          TabularMapIndex(&grid_path_) );
}

Grid_path::const_iterator
Grid_path::end() const
{
  return Grid_path::const_iterator( grid_, prop_, 
	                  		      grid_path_.size(), grid_path_.size(),  
		                          TabularMapIndex(&grid_path_) );
}

GsTLInt 
Grid_path::size() const
{
  return grid_path_.size();
}

GsTLInt 
Grid_path::node_id( GsTLInt _path_index ) const
{
  return grid_->node_id( grid_path_[_path_index] );
}

Geovalue 
Grid_path::geovalue( GsTLInt _path_index ) const
{
  GsTLInt node_id = grid_->node_id( grid_path_[_path_index] );
  return Geovalue(grid_, prop_, node_id);
}

Grid_path::Grid_path() : grid_(0),prop_(0)  {}

Grid_path::Grid_path(Geostat_grid * _grid, Grid_continuous_property * _prop, Grid_region * _region)
{
  //if ( !_grid || !_prop ) return;
  this->grid_ = _grid;
  this->prop_ = _prop;
  
  // init random path
  if(_region == 0) {
    grid_path_.resize( grid_->size() );
    for( int i = 0; i < grid_->size(); i++ ) {
      grid_path_[i] = i;
    }
  }
  else {
    grid_path_.reserve( grid_->size() );
    for( int i = 0; i < grid_->size(); i++ ) {
      if(_region->is_inside_region(i)) grid_path_.push_back( i );
    }
  }
}

/*
Grid_path::Grid_path(Geostat_grid * _grid,  Grid_region * _region)
{
  if ( !_grid  ) return;
  this->grid_ = _grid;
  this->prop_ = 0;
  
  // init random path
  if(_region == 0) {
    grid_path_.resize( grid_->size() );
    for( int i = 0; i < grid_->size() ; i++ ) {
      grid_path_[i] = i;
    }
  }
  else {
    grid_path_.reserve( grid_->size() );
    for( int i = 0; i <  grid_->size() ; i++ ) {
      if(_region->is_inside_region(i)) grid_path_.push_back( i );
    }
  }
}
*/
Grid_path::~Grid_path(void)
{
  this->grid_ = NULL;
  this->prop_ = NULL;
}

bool Grid_path::set_property(std::string prop_name){
  this->prop_ = grid_->property(prop_name);
  return prop_ != 0;
}

void Grid_path::randomize(){
  STL_generator gen;
  std::random_shuffle( grid_path_.begin(), grid_path_.end(), gen );
}


// --------------------------

Grid_path_const::const_iterator
Grid_path_const::begin() const
{
  return Grid_path::const_iterator( grid_, prop_, 
	                  		       0, grid_path_.size(),  
		                           TabularMapIndex(&grid_path_) );
}


Grid_path_const::const_iterator
Grid_path_const::end() const
{
  return Grid_path::const_iterator( grid_, prop_, 
	                  		      grid_path_.size(), grid_path_.size(),  
		                          TabularMapIndex(&grid_path_) );
}

GsTLInt 
Grid_path_const::size() const
{
  return grid_path_.size();
}

GsTLInt 
Grid_path_const::node_id( GsTLInt _path_index ) const
{
  return grid_->node_id( grid_path_[_path_index] );
}

Const_geovalue 
Grid_path_const::geovalue( GsTLInt _path_index ) const
{
  GsTLInt node_id = grid_->node_id( grid_path_[_path_index] );
  return Const_geovalue(grid_, prop_, node_id);
}

Grid_path_const::Grid_path_const() : grid_(0),prop_(0)  {}

Grid_path_const::Grid_path_const(const Geostat_grid * _grid, const Grid_continuous_property * _prop, const Grid_region * _region)
{
  if ( !_grid || !_prop ) return;
  this->grid_ = _grid;
  this->prop_ = _prop;
  
  // init random path
  if(_region == 0) {
    grid_path_.resize( grid_->size() );
    for( int i = 0; i <  grid_->size(); i++ ) {
      grid_path_[i] = i;
    }
  }
  else {
    grid_path_.reserve( grid_->size() );
    for( int i = 0; i <  grid_->size(); i++ ) {
      if(_region->is_inside_region(i)) grid_path_.push_back( i );
    }
  }
}

/*
Grid_path_const::Grid_path_const(const Geostat_grid * _grid, const Grid_region * _region)
{
  if ( !_grid  ) return;
  this->grid_ = _grid;
  this->prop_ = 0;
  
  // init random path
  if(_region == 0) {
    grid_path_.resize( grid_->size() );
    for( int i = 0; i < grid_->size(); i++ ) {
      grid_path_[i] = i;
    }
  }
  else {
    grid_path_.reserve( grid_->size() );
    for( int i = 0; i <i <  grid_->size(); i++ ) {
      if(_region->is_inside_region(i)) grid_path_.push_back( i );
    }
  }
}
*/
Grid_path_const::~Grid_path_const(void)
{
  this->grid_ = NULL;
  this->prop_ = NULL;
}

bool Grid_path_const::set_property(std::string prop_name){
  this->prop_ = grid_->property(prop_name);
  return prop_ != 0;
}

void Grid_path_const::randomize(){
  STL_generator gen;
  std::random_shuffle( grid_path_.begin(), grid_path_.end(), gen );
}


// --------------------------

Grid_path_ordered::iterator
Grid_path_ordered::begin()
{
  return Grid_path_ordered::iterator( grid_, prop_, 
	                  		       0, grid_->size(),  
                               LinearMapIndex() );
}

Grid_path_ordered::const_iterator
Grid_path_ordered::begin() const
{
  return Grid_path_ordered::const_iterator( grid_, prop_, 
	                  		       0, grid_->size(),  
                               LinearMapIndex() );
}

Grid_path_ordered::iterator
Grid_path_ordered::end()
{
  return Grid_path_ordered::iterator( grid_, prop_, 
	                  		      grid_->size(), grid_->size(),  
		                          LinearMapIndex() );
}

Grid_path_ordered::const_iterator
Grid_path_ordered::end() const
{
  return Grid_path_ordered::const_iterator( grid_, prop_, 
	                  		      grid_->size(), grid_->size(),  
		                          LinearMapIndex() );
}

GsTLInt 
Grid_path_ordered::size() const
{
  return grid_->size();
}

GsTLInt 
Grid_path_ordered::node_id( GsTLInt _path_index ) const
{
  return _path_index;
}

Geovalue 
Grid_path_ordered::geovalue( GsTLInt _path_index ) const
{
  return Geovalue(grid_, prop_, _path_index);
}

Grid_path_ordered::Grid_path_ordered() : grid_(0),prop_(0)  {}

Grid_path_ordered::Grid_path_ordered(Geostat_grid * _grid, Grid_continuous_property * _prop)
{
  //if ( !_grid || !_prop ) return;
  this->grid_ = _grid;
  this->prop_ = _prop;
}


Grid_path_ordered::Grid_path_ordered(Geostat_grid * _grid)
{
  //if ( !_grid  ) return;
  this->grid_ = _grid;
  this->prop_ = 0;
}

Grid_path_ordered::~Grid_path_ordered(void)
{
  this->grid_ = NULL;
  this->prop_ = NULL;
}

bool Grid_path_ordered::set_property(std::string prop_name){
  this->prop_ = grid_->property(prop_name);
  return prop_ != 0;
}


// --------------------------


Grid_path_ordered_const::const_iterator
Grid_path_ordered_const::begin() const
{
  return Grid_path_ordered_const::const_iterator( grid_, prop_, 
	                  		       0, grid_->size(),  
                               LinearMapIndex() );
}


Grid_path_ordered_const::const_iterator
Grid_path_ordered_const::end() const
{
  return Grid_path_ordered::const_iterator( grid_, prop_, 
	                  		      grid_->size(), grid_->size(),  
		                          LinearMapIndex() );
}

GsTLInt 
Grid_path_ordered_const::size() const
{
  return grid_->size();
}

GsTLInt 
Grid_path_ordered_const::node_id( GsTLInt _path_index ) const
{
  return _path_index;
}

Const_geovalue 
Grid_path_ordered_const::geovalue( GsTLInt _path_index ) const
{
  return Const_geovalue(grid_, prop_, _path_index);
}

Grid_path_ordered_const::Grid_path_ordered_const() : grid_(0),prop_(0)  {}

Grid_path_ordered_const::Grid_path_ordered_const(const Geostat_grid * _grid, const Grid_continuous_property * _prop)
{
  //if ( !_grid || !_prop ) return;
  this->grid_ = _grid;
  this->prop_ = _prop;
}


Grid_path_ordered_const::Grid_path_ordered_const(const Geostat_grid * _grid)
{
  //if ( !_grid  ) return;
  this->grid_ = _grid;
  this->prop_ = 0;
}

Grid_path_ordered_const::~Grid_path_ordered_const(void)
{
  this->grid_ = NULL;
  this->prop_ = NULL;
}

bool Grid_path_ordered_const::set_property(std::string prop_name){
  this->prop_ = grid_->property(prop_name);
  return prop_ != 0;
}
