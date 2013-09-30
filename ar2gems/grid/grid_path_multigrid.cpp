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

#include <grid/grid_path_multigrid.h>
#include <math/random_numbers.h>


void 
Multi_grid_path::multigrid_level_is(GsTLInt _multigrid_level)
{
  multigrid_level_ = _multigrid_level;
  cursor_->set_multigrid_level(multigrid_level_);

  // init random path for this multigrid_level
  grid_path_.clear();
  grid_path_.resize( cursor_->max_index() );
  for( int i = 0; i < int( grid_path_.size() ); i++ ) 
    grid_path_[i] = i;

//  STL_generator gen;
//  std::random_shuffle( grid_path_.begin(), grid_path_.end(), gen );
}

Multi_grid_path::Multi_grid_path(RGrid * _rgrid, int level, Grid_region* _region) : Grid_path(  )
{
  this->grid_ = _rgrid;
  this->prop_ = 0;

  if ( !_rgrid  ) return;
  this->cursor_ = new SGrid_cursor( *_rgrid->cursor() );
  multigrid_level_ = level; // initially
  cursor_->set_multigrid_level(multigrid_level_);

  if(_region == 0) {
    grid_path_.resize( cursor_->max_index() );
    for( int i = 0; i < int( cursor_->max_index() ); i++ ) {
      grid_path_[i] = cursor_->node_id(i);
    }
  }
  else {
    grid_path_.reserve( cursor_->max_index() );
    for( int i = 0; i < int( cursor_->max_index() ); i++ ) {
      int node_id = cursor_->node_id(i);
      if(_region->is_inside_region(node_id)) grid_path_.push_back( node_id );
    }
  }

}

Multi_grid_path::Multi_grid_path(RGrid * _rgrid, int level, Grid_continuous_property * _prop, Grid_region* _region): Grid_path(  ){
  this->grid_ = _rgrid;
  this->prop_ = _prop;

  if ( !_rgrid && !_prop  ) return;
  this->cursor_ = new SGrid_cursor( *_rgrid->cursor() );
  multigrid_level_ = level; // initially
  cursor_->set_multigrid_level(multigrid_level_);

  if(_region == 0) {
    grid_path_.resize( cursor_->max_index() );
    for( int i = 0; i < int( cursor_->max_index() ); i++ ) {
      grid_path_[i] = cursor_->node_id(i);
    }
  }
  else {
    grid_path_.reserve( cursor_->max_index() );
    for( int i = 0; i < int( cursor_->max_index() ); i++ ) {
      int node_id = cursor_->node_id(i);
      if(_region->is_inside_region(node_id)) grid_path_.push_back( node_id );
    }
  }
}

Multi_grid_path::~Multi_grid_path(void)
{
  this->grid_ = NULL;
  this->prop_ = NULL;
  delete this->cursor_;
}

// --------------------


void 
Multi_grid_path_const::multigrid_level_is(GsTLInt _multigrid_level)
{
  multigrid_level_ = _multigrid_level;
  cursor_->set_multigrid_level(multigrid_level_);

  // init random path for this multigrid_level
  grid_path_.clear();
  grid_path_.resize( cursor_->max_index() );
  for( int i = 0; i < int( grid_path_.size() ); i++ ) 
    grid_path_[i] = i;

//  STL_generator gen;
//  std::random_shuffle( grid_path_.begin(), grid_path_.end(), gen );
}

Multi_grid_path_const::Multi_grid_path_const(const RGrid * _rgrid, int level, const Grid_region* _region) : Grid_path_const(  )
{
  this->grid_ = _rgrid;
  this->prop_ = 0;

  if ( !_rgrid  ) return;
  this->cursor_ = new SGrid_cursor( *_rgrid->cursor() );
  multigrid_level_ = level; // initially
  cursor_->set_multigrid_level(multigrid_level_);

  if(_region == 0) {
    grid_path_.resize( cursor_->max_index() );
    for( int i = 0; i < int( cursor_->max_index() ); i++ ) {
      grid_path_[i] = cursor_->node_id(i);
    }
  }
  else {
    grid_path_.reserve( cursor_->max_index() );
    for( int i = 0; i < int( cursor_->max_index() ); i++ ) {
      int node_id = cursor_->node_id(i);
      if(_region->is_inside_region(node_id)) grid_path_.push_back( node_id );
    }
  }

}

Multi_grid_path_const::Multi_grid_path_const(const RGrid * _rgrid, int level, const Grid_continuous_property * _prop, const Grid_region* _region): Grid_path_const(  ){
  this->grid_ = _rgrid;
  this->prop_ = _prop;

  if ( !_rgrid && !_prop  ) return;
  this->cursor_ = new SGrid_cursor( *_rgrid->cursor() );
  multigrid_level_ = level; // initially
  cursor_->set_multigrid_level(multigrid_level_);

  if(_region == 0) {
    grid_path_.resize( cursor_->max_index() );
    for( int i = 0; i < int( cursor_->max_index() ); i++ ) {
      grid_path_[i] = cursor_->node_id(i);
    }
  }
  else {
    grid_path_.reserve( cursor_->max_index() );
    for( int i = 0; i < int( cursor_->max_index() ); i++ ) {
      int node_id = cursor_->node_id(i);
      if(_region->is_inside_region(node_id)) grid_path_.push_back( node_id );
    }
  }
}

Multi_grid_path_const::~Multi_grid_path_const(void)
{
  this->grid_ = NULL;
  this->prop_ = NULL;
  delete this->cursor_;
}
