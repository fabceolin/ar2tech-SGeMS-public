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


#include <utils/manager.h>
#include <grid/geostat_grid.h>
#include <grid/neighborhood.h>
#include <grid/grid_property_set.h>


Geostat_grid::Geostat_grid(std::string name) :
	 name_(name),coord_mapper_(0){}

Neighborhood* Geostat_grid::colocated_neighborhood( const std::string& prop ) {
  return new Colocated_neighborhood( this, prop );
}

Neighborhood* Geostat_grid::colocated_neighborhood( ) {
  return new Colocated_neighborhood( this );
}

int Geostat_grid::row() const{
	if(parent_interface_ == 0) return 0;
	Manager* parent  = dynamic_cast<Manager*>(parent_interface_);
	if(parent==0) return 0;
	Manager::interface_iterator begin = parent->begin_interfaces();
	Manager::interface_iterator end = parent->end_interfaces();

	for(int i=0 ; begin != end; ++begin, ++i) {
		if( begin->raw_ptr() == this) {
			return i;
		}
	}
	return 0;
}



Grid_continuous_property* Geostat_grid::add_property( const std::string& name ) {
				
  return property_manager_.add_property( name );
}

Grid_continuous_property* Geostat_grid::add_property_from_disk( const std::string& name, const std::string& filename ) {

  return property_manager_.add_property_from_disk( name, filename );
}


Grid_weight_property* Geostat_grid::add_weight_property( const std::string& name ) {
				
  return property_manager_.add_weight_property( name );
}

Grid_weight_property* Geostat_grid::add_weight_property_from_disk( const std::string& name, const std::string& filename ) {

  return property_manager_.add_weight_property_from_disk( name, filename );
}


Grid_categorical_property* Geostat_grid::add_categorical_property(
		const std::string& name,
		const std::string& definition_name){
	return property_manager_.add_categorical_property( name, definition_name );
}


Grid_categorical_property* Geostat_grid::add_categorical_property_from_disk(
		const std::string& name,
		const std::string& filename,
		const std::string& definition_name){
	return property_manager_.add_categorical_property_from_disk( name, filename, definition_name );
}

bool Geostat_grid::remove_property( const std::string& name ) {
 // return property_manager_.remove_property( name );

  std::string name_group;

  Grid_continuous_property* prop = property(name );
  if(!prop) return false;
  std::vector< Grid_property_group*> groups = prop->groups();
  bool ok = property_manager_.remove_property( name);
  if(!ok) return false;
  for(int i=0; i<groups.size(); i++) {
    if(groups[i]->size() == 0) {
      remove_group(groups[i]->name());
    }
  }
  return true;

}

Grid_continuous_property* Geostat_grid::select_property(const std::string& prop_name) {
  Grid_continuous_property* prop = property_manager_.select_property( prop_name );

  return prop;
}

bool Geostat_grid::reNameProperty(std::string oldName, std::string newName)
{
	return property_manager_.reNameProperty(oldName, newName);
}


std::list<std::string> Geostat_grid::property_list() const {

  std::list<std::string> result;

  Grid_property_manager::Property_name_iterator it = 
    property_manager_.names_begin();
  Grid_property_manager::Property_name_iterator end = 
    property_manager_.names_end();
  for( ; it != end ; ++it )
    result.push_back( *it );

  return result;
}

std::list<std::string> Geostat_grid::categorical_property_list() const {
  std::list<std::string> result;

  Grid_property_manager::Property_name_iterator it =
    property_manager_.names_begin();
  Grid_property_manager::Property_name_iterator end =
    property_manager_.names_end();
  for( ; it != end ; ++it ) {
    const Grid_categorical_property* prop = categorical_property(*it);
    if(prop) result.push_back( *it );
  }

  return result;
}


std::list<std::string> Geostat_grid::weight_property_list() const {
  std::list<std::string> result;

  Grid_property_manager::Property_name_iterator it =
    property_manager_.names_begin();
  Grid_property_manager::Property_name_iterator end =
    property_manager_.names_end();
  for( ; it != end ; ++it ) {
    const Grid_weight_property* prop = weight_property(*it);
    if(prop) result.push_back( *it );
  }

  return result;
}


MultiRealization_property* 
Geostat_grid::add_multi_realization_property( const std::string& name ) {
  MultiRealization_property* mprops = property_manager_.new_multireal_property( name );
  Grid_property_group* group = this->add_group( mprops->name(), "Simulation" );
  if(group) {
    mprops->set_group(group);
  }
  return mprops;
}

 
std::list<std::string> Geostat_grid::region_list() const {

  std::list<std::string> result;

  Grid_region_manager::Region_name_iterator it = 
    region_manager_.names_begin();
  Grid_region_manager::Region_name_iterator end = 
    region_manager_.names_end();
  for( ; it != end ; ++it )
    result.push_back( *it );

  return result;
}


void Geostat_grid::clear_selected_region_from_property(){
  Grid_property_manager::Property_name_iterator it = property_manager_.names_begin();
  for(; it != property_manager_.names_end(); ++it) {
    Grid_continuous_property* prop = property_manager_.get_property( *it );
    prop->set_region(NULL);
  }
}
