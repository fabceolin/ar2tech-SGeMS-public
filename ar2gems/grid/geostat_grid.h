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

#ifndef __GSTLAPPLI_GRID_MODEL_GEOSTAT_GRID_H__ 
#define __GSTLAPPLI_GRID_MODEL_GEOSTAT_GRID_H__ 
 


#include <grid/common.h>
//#include <grid/gval_iterator.h>
#include <utils/named_interface.h> 
#include <math/gstlpoint.h> 
#include <grid/coordinate_mapper.h>
#include <grid/grid_property.h>
#include <grid/grid_categorical_property.h>
#include <grid/grid_weight_property.h>
#include <grid/grid_region.h>
#include <grid/grid_property_set.h>
#include <grid/grid_property_manager.h>
#include <grid/grid_region_manager.h>


#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <QMimeData>
#include <QAbstractProxyModel>
#include <QStringList>


#include <typeinfo> 
#include <list> 
#include <string>

//class Grid_continuous_property;
//class Grid_categorical_property;
//class Grid_weight_property;
//class Grid_region;
//class Grid_property_group;
class MultiRealization_property; 
class CategoricalPropertyDefinition;
template<class R> class Gval_iterator; 
template<class R> class Gval_const_iterator; 
class LinearMapIndex; 
class TabularMapIndex; 
class Neighborhood; 
class Geovalue;
template<class Location_> class Covariance; 


 
/** Geostat_grid is the root of the grid-models hierarchy. It is the 
 * base object on which any geostatistics algorithm can be applied. 
 *  
 * Every node of a geostat_grid is assigned a unique id (node_id). 
 * It is possible to re-order the nodes by re-defining function node_id(...). 
 */ 
 
class GRID_DECL Geostat_grid : public Named_interface {


 public: 
 
  typedef float property_type;  
  typedef GsTLPoint location_type; 
 
  typedef Gval_iterator<LinearMapIndex> iterator; 
  typedef Gval_const_iterator<LinearMapIndex> const_iterator; 
  typedef Gval_iterator<TabularMapIndex> random_path_iterator; 
 
  
 public: 
 
  Geostat_grid(): coord_mapper_(0){}
  Geostat_grid(std::string name);
  virtual ~Geostat_grid() {} 

  // Returns the most specific name of the current class 
  virtual std::string classname() const { return "Geostat_grid"; }

  virtual std::string name() const { return name_; }
  virtual void name(std::string name){ name_ = name; }


  /** Get the topology of the grid
  * Allows conversion between geological and real-world coordinates
  */
  virtual Coordinate_mapper* coordinate_mapper() {return coord_mapper_;}
  virtual const Coordinate_mapper* coordinate_mapper() const {return coord_mapper_;}
  
  virtual void set_coordinate_mapper(Coordinate_mapper* coord_mapper) {coord_mapper_ = coord_mapper;}

  /** get the support (length or volume) of a cell at location nodeid
  */
  virtual double get_support(int nodeid) const {return 1;}

  virtual Geovalue geovalue( GsTLInt node_id )=0;

 
  //------------------------------------ 
  // Properties management 
 
  /** Adds a new property called \a name. 
   * A pointer to the new property is returned. If \a add_property(...) failed, 
   * for example because the property already existed, a null pointer 
   * is returned. 
   */ 
  virtual Grid_continuous_property* add_property( const std::string& name );
  
  /** Adds a new property called \a name where the values
   * are loaded from the file \a filename.
   * A pointer to the new property is returned. If \a add_categorical_property(...)
   * failed,for example because the property already existed, a null pointer
   * is returned.
   */
  virtual Grid_continuous_property* add_property_from_disk( const std::string& name,const std::string& filename );

  /** Adds a new weight property called \a name. 
   * A pointer to the new property is returned. If \a add_property(...) failed, 
   * for example because the property already existed, a null pointer 
   * is returned. 
   */ 
  virtual Grid_weight_property* add_weight_property( const std::string& name );
  
  /** Adds a new weight property called \a name where the values
   * are loaded from the file \a filename.
   * A pointer to the new property is returned. If \a add_categorical_property(...)
   * failed,for example because the property already existed, a null pointer
   * is returned.
   */
  virtual Grid_weight_property* add_weight_property_from_disk( const std::string& name,const std::string& filename );

  /** Adds a new categorical property called \a name.
   * A pointer to the new property is returned. If \a add_categorical_property(...)
   * failed,for example because the property already existed, a null pointer
   * is returned.
   */
  virtual Grid_categorical_property* add_categorical_property(
			  const std::string& name,
			  const std::string& definition_name="Default");

  /** Adds a new categorical property called \a name where the values
   * are loaded from the file @filename.
   * A pointer to the new property is returned. If \a add_categorical_property(...)
   * failed,for example because the property already existed, a null pointer
   * is returned.
   */
  virtual Grid_categorical_property* add_categorical_property_from_disk(
			  const std::string& name, const std::string& filename,
			  const std::string& definition_name="Default");

  /** Removes a given property from the property list 
   * @return false if the function failed, eg because the property 
   * did not exist 
   */ 
  virtual bool remove_property( const std::string& name ); 
 
  /** Selects one of the properties of the grid. That property becomes 
   * the property the grid operates on by default: iterators returned 
   * by begin(), end() will iterate on the default property.  
   */ 
  virtual Grid_continuous_property* select_property( const std::string& name );
  
  /** Returns a pointer to the currently selected property. To select a 
  * property, use function \a select_property( name ).
  * By default, the first property that was added to the grid is the
  * selected property.
  */
  virtual const Grid_continuous_property* selected_property() const; 
  virtual Grid_continuous_property* selected_property();
  
  /** Provides direct access to property \a name. The function returns
  * a pointer to the property array. If property \a name does not exist,
  * a null pointer is returned.
  */
  virtual const Grid_continuous_property* property( const std::string& name ) const; 
  virtual Grid_continuous_property* property( const std::string& name ); 

  /** Provides direct access to categorical property \a name. The function returns
  * a pointer to the categorical property array. If categorical property \a name does not exist,
  * a null pointer is returned.
  */
  virtual const Grid_categorical_property* categorical_property( const std::string& name ) const;
  virtual Grid_categorical_property* categorical_property( const std::string& name );


  /** Provides direct access to weight property \a name. The function returns
  * a pointer to the categorical property array. If weight property \a name does not exist,
  * a null pointer is returned.
  */
  virtual const Grid_weight_property* weight_property( const std::string& name ) const;
  virtual Grid_weight_property* weight_property( const std::string& name );

  /** Gives the list of all the names of the properties currently in the grid.
  */
  virtual std::list<std::string> property_list() const; 

  /** Gives the list of all the names of the categorical properties currently in the grid.
  */
   virtual std::list<std::string> categorical_property_list() const; 

  /** Gives the list of all the names of the weight properties currently in the grid.
  */
   virtual std::list<std::string> weight_property_list() const; 

  /** Adds a multi-realization property to the grid.
  */
  virtual MultiRealization_property*  
    add_multi_realization_property( const std::string& name ); 
 

  //--------------------------- 
  // PropertyGroup management 
  //--------------------------- 
  virtual Grid_property_group* add_group( const std::string& name, const std::string& type  );   
  virtual std::list<std::string> get_group_names(const std::string& type = "") const;   
  virtual unsigned int group_size() const;   
  virtual Grid_property_group* get_group( const std::string& name );   
  virtual const Grid_property_group* get_group( const std::string& name ) const;  
  virtual void remove_group( const std::string& name );

  //--------------------------- 
  // Region management 
 

  virtual std::list<std::string> region_list() const; 

  virtual const Grid_region* region(const std::string& region_name) const;
  virtual Grid_region* region(const std::string& region_name);

  virtual Grid_region* add_region( const std::string& region_name ); 
  virtual bool remove_region( const std::string& region_name ); 
 
