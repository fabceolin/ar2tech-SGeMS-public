#include "grid_ordered_path.h"


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