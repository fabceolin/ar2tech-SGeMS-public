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


#include <grid/neighbors.h>


Geovalue 
Neighbors::center() const
{
  return center_;
}

void Neighbors::center_is(const Geovalue & _center)
{
  center_ = _center;
}


GsTLInt
Neighbors::number_informed_neighbors()
{
  if( neighbors_vector_changed_ )
  {
    number_informed_neighbors_ = 0;
    for (iterator it = neighbors_.begin(); it != neighbors_.end(); it++ ) {
      if ( it->is_informed() ) number_informed_neighbors_++;
    }
    neighbors_vector_changed_ = false;
  }
  return number_informed_neighbors_;
}

GsTLInt
Neighbors::number_harddata_neighbors()
{
  if( neighbors_vector_changed_ )
  {
    number_harddata_neighbors_ = 0;
    for (iterator it = neighbors_.begin(); it != neighbors_.end(); it++ ) {
      if ( it->is_harddata() ) number_harddata_neighbors_++;
    }
    neighbors_vector_changed_ = false;
  }
  return number_harddata_neighbors_;
}

Neighbors::Neighbors() : is_valid_(true)
{
  neighbors_vector_changed_ = true;
}

Neighbors::~Neighbors(){}
