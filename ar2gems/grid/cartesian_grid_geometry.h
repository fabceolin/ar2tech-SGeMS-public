/* -----------------------------------------------------------------------------
** Copyright (c) 2014 Advanced Resources and Risk Technology, LLC
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


#ifndef __CARTESIAN_GRID_GEOMETRY_H__ 
#define __CARTESIAN_GRID_GEOMETRY_H__ 
 
#include <grid/common.h>
#include <math/gstlpoint.h> 
#include <grid/rgrid_geometry.h>
 
 
class GRID_DECL Cartesian_grid_geometry : public RGrid_geometry { 
public: 
    Cartesian_grid_geometry( int nx, int ny, int nz, 
                        double dimx=1, double dimy=1, double dimz=1, 
                        double ox=0, double oy=0, double oz=0); 
    Cartesian_grid_geometry( int nx, int ny, int nz, 
                           double dimx, double dimy, double dimz, 
                           double ox, double oy, double oz,
                           double rotation_angle_z, 
                           double rot_pivot_x, double rot_pivot_y, double rot_pivot_z); 
    ~Cartesian_grid_geometry(); 
 
    virtual RGrid_geometry* clone() const; 
 
 
   const GsTLPoint& origin() const { 
        return origin_; 
    } 

    virtual void set_cell_dims( const GsTLCoordVector& v ) { 
        cell_dims_ = v; 
        volume_ = cell_dims_[0]*cell_dims_[1]*cell_dims_[2];
    } 

    double get_volume() const {return volume_;}

    /** The zrotation defined the azimuth of the j axis
     */ 
   virtual double rotation_z() const { 
        return z_rot_; 
    } 
   virtual GsTLPoint rotation_point() const {return rotation_point_;}


    /** Find the actual coordinates of a grid node given its (i,j,k)  
     * coordinates. 
     */ 
    virtual GsTLPoint coordinates(GsTLInt i, GsTLInt j, GsTLInt k) const;
 
    virtual bool grid_coordinates( GsTLGridNode& ijk, const GsTLPoint& p ) const;

    /** The dimensions of a grid cell (in stratigraphic coordinates) 
     */ 
    virtual const GsTLCoordVector& cell_dims() const  { 
        return cell_dims_; 
    }
    


protected :
  void set_rotation_z(double z_angle, const GsTLPoint& rotation_point);
 
protected:

    GsTLCoordVector cell_dims_;

    GsTLPoint origin_;
    GsTLPoint rotation_point_;
    GsTLPoint::difference_type delta_origin_rotation_point_;
    GsTLPoint::difference_type rotated_delta_origin_rotation_point_;
    double z_rot_;
    double z_cos_angle_;
    double z_sin_angle_;

    double volume_;

}; 
 
 
#endif 
 
