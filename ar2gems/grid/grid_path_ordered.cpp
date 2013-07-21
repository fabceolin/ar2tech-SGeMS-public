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

#include <grid/grid_path_ordered.h>


Grid_ordered_path::iterator
Grid_ordered_path::begin() const
{
  return Grid_ordered_path::iterator( grid_, prop_, 
	                  		       0, grid_path_.size(),  
		                           LinearMapIndex() );
}

Grid_ordered_path::iterator
Grid_ordered_path::end() const
{
  return Grid_ordered_path::iterator( grid_, prop_, 
	                  		       grid_path_.size(), grid_path_.size(),  
		                           LinearMapIndex() );
}

GsTLInt 
Grid_ordered_path::size() const
{
  return grid_path_.size();
}

GsTLInt 
Grid_ordered_path::node_id( GsTLInt _path_index ) const
{
  return grid_->node_id( grid_path_[_path_index] );
}

Geovalue 
Grid_ordered_path::geovalue( GsTLInt _path_index ) const
{
  GsTLInt node_id = grid_->node_id( grid_path_[_path_index] );
  return Geovalue(grid_, prop_, node_id);
}

Grid_ordered_path::Grid_ordered_path(Geostat_grid * _grid, Grid_continuous_property * _prop)
{
  if ( !_grid || !_prop ) return;
  this->grid_ = _grid;
  this->prop_ = _prop;
  
  // init random path
  grid_path_.resize( grid_->size() );
  for( int i = 0; i < int( grid_path_.size() ); i++ ) 
    grid_path_[i] = i;
}

Grid_ordered_path::~Grid_ordered_path(void)
{
  this->grid_ = NULL;
  this->prop_ = NULL;
}