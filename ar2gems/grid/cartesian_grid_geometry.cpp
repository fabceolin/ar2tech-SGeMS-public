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


#include <grid/cartesian_grid_geometry.h>

#include <GsTL/math/math_functions.h>



Cartesian_grid_geometry::Cartesian_grid_geometry( int nx, int ny, int nz, 
                        double dimx, double dimy, double dimz, 
                        double ox, double oy, double oz ) 
   : RGrid_geometry(nx,ny,nz),  cell_dims_(dimx, dimy, dimz), origin_( ox, oy, oz ), volume_(dimx*dimy*dimz)
{
  GsTLPoint rotation_point(ox, oy, oz);
  this->set_rotation_z(0.0, rotation_point);

}


Cartesian_grid_geometry::Cartesian_grid_geometry( int nx, int ny, int nz, 
                        double dimx, double dimy, double dimz, 
                        double ox, double oy, double oz,
                        double rotation_angle_z, 
                        double rot_pivot_x, double rot_pivot_y, double rot_pivot_z)  
                        : RGrid_geometry(nx,ny,nz),  cell_dims_(dimx, dimy, dimz), origin_( ox, oy, oz ), volume_(dimx*dimy*dimz)
{
  GsTLPoint rotation_point(rot_pivot_x, rot_pivot_y, rot_pivot_z);
  this->set_rotation_z(rotation_angle_z, rotation_point);
}


Cartesian_grid_geometry::~Cartesian_grid_geometry() {
}

RGrid_geometry* Cartesian_grid_geometry::clone() const {
  Cartesian_grid_geometry* geom = new Cartesian_grid_geometry(ncells_[0], ncells_[1], ncells_[2],
                                                          cell_dims_.x(),cell_dims_.y(),cell_dims_.y(),
                                                          origin_.x(), origin_.y(), origin_.z(),
                                                          z_rot_, rotation_point_.x(), rotation_point_.y(), rotation_point_.z() );

  return geom;
}




GsTLPoint Cartesian_grid_geometry::coordinates(GsTLInt i, GsTLInt j, GsTLInt k) const {
  
  GsTLCoord x,y,z;
  if(z_rot_ == 0) {
    x = origin_.x() + GsTLCoord(i)*cell_dims_.x(); 
    y = origin_.y() + GsTLCoord(j)*cell_dims_.y(); 
    z = origin_.z() + GsTLCoord(k)*cell_dims_.z();
  }
  else {
    GsTLCoord dx = GsTLCoord(i)*cell_dims_.x() + rotated_delta_origin_rotation_point_.x(); 
    GsTLCoord dy = GsTLCoord(j)*cell_dims_.y() + rotated_delta_origin_rotation_point_.y();
  
    x = dx*z_cos_angle_ + dy*z_sin_angle_ + rotation_point_.x();  // - delta + origin
    y = -dx*z_sin_angle_ + dy*z_cos_angle_ + rotation_point_.y(); 
    z = origin_.z() + GsTLCoord(k)*cell_dims_.z() ; 
  }


  return GsTLPoint(x, y, z);


  return GsTLPoint(x, y, z);
}


bool Cartesian_grid_geometry::
grid_coordinates( GsTLGridNode& ijk, const GsTLPoint& p ) const {

  if(z_rot_ != 0) {
    GsTLPoint::difference_type translated = p - rotation_point_;

    // back-rotation of the point
    GsTLCoord rot_dx  = translated.x()*z_cos_angle_ - translated.y()*z_sin_angle_;
    GsTLCoord rot_dy  = translated.x()*z_sin_angle_ + translated.y()*z_cos_angle_;

    rot_dx -= rotated_delta_origin_rotation_point_.x();    // +rotated_point + ()
    rot_dy -= rotated_delta_origin_rotation_point_.y(); 
    //double rot_dz = translated.z(); 
    double rot_dz = p.z() - origin_.z(); 
    ijk[0] = GsTL::round( rot_dx / float( cell_dims_.x() ) );
    ijk[1] = GsTL::round( rot_dy / float( cell_dims_.y() ) );
    ijk[2] = GsTL::round( rot_dz / float( cell_dims_.z() ) );

  }
  else {
    GsTLPoint::difference_type translated = p - origin_;
    ijk[0] = GsTL::floor( translated.x() / float( cell_dims_.x() ) );
    ijk[1] = GsTL::floor( translated.y() / float( cell_dims_.y() ) );
    ijk[2] = GsTL::floor( translated.z() / float( cell_dims_.z() ) );
  }

  return true;
}
 
void Cartesian_grid_geometry::set_rotation_z(double z_angle, const GsTLPoint& rotation_point) {
  z_rot_ = z_angle; 
  double z_rad = GsTL::PI/180.0*z_rot_; 
  z_cos_angle_ = std::cos(z_rad);
  z_sin_angle_ = std::sin(z_rad);
  rotation_point_ = rotation_point;
  delta_origin_rotation_point_ = origin_ - rotation_point_;

  double rot_dx  = delta_origin_rotation_point_.x()*z_cos_angle_ - delta_origin_rotation_point_.y()*z_sin_angle_;
  double rot_dy  = delta_origin_rotation_point_.x()*z_sin_angle_ + delta_origin_rotation_point_.y()*z_cos_angle_;

  rotated_delta_origin_rotation_point_ = GsTLPoint::difference_type( rot_dx, rot_dy, delta_origin_rotation_point_.z() );

}