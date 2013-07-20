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

#ifndef __GSTLAPPLI_GRID_MODEL_NEIGHBORS_H__ 
#define __GSTLAPPLI_GRID_MODEL_NEIGHBORS_H__ 
 
#include <grid/common.h>
#include <math/gstlvector.h> 
#include <grid/geovalue.h>  
#include <vector> 
 
 

/** Neighbors class contains neighborhood nodes returned by Neighborhood class. 
 */ 
class GRID_DECL Neighbors 
{ 
 public: 
  typedef std::vector<Geovalue>::iterator iterator; 
  typedef std::vector<Geovalue>::const_iterator const_iterator; 
   
 
 public:
   /* some inline APIs from std::vector<> */
   inline bool empty() const;
   inline void push_back (const Geovalue & node);
   inline void pop_back();
   template <class InputIterator>
   inline void insert(Neighbors::iterator position, InputIterator first, InputIterator last);
   inline Geovalue & operator[] (GsTLInt n);
   inline void clear();
   inline void reserve(GsTLInt n);
   inline Neighbors::iterator erase(Neighbors::iterator first, Neighbors::iterator last);
   inline void resize(GsTLInt n, Geovalue val = Geovalue());
   inline GsTLInt size() const;
   inline Geovalue & front();
   inline Geovalue & back();

   inline Neighbors::iterator begin();
   inline Neighbors::iterator end();


   /* Get the number of informed neighbors */
   GsTLInt number_informed_neighbors();

   /* Get the number of harddata neighbors */
   GsTLInt number_harddata_neighbors();


   Neighbors();
   ~Neighbors();

   
 protected: 
  std::vector<Geovalue> neighbors_; 
  GsTLInt number_informed_neighbors_; // number of informed neighbors
  GsTLInt number_harddata_neighbors_; // number of harddata neighbors
  bool neighbors_vector_changed_; // keep track of modifications on neighbors_ vector
}; 


/* inline methods */
bool 
Neighbors::empty() const
{
  return neighbors_.empty();
}

void 
Neighbors::push_back (const Geovalue & node)
{
  neighbors_.push_back(node);
  neighbors_vector_changed_ = true;
}

void 
Neighbors::pop_back()
{
  neighbors_.pop_back();
}

template <class InputIterator>
void 
Neighbors::insert(Neighbors::iterator position, InputIterator first, InputIterator last)
{
  neighbors_.insert(position, first, last);
}

Geovalue & 
Neighbors::operator[] (GsTLInt n)
{
  return neighbors_[n];
}

void 
Neighbors::clear()
{
  neighbors_.clear();
  neighbors_vector_changed_ = true;
}

void 
Neighbors::reserve(GsTLInt n)
{
  neighbors_.reserve(n);
}

Neighbors::iterator 
Neighbors::erase(Neighbors::iterator first, Neighbors::iterator last)
{
  return neighbors_.erase(first, last);
}

void 
Neighbors::resize(GsTLInt n, Geovalue val)
{
  neighbors_.resize(n, val);
}

GsTLInt
Neighbors::size() const
{
  return neighbors_.size();
}

Geovalue & 
Neighbors::front()
{
  return neighbors_[0];
}

Geovalue & 
Neighbors::back()
{
  return neighbors_[neighbors_.size() - 1];
}

Neighbors::iterator 
Neighbors::begin()
{
  return neighbors_.begin();
}

Neighbors::iterator 
Neighbors::end()
{
  return neighbors_.end();
}



#endif 
