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

#include <grid/rgrid_neighborhood.h>
#include <grid/rgrid.h>
#include <math/random_numbers.h>

#include <GsTL/math/math_functions.h>

#include <QIcon>

Named_interface* create_Rgrid( std::string& ) {
  return new RGrid;
}

//=======================================================



RGrid::RGrid( )
  :  Geostat_grid(),
     geom_(0),
     accessor_(0),
     connection_is_updated_(false),
     grid_cursor_(0){
	 region_manager_.set_parent_item(this);
	 property_manager_.set_parent_item(this);
}

RGrid::RGrid( std::string name )
:  Geostat_grid(name),
   geom_(0),
   cell_connection_(0),
   connection_is_updated_(false),
   accessor_(0),
   grid_cursor_(0){
	 region_manager_.set_parent_item(this);
	 property_manager_.set_parent_item(this);
	 group_manager_.set_parent_item(this);

}


RGrid::~RGrid() {
    delete geom_;
    //delete cell_connection_;
    delete accessor_;
    delete grid_cursor_;
}


void RGrid::set_geometry(RGrid_geometry* geom) {
  if( geom_ != geom ) {
    delete geom_;
    geom_ = geom->clone();
    connection_is_updated_ = false;
  }
  grid_cursor_ = new SGrid_cursor(nx(), ny(), nz(), 1);
  property_manager_.set_prop_size( geom->size() );
  region_manager_.set_region_size( geom->size() );
}


void RGrid::set_accessor(RGrid_gval_accessor* accessor) {
  if(accessor_ != accessor) {
    delete accessor_;
    accessor_ = accessor;
  }
}

Neighborhood* RGrid::neighborhood( double x, double y, double z,
				   double ang1, double ang2, double ang3,
				   const Covariance<location_type>* cov,
				   bool only_harddata, const Grid_region* region,
           Coordinate_mapper* coord_mapper) {

  // The constructor of Rgrid_ellips_neighborhood expects the dimensions
  // of the search ellipsoid to be in "number of cells", and the covariance
  // ranges to be expressed in term of "number of cells".

  int nx = GsTL::round( x /geom_->cell_dims()[0] );
  int ny = GsTL::round( y /geom_->cell_dims()[1] );
  int nz = GsTL::round( z /geom_->cell_dims()[2] );

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
    return new Rgrid_ellips_neighborhood_hd( this, 
					     property_manager_.selected_property(),
					     nx,ny,nz, ang1,ang2,ang3,
					     nx*ny*nz, cov_copy, region );
  else
    return new Rgrid_ellips_neighborhood( this, 
					  property_manager_.selected_property(),
					  nx,ny,nz, ang1,ang2,ang3,
					  nx*ny*nz, cov_copy, region  );

  delete cov_copy;
}



Neighborhood* RGrid::neighborhood( const GsTLTripletTmpl<double>& dim,
				   const GsTLTripletTmpl<double>& angles,
				   const Covariance<location_type>* cov,
				   bool only_harddata, const Grid_region* region,
           Coordinate_mapper* coord_mapper) {
  int nx = GsTL::round( dim[0] /geom_->cell_dims()[0] );
  int ny = GsTL::round( dim[1] /geom_->cell_dims()[1] );
  int nz = GsTL::round( dim[2] /geom_->cell_dims()[2] );

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
    return new Rgrid_ellips_neighborhood_hd( this, 
					     property_manager_.selected_property(),
					     nx,ny,nz,
					     angles[0], angles[1], angles[2],
					     nx*ny*nz, cov_copy, region );
  else
    return new Rgrid_ellips_neighborhood( this, 
					  property_manager_.selected_property(),
					  nx,ny,nz,
					  angles[0], angles[1], angles[2],
					  nx*ny*nz, cov_copy, region );
}

Window_neighborhood* RGrid::window_neighborhood( const Grid_template& templ) {
  return new Rgrid_window_neighborhood( templ, this,
					property_manager_.selected_property() );
}
 


QString RGrid::item_type() const{
	return QString(this->classname().c_str());
}

GsTL_object_item *RGrid::child(int row){
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


const GsTL_object_item *RGrid::child(int row) const{
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


int RGrid::childCount() const{
	return group_manager_.size()+2;

}

int RGrid::columnCount() const{
	return 2;

}

QVariant RGrid::item_data(int column) const{
	if(column ==0)
		return QString(name_.c_str());
	if(column ==1)
		return this->item_type();

	return QVariant();

}


/*
QModelIndex	RGrid::mapToSource ( const QModelIndex & proxyIndex ) const{
	return proxyIndex;
}
QModelIndex	RGrid::mapFromSource ( const QModelIndex & sourceIndex ) const{
	return sourceIndex;
}
*/