  /** Selects a region. After calling this function, any operation  
   * that can be restricted to a region will only operate on the 
   * selected region. For example, an iterator returned by begin() 
   * will only iterate on the selected region.  
   */ 
  virtual bool select_region( const std::string& region_name ); 
//  virtual bool select_region(const std::vector<std::string>& region_names) = 0;
 
  virtual const Grid_region* selected_region() const; 
  virtual Grid_region* selected_region();

  virtual void unselect_region();

  virtual bool is_inside_selected_region(int node_id) const;

  //-------------------------------- 
  // neighborhood definition 

  /** Creates a new ellipsoid neighborhood. The ellipsoid is defined by the 
  * 3 affinity factors \a x, \a y, \a z, and the 3 angles (in degrees) \a ang1,
  * \a ang2, \a ang3.
  * If covariance \a cov is not 0, it will be used by the neighborhood to sort
  * the neighbors it finds: the first neighbor will be the closest to the 
  * central point u, ie the point v such that cov(u,v) is highest.
  * Set \a only_harddata to true to force the neighborhood to only consider
  * hard-data.
  * \warning It is the user's responsability to \c delete the neighborhood 
  * when it is not needed anymore.}
  */
  virtual Neighborhood* neighborhood( double x, double y, double z, 
				      double ang1, double ang2, double ang3, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false, const Grid_region* region = 0,
              Coordinate_mapper* coord_mapper=0)=0 ; 

  /** Creates a new ellipsoid neighborhood. Triplet \a dim defines the affinity 
  * ratios of the ellispoid, and triplet angles the rotation angles.
  * If covariance \a cov is not 0, it will be used by the neighborhood to sort
  * the neighbors it finds: the first neighbor will be the closest to the 
  * central point u, ie the point v such that cov(u,v) is highest.
  * Set \a only_harddata to true to force the neighborhood to only consider
  * hard-data.
  * \warning It is the user's responsability to \c delete the neighborhood 
  * when it is not needed anymore.
  */
  virtual Neighborhood* neighborhood( const GsTLTripletTmpl<double>& dim, 
				      const GsTLTripletTmpl<double>& angles, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false, const Grid_region* region = 0,
              Coordinate_mapper* coord_mapper=0)=0; 
  virtual Neighborhood* colocated_neighborhood( const std::string& prop ); 

  /** It is the used reponsability to set a selected property
  * to the colocated neighborhood
  */
  virtual Neighborhood* colocated_neighborhood(  ); 
  //---------------------------   
 
  //---------------------------- 
  // Misc. 
   
  /** Computes the location of a node, given its node_id. 
  // This is the "geological" coordinates.  Use xyz_location to get
  // the real work coordinate
   */ 
  virtual location_type location( int node_id ) const = 0; 

  /** Computes the location of a node, given its node_id. 
  // This is the "real world" coordinates.  Use location to get
  // the geological coordinate
   */ 
  virtual location_type xyz_location( int node_id ) const {return this->location(node_id);} 

  /** Find the node of the grid which is closest to the input point 
   * The input point (geovalue) may not be a grid node.  
   * @return the node-id of the node closest to "P" 
   */ 
  virtual GsTLInt closest_node( const location_type& P ) const = 0; 
 
  /** This function returns the node-id of the index-th node. 
   * This is useful if there is a re-mapping of the node-id, ie 
   * if we want that the first node has node-id 45, instead of the  
   * default id 1. (this is especially useful for multiple grids). 
   */ 
  virtual GsTLInt node_id( GsTLInt index ) const { return index; } 
 
  /** Returns the total number of nodes in the grid.
  */
  virtual GsTLInt size() const  = 0;

  //TL modified
  virtual bool reNameProperty(std::string old_name, std::string new_name);

  //GsTL_object_item
  virtual int row() const;
  virtual GsTL_object_item* properties_item(){return this->child(0);}
  virtual GsTL_object_item* regions_item()=0;


public :

  protected:
    void clear_selected_region_from_property();

