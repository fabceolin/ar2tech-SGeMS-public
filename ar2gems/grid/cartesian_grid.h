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

#ifndef __GSTLAPPLI_CARTESIAN_GRID_H__ 
#define __GSTLAPPLI_CARTESIAN_GRID_H__ 
 
 
#include <grid/common.h>
#include <grid/rgrid.h> 
#include <grid/cartesian_grid_geometry.h>
#include <math/box.h> 
 
 
 
class GRID_DECL Cartesian_grid : public RGrid { 
 
 public: 
   
  Cartesian_grid()  
    : RGrid() { 
  }  

  Cartesian_grid(std::string name)
    : RGrid(name) {
  }
/*
  Cartesian_grid(std::string name, int nx, int ny, int nz) ;

  Cartesian_grid(std::string name, int nx, int ny, int nz,
                            double xsize, double ysize, double zsize);
*/   
  virtual void set_geometry( RGrid_geometry* geom ); 
   
  void set_dimensions( int nx, int ny, int nz );

  void set_dimensions( int nx, int ny, int nz, 
		       double xsize, double ysize, double zsize,
           double origin_x, double origin_y, double origin_z); 

  void set_dimensions( int nx, int ny, int nz, 
		       double xsize, double ysize, double zsize,
           double origin_x, double origin_y, double origin_z,
           double rotation_angle_z, 
           double rot_pivot_x, double rot_pivot_y, double rot_pivot_z); 

  GsTLCoordVector cell_dimensions() const { return cgrid_geometry_->cell_dims(); } 
 

  virtual GsTLPoint origin() const { return cgrid_geometry_->origin(); } 
   
  virtual std::string type() const { return "Cgrid"; }
  virtual std::string classname() const { return this->type(); } 
  
  virtual GsTLInt closest_node( const location_type& P ) const; 
 
  GsTL_cube bounding_box() const; 

  virtual double get_support(int nodeid) const {return cgrid_geometry_->get_volume();}
 
 
 protected: 
   Cartesian_grid_geometry* cgrid_geometry_; 
}; 
 
 
 
//================================ 
// creation function 
Named_interface* create_Cgrid( std::string& ); 
 
 
 
#endif 
