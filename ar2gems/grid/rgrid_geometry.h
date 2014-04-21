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

#ifndef __GSTLAPPLI_GRID_RGRIDGEOMETRY_H__ 
#define __GSTLAPPLI_GRID_RGRIDGEOMETRY_H__ 
 
#include <grid/common.h>
#include <math/gstlpoint.h> 
 
 
/*****************************************************************/ 
class GRID_DECL RGrid_geometry { 
public: 

    RGrid_geometry(int nx, int ny, int nz ) : ncells_(nx,ny,nz) {}

    virtual ~RGrid_geometry(){}
     
    virtual RGrid_geometry* clone() const = 0; 
 
    GsTLInt dim( GsTLInt i ) const { return ncells_[i];} 
    GsTLInt ni() const { return ncells_.x();} 
    GsTLInt nj() const { return ncells_.y();} 
    GsTLInt nk() const { return ncells_.z();} 

    GsTLInt size() const { return ( ncells_[0]*ncells_[1]*ncells_[2] ); } 

 
    /** Returns the location of node (i,j,k). For an Rgrid, the location 
     * is expressed in stratigraphic coordinates (x and y are the real coordinates 
     * but z is in [0,1[ if the point is in layer 0, [1,2[ if in layer 1, etc) 
     * Actually, z can b multiplied by a constant layer thickness and shifted by 
     * a constant z0. 
     *  
     * This point should be clarified... 
     */ 
    virtual GsTLPoint coordinates(GsTLInt i, GsTLInt j, GsTLInt k) const = 0;

    /** find the grid coordinates of point \c p. The grid coordinates are 
    * stored into \c ijk. \c grid_coordinates return a flag indicating 
    * whether the operation suceeded. If the flag is flase, \c ijk was not 
    * changed.
    * The i,j,k coordinates are coordinates in grid itself (no coarse grid
    * is taken into account)
    */
    virtual bool 
      grid_coordinates( GsTLGridNode& ijk, const GsTLPoint& p ) const = 0;
 
   virtual double rotation_z() const =0;
   virtual GsTLPoint rotation_point() const =0;


    /** The dimensions of a grid cell (in stratigraphic coordinates) 
     */ 
    virtual const GsTLCoordVector& cell_dims() const=0;

protected :
  GsTLVector<int> ncells_;

}; 
 
#endif 
 