 protected :
   Coordinate_mapper* coord_mapper_;
  Grid_property_group_manager group_manager_;
  Grid_region_manager region_manager_;
  Grid_property_manager property_manager_; 

  std::vector<GsTLInt> grid_path_; 

 protected :
  std::string name_;

}; 
 

inline  
const Grid_continuous_property* Geostat_grid::property( const std::string& name ) const { 
  return property_manager_.get_property( name ); 
} 
 
inline  
Grid_continuous_property* Geostat_grid::property( const std::string& name ) { 
  return property_manager_.get_property( name ); 
} 
 
inline
const Grid_categorical_property* Geostat_grid::categorical_property( const std::string& name ) const{
	 return property_manager_.get_categorical_property( name );
}

inline
Grid_categorical_property* Geostat_grid::categorical_property( const std::string& name ){
	return property_manager_.get_categorical_property( name );
}

inline
const Grid_weight_property* Geostat_grid::weight_property( const std::string& name ) const{
	 return property_manager_.get_weight_property( name );
}

inline
Grid_weight_property* Geostat_grid::weight_property( const std::string& name ){
	return property_manager_.get_weight_property( name );
}

inline const Grid_continuous_property* Geostat_grid::selected_property() const { 
  return property_manager_.selected_property(); 
} 
 
inline Grid_continuous_property* Geostat_grid::selected_property() { 
  return property_manager_.selected_property(); 
} 
 
 
inline 
Grid_region* Geostat_grid::add_region(const std::string& name) { 
  return region_manager_.add_region(name); 
} 
 
inline 
bool Geostat_grid::remove_region(const std::string& name) {

  bool is_selected_property = false;  
  if(selected_region() != NULL) {
    is_selected_property= selected_region()->name() == name;
  }
  bool ok =  region_manager_.remove_region(name);
  if(!ok) return false;

  if( is_selected_property )  
    clear_selected_region_from_property();
  return true; 

} 
 
inline 
Grid_region* Geostat_grid::region(const std::string& name) { 
  return region_manager_.get_region(name);
} 

inline 
const Grid_region* Geostat_grid::region(const std::string& name) const { 
  return region_manager_.get_region(name);
} 

inline const Grid_region* Geostat_grid::selected_region() const { 
  return region_manager_.selected_region(); 
} 
 
inline Grid_region* Geostat_grid::selected_region() { 
  return region_manager_.selected_region(); 
}

inline bool Geostat_grid::select_region(const std::string& region_name) {
  Grid_region* region = region_manager_.select_region( region_name );
  if( region == NULL && !region_name.empty() ) return false;
  Grid_property_manager::Property_name_iterator it = property_manager_.names_begin();
  for(; it != property_manager_.names_end(); ++it) {
    Grid_continuous_property* prop = property_manager_.get_property( *it );
    prop->set_region(region);
  }
  return true;
} 

inline void Geostat_grid::unselect_region() {
  region_manager_.unselect_region();
  clear_selected_region_from_property();
}

inline bool Geostat_grid::is_inside_selected_region(int node_id) const {
  const Grid_region* region = region_manager_.selected_region();
  // When no region is selected, use the full grid
  if( !region ) return true;  
  return region->is_inside_region(node_id);
}


inline Grid_property_group* 
Geostat_grid::add_group( const std::string& name, const std::string& type ) {
  return group_manager_.add_group(name,type);
}

inline std::list<std::string> 
Geostat_grid::get_group_names(const std::string& type) const {
  return group_manager_.group_names(type);
}

inline unsigned int Geostat_grid::group_size() const {
  return group_manager_.size();
}

inline Grid_property_group* 
Geostat_grid::get_group( const std::string& name ){
  return group_manager_.get_group(name);
}
inline const Grid_property_group* 
Geostat_grid::get_group( const std::string& name ) const{
  return group_manager_.get_group(name);
}
 
inline void
Geostat_grid::remove_group( const std::string& name ){
	group_manager_.remove_group(name);
}

 
#endif 
 
