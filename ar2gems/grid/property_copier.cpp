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

#include <grid/property_copier.h>
#include <grid/point_set.h>
#include <grid/cartesian_grid.h>
#include <grid/sgrid_cursor.h>
#include <utils/manager_repository.h>
#include <grid/reduced_grid.h>
#include <grid/structured_grid.h>
#include <grid/grid_categorical_property.h>

#include <GsTL/geometry/geometry_algorithms.h>
#include <qmessagebox.h>

#include <map>

std::string propertyCopier_manager = gridObject_manager + "/PropertyCopier";


SmartPtr<Property_copier> 
Property_copier_factory::get_copier( const Geostat_grid* server, 
                                     const Geostat_grid* client ) {

  if(server == client) {
     return SmartPtr<Property_copier>( new Identical_grid_copier() );
  }

  std::string type = create_type_name( server->classname(),
                                       client->classname() );
 
  SmartPtr<Named_interface> ni =
    Root::instance()->new_interface( type, propertyCopier_manager + "/" );
  
  Property_copier* copier = dynamic_cast< Property_copier* >( ni.raw_ptr() );
  return SmartPtr<Property_copier>( copier );

}
 
  
bool Property_copier_factory::add_method( const std::string& server_type, 
                                          const std::string& client_type,
                                          CallBackType method ) {
  std::string type = create_type_name( server_type, client_type );
  
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( propertyCopier_manager );

  Manager* mng = dynamic_cast< Manager* >( ni.raw_ptr() );
  
  if( !mng ) {
    SmartPtr<Named_interface> ni2 = 
      Root::instance()->new_interface( "directory", propertyCopier_manager ); 
    mng = dynamic_cast< Manager* >( ni2.raw_ptr() );
  }

  if( !mng ) return false;
  
  return mng->factory( type, method );
}


std::string 
Property_copier_factory::create_type_name( const std::string& server_type, 
                                           const std::string& client_type) {
  return ( server_type + ";;" + client_type );
}





//==========================================

Property_copier::Property_copier() {
  from_scratch_ = true;
  overwrite_ = false;
  mark_as_hard_ = true;
  undo_enabled_ = false;
}


void Property_copier::copy_categorical_definition(
  const Grid_continuous_property* server_prop,
  Grid_continuous_property* client_prop) {
    //Set the definition if the property is categorical
    const Grid_categorical_property* server_cprop = 
              dynamic_cast<const Grid_categorical_property*>(server_prop);
    Grid_categorical_property* client_cprop = 
              dynamic_cast<Grid_categorical_property*>(client_prop);
    if(server_cprop && client_cprop ) {
      client_cprop->set_category_definition(server_cprop->get_category_definition()->name());
    }
}


//==========================================
Mask_to_mask_copier::Mask_to_mask_copier()
{
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  
  unset_harddata_flag_ = true;

}

bool Mask_to_mask_copier::undo_copy()
{
	return true;
}


bool Mask_to_mask_copier::copy( const Geostat_grid* server, 
                                 const Grid_continuous_property* server_prop,
                                 Geostat_grid* client, 
                                 Grid_continuous_property* client_prop ) 
{
	Reduced_grid* to_grid = dynamic_cast< Reduced_grid* >( client );
	const Reduced_grid* from_grid = dynamic_cast< const Reduced_grid* >( server );

	if( !from_grid || !to_grid ) return false;
  if (from_grid != to_grid) return false ; // to remove the 

  copy_categorical_definition(server_prop,client_prop);

	for( int i=0; i < server_prop->size() ; i++ ) {
		if( server_prop->is_informed( i ) ) {
			client_prop->set_value( server_prop->get_value( i ), i );
			if( mark_as_hard_ )   //added by Yongshe
				client_prop->set_harddata( true, i); //added by Yongshe
		}
		else if(overwrite_)
			client_prop->set_not_informed( i );
	}
	return true;
}
								 
									 
//===========================================

Pset_to_cgrid_copier::Pset_to_cgrid_copier() 
  : Property_copier() {
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  
  unset_harddata_flag_ = true;

}

bool Pset_to_cgrid_copier::copy( const Geostat_grid* server, 
                                 const Grid_continuous_property* server_prop,
                                 Geostat_grid* client, 
                                 Grid_continuous_property* client_prop ) {
  Cartesian_grid* cgrid = dynamic_cast< Cartesian_grid* >( client );
  const Point_set* pset = dynamic_cast< const Point_set* >( server );

  if( !cgrid || !pset ) return false;

  typedef Grid_continuous_property::property_type Property_type;

  copy_categorical_definition(server_prop,client_prop);

  // check if we already worked with "source" and "property_name" 
  // If that's the case and we're not required to do the assignement
  // from scratch, use what we did last time.
  if( !from_scratch_ && server == server_ &&
      server_prop == server_prop_ && client == client_ ) {
    
    for( unsigned int i = 0 ; i < last_assignement_.size() ; i++ ) {
      Property_type val = server_prop->get_value( last_assignement_[i].first );
      client_prop->set_value( val, last_assignement_[i].second );
      if( mark_as_hard_ )
      	client_prop->set_harddata( true, last_assignement_[i].second );
    } 
    return true;
  }
  
  last_assignement_.clear();
  backup_.clear();
  server_ = server;
  server_prop_ = server_prop;
  client_ = client;
  client_property_ = client_prop;

  typedef Point_set::location_type location_type;
  typedef std::vector<location_type> Location_vector;
  typedef std::vector<location_type>::const_iterator const_iterator;

  // We will need to obtain the coordinates of a grid node from its
  // node id. Hence we need a grid-cursor, set to multigrid level 1.
  SGrid_cursor cursor = *( cgrid->cursor() );
  cursor.set_multigrid_level( 1 );

  const Location_vector& locations = pset->point_locations();

  GsTL_cube bbox = cgrid->bounding_box();
  GsTLInt current_id = 0;

  // Use a map to record what point was assigned to which grid node
  // This map is used in case multiple points could be assigned to the
  // same grid node: in that case the new point is assigned if it is closer
  // to the grid node than the previously assigned node was.
  typedef std::map<GsTLInt,location_type>::iterator map_iterator;
  std::map<GsTLInt,location_type> already_assigned;

  overwrite_ = true;   // added by Jianbing Wu, 03/20/2006

  for( const_iterator loc_ptr = locations.begin(); loc_ptr != locations.end(); 
       ++loc_ptr, current_id++ ) {

    if( !server_prop->is_informed( current_id ) ) continue;

    // only consider the points inside the target's bounding box
    if( !bbox.contains( *loc_ptr ) ) continue;


    GsTLInt node_id = cgrid->closest_node( *loc_ptr );
    appli_assert( node_id >=0 && node_id < client_prop->size() );
    appli_assert( current_id < server_prop->size() );

    // If there is already a property value (not assigned by the
    // grid initializer), and we don't want to overwrite, leave it alone
    if( !overwrite_ && client_prop->is_informed( node_id ) ) continue;

//    bool perform_assignment = true;

    // check if a point was already assigned to that node
    map_iterator it = already_assigned.find( node_id );
    if( it != already_assigned.end() ) {
    	int i,j,k;
    	GsTLCoordVector sizes = cgrid->cell_dimensions();
     	cursor.coords( node_id, i,j,k );
     	location_type node_loc( float(i)*sizes.x(),
			                        float(j)*sizes.y(),
			                        float(k)*sizes.z() );

     	// if the new point is further away to the grid node than
     	// the already assigned node, don't assign the new point
     	if( square_euclidean_distance( node_loc, *loc_ptr ) > 
          square_euclidean_distance( node_loc, it->second ) ) 
        continue; //perform_assignment = false;
    }

//    if( perform_assignment ) {
  	// store the node id of the source and of the target and make a backup of the
    // property value of the client property
   	last_assignement_.push_back( std::make_pair( current_id, node_id ) );

    if( !undo_enabled_ ) {
      Property_type backup_val = Grid_continuous_property::no_data_value;
      if( client_prop->is_informed( node_id ) )
        backup_val = client_prop->get_value( node_id );
      backup_.push_back( std::make_pair( node_id, backup_val ) );
    }

    Property_type val = server_prop->get_value( current_id ); 
    client_prop->set_value( val, node_id );
    already_assigned[node_id] = *loc_ptr;
    if( mark_as_hard_ )
        client_prop->set_harddata( true, node_id );

    }

  overwrite_ = false;   // added by Jianbing Wu, 03/20/2006

  unset_harddata_flag_ = mark_as_hard_;
  return true;
}




bool Pset_to_cgrid_copier::undo_copy() {
  //if( !undo_enabled_ ) return false;

  for( unsigned int i = 0 ; i < backup_.size() ; i++ ) {
    client_property_->set_value( backup_[i].second, backup_[i].first );
    if( unset_harddata_flag_ ) 
      client_property_->set_harddata( false, backup_[i].first );
  }

  return true;
}






//=========================================

Cgrid_to_cgrid_copier::Cgrid_to_cgrid_copier() 
  : Property_copier() {
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  
  unset_harddata_flag_ = true;

}



bool Cgrid_to_cgrid_copier::copy( const Geostat_grid* server, 
                                  const Grid_continuous_property* server_prop,
                                  Geostat_grid* client, 
                                  Grid_continuous_property* client_prop ) {
  Cartesian_grid* to_grid = dynamic_cast< Cartesian_grid* >( client );
  const Cartesian_grid* from_grid = dynamic_cast< const Cartesian_grid* >( server );

  if( !from_grid || !to_grid ) return false;

  copy_categorical_definition(server_prop,client_prop);

  // if the 2 grids are identical, just copy the property
  if( are_identical_grids( from_grid, to_grid ) ) {
    
    appli_assert( server_prop->size() == client_prop->size() );
    for(int nodeid=0; nodeid<from_grid->size();++nodeid) {
      if( server_prop->is_informed(nodeid) ) {
      
		    //client_prop->set_value( server_prop->get_value( i ), i );
        client_prop->set_value( server_prop->get_value(nodeid), nodeid );
		    if( mark_as_hard_ )   //added by Yongshe
                client_prop->set_harddata( true, nodeid); //added by Yongshe
		  }
	    else if(overwrite_)
		    client_prop->set_not_informed( nodeid );
    }
    return true;
  }
  

  typedef Grid_continuous_property::property_type Property_type;

  // check if we already worked with "source" and "property_name" 
  // If that's the case and we're not required to do the assignement
  // from scratch, use what we did last time.
  if( !from_scratch_ && server == server_ &&
      server_prop == server_prop_ && client == client_ ) {
    
    for( unsigned int i = 0 ; i < last_assignement_.size() ; i++ ) {
      Property_type val = server_prop->get_value( last_assignement_[i].first );
      client_prop->set_value( val, last_assignement_[i].second );
      if( mark_as_hard_ )
      	client_prop->set_harddata( true, last_assignement_[i].second );
    } 
    return true;
  }
  
  last_assignement_.clear();
  backup_.clear();
  server_ = server;
  server_prop_ = server_prop;
  client_ = client;
  client_property_ = client_prop;

 
  // We will need to obtain the coordinates of a grid node from its
  // node id. Hence we need a grid-cursor, set to multigrid level 1.
  SGrid_cursor cursor = *( to_grid->cursor() );
  cursor.set_multigrid_level( 1 );

  GsTL_cube bbox = to_grid->bounding_box();

  // Use a map to record what point was assigned to which grid node
  // This map is used in case multiple points could be assigned to the
  // same grid node: in that case the new point is assigned if it is closer
  // to the grid node than the previously assigned node was.
  typedef Cartesian_grid::location_type location_type;
  typedef std::map<GsTLInt,location_type>::iterator map_iterator;
  std::map<GsTLInt,location_type> already_assigned;

  for(int server_id = 0 ; server_id < from_grid->size(); ++server_id ) {

    if( !server_prop->is_informed(server_id) ) continue;

    // only consider the points inside the target's bounding box
    location_type current_loc = from_grid->location(server_id);
    if( !bbox.contains( current_loc) ) continue;


    GsTLInt node_id = to_grid->closest_node( current_loc );
    appli_assert( node_id >=0 && node_id < client_prop->size() );
    appli_assert( server_id < server_prop->size() );
    if(  !to_grid->is_inside_selected_region(node_id) ) continue;
            
    // If there is already a property value (not assigned by the
    // grid initializer), and we don't want to overwrite, leave it alone
    if( !overwrite_ && client_prop->is_informed( node_id ) ) continue;


//    bool perform_assignment = true;

    // check if a point was already assigned to that node
    map_iterator it = already_assigned.find( node_id );
    if( it != already_assigned.end() ) {
    	int i,j,k;
    	GsTLCoordVector sizes = to_grid->cell_dimensions();
     	cursor.coords( node_id, i,j,k );
     	location_type node_loc( float(i)*sizes.x(),
			                        float(j)*sizes.y(),
			                        float(k)*sizes.z() );

     	// if the new point is further away to the grid node than
     	// the already assigned node, don't assign the new point
     	if( square_euclidean_distance( node_loc, current_loc ) > 
          square_euclidean_distance( node_loc, it->second ) ) 
        continue; //perform_assignment = false;
    }
	
//    if( perform_assignment ) {
  	// store the node id of the source and of the target and make a backup of the
    // property value of the client property
   	last_assignement_.push_back( std::make_pair( server_id, node_id ) );

    if( undo_enabled_ ) {
      Property_type backup_val = Grid_continuous_property::no_data_value;
      if( client_prop->is_informed( node_id ) )
        backup_val = client_prop->get_value( node_id );
      backup_.push_back( std::make_pair( node_id, backup_val ) );
    }

    Property_type val = server_prop->get_value(server_id);
    client_prop->set_value( val, node_id );
    already_assigned[node_id] = current_loc;
    if( mark_as_hard_ )
      client_prop->set_harddata( true, node_id );
    
  }
  
  unset_harddata_flag_ = mark_as_hard_;
  return true;
}




bool Cgrid_to_cgrid_copier::undo_copy() {
  for( unsigned int i = 0 ; i < backup_.size() ; i++ ) {
    client_property_->set_value( backup_[i].second, backup_[i].first );
    if( unset_harddata_flag_ ) 
      client_property_->set_harddata( false, backup_[i].first );
  }

  return true;
}


bool Cgrid_to_cgrid_copier::
are_identical_grids( const Cartesian_grid* grid1, 
                     const Cartesian_grid* grid2 ) {
  if( grid1 == grid2 ) return true;

  if( grid1->cell_dimensions() == grid2->cell_dimensions() &&
//      grid1->origin() == grid2->origin() &&
      grid1->nx() == grid2->nx() && grid1->ny() == grid2->ny() &&
      grid1->nz() == grid2->nz() ) {
    return true;
  }

  return false;
}



Pset_to_mask_copier::Pset_to_mask_copier() 
  : Property_copier() {
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  

}

bool Pset_to_mask_copier::copy( const Geostat_grid* server, 
                                  const Grid_continuous_property* server_prop,
                                  Geostat_grid* client, 
                                  Grid_continuous_property* client_prop ) {
  Reduced_grid* to_grid = dynamic_cast< Reduced_grid* >( client );
  const Point_set* from_grid = dynamic_cast< const Point_set* >( server );
  client_property_ = client_prop;
  index_client_to_source_.clear();
  original_client_value_.clear();

  int nx = to_grid->nx();
  int ny = to_grid->ny();
  int nz = to_grid->nz();
  GsTLPoint o = to_grid->origin();
  int xy = nx*ny;
  int id;

  if( !from_grid || !to_grid ) return false;

  copy_categorical_definition(server_prop,client_prop);

  // Use a map to record what point was assigned to which grid node
  // This map is used in case multiple points could be assigned to the
  // same grid node: in that case the new point is assigned if it is closer
  // to the grid node than the previously assigned node was.
  typedef Geostat_grid::location_type location_type;
  typedef std::map<GsTLInt,location_type>::iterator map_iterator;
  std::map<GsTLInt,location_type> already_assigned;

  // Copy the property
  location_type pset_loc;
   for( int i=0; i < server_prop->size() ; i++ ) {
	   pset_loc = from_grid->location(i);
     id = to_grid->closest_node(pset_loc);
     if(id < 0 ) continue;

     std::map<GsTLInt,location_type>::iterator it = already_assigned.find(id);
     if( it != already_assigned.end() ) {
       
       location_type to_grid_loc = to_grid->location(id);

     	  // if the new point is further away to the grid node than
     	  // the already assigned node, don't assign the new point
     	  if( square_euclidean_distance( pset_loc, to_grid_loc) > 
          square_euclidean_distance( it->second, to_grid_loc ) ) 
          continue; 
     }

     if( server_prop->is_informed(i ) ) {

      // Check if it the first time this client node is considered
      if(client_prop->is_informed(id) && it == already_assigned.end()) {
        original_client_value_[id] = client_prop->get_value(id);
      }


      client_prop->set_value( server_prop->get_value( i ), id );
      already_assigned[id] = pset_loc;

      if(mark_as_hard_) client_prop->set_harddata(true,id);

      index_client_to_source_[id] = i;
       

     }
    else if(overwrite_) {
		client_prop->set_not_informed( id );
    index_client_to_source_[id] = i;
    already_assigned[id] = pset_loc;
    }

   }

   return true;
}

bool Pset_to_mask_copier::undo_copy(){
  std::map<int,int>::const_iterator it = index_client_to_source_.begin();

  for( ; it != index_client_to_source_.end(); ++it) {
    int client_id = it->first;
    std::map<int,float>::const_iterator it_client = original_client_value_.find(client_id);
    if(it_client != original_client_value_.end()) {
      client_property_->set_value( it_client->second, client_id  );
    }
    else {
      client_property_->set_not_informed(client_id);
    }
  }
  return true;
}

//--------------------------
Pset_to_pset_copier::Pset_to_pset_copier() 
  : Property_copier() {
  server_ = 0;
  client_ = 0;
  server_prop_ = 0;
  client_property_ = 0;
  
  unset_harddata_flag_ = true;

}


bool Pset_to_pset_copier::copy( const Geostat_grid* server, 
                                  const Grid_continuous_property* server_prop,
                                  Geostat_grid* client, 
                                  Grid_continuous_property* client_prop ) {
  Point_set* to_grid = dynamic_cast< Point_set* >( client );
  const Point_set* from_grid = dynamic_cast< const Point_set* >( server );

  if( !from_grid || !to_grid ) return false;

  //Only allow the copy on the same grid
  if( !are_identical_grids(from_grid,to_grid) ) return false;

  copy_categorical_definition(server_prop,client_prop);

  // Copy the property
  for( int nodeid=0 ; nodeid < from_grid->size(); ++nodeid) {
    if( server_prop->is_informed(nodeid) && to_grid->is_inside_selected_region(nodeid) ) 
      client_prop->set_value( server_prop->get_value_no_check(nodeid), nodeid );
	  else if(overwrite_)
		  client_prop->set_not_informed( nodeid );
  }


   return true;
}



//-----------------------------

bool Rgrid_to_pset_copier::copy( const Geostat_grid* server, 
                     const Grid_continuous_property* server_prop,
                     Geostat_grid* client, 
                     Grid_continuous_property* client_prop )
{
  const RGrid* server_rgrid = dynamic_cast<const RGrid*>(server);
  Point_set* client_pset = dynamic_cast<Point_set*>(client);

  // looping on the point set since it is the constraining grid
  for(int client_id=0 ; client_id < client_pset->size(); ++client_id) {
    if(!client_pset->is_inside_selected_region(client_id) ) continue;

    Geostat_grid::location_type loc = client_pset->location(client_id);
    int node_id = server_rgrid->closest_node(loc);
    if(node_id < 0 ) continue;


    if( server_prop->is_informed(node_id) )  {
      client_prop->set_value(server_prop->get_value(node_id),client_id);
      if( mark_as_hard_ ) client_prop->set_harddata( true, client_id );
    }
	  else if(overwrite_)
		  client_prop->set_not_informed( client_id );

  }

  return true;
}

// --------------------

bool Pset_to_structured_grid_copier::copy( const Geostat_grid* server, 
                     const Grid_continuous_property* server_prop,
                     Geostat_grid* client, 
                     Grid_continuous_property* client_prop ) {

  Structured_grid* to_grid = dynamic_cast< Structured_grid* >( client );
  const Point_set* from_grid = dynamic_cast< const Point_set* >( server );
  client_property_ = client_prop;
  index_client_to_source_.clear();
  original_client_value_.clear();

  int nx = to_grid->nx();
  int ny = to_grid->ny();
  int nz = to_grid->nz();
  GsTLPoint o = to_grid->origin();
  int xy = nx*ny;

  if( !from_grid || !to_grid ) return false;

  copy_categorical_definition(server_prop,client_prop);

  // Use a map to record what point was assigned to which grid node
  // This map is used in case multiple points could be assigned to the
  // same grid node: in that case the new point is assigned if it is closer
  // to the grid node than the previously assigned node was.
  typedef Geostat_grid::location_type location_type;
  typedef std::map<GsTLInt,location_type>::iterator map_iterator;
  std::map<GsTLInt,location_type> already_assigned;

  // Copy the property
   for( int i=0; i < server_prop->size() ; i++ ) {
     location_type pset_loc = to_grid->coordinate_mapper()->uvw_coords(from_grid->location(i));
	   //pset_loc = from_grid->location(i);
     int id = to_grid->closest_node(pset_loc);
     if(id < 0 ) continue;

     std::map<GsTLInt,location_type>::iterator it = already_assigned.find(id);
     if( it != already_assigned.end() ) {
       
       location_type to_grid_loc = to_grid->location(id);

     	  // if the new point is further away to the grid node than
     	  // the already assigned node, don't assign the new point
     	  if( square_euclidean_distance( pset_loc, to_grid_loc) > 
          square_euclidean_distance( it->second, to_grid_loc ) ) 
          continue; 
     }

     if( server_prop->is_informed(i ) ) {

      // Check if it the first time this client node is considered
      if(client_prop->is_informed(id) && it == already_assigned.end()) {
        original_client_value_[id] = client_prop->get_value(id);
      }


      client_prop->set_value( server_prop->get_value( i ), id );
      already_assigned[id] = pset_loc;

      if(mark_as_hard_) client_prop->set_harddata(true,id);

      index_client_to_source_[id] = i;
       

     }
    else if(overwrite_) {
		client_prop->set_not_informed( id );
    index_client_to_source_[id] = i;
    already_assigned[id] = pset_loc;
    }

   }

   return true;
}

bool Pset_to_structured_grid_copier::undo_copy(){
  std::map<int,int>::const_iterator it = index_client_to_source_.begin();

  for( ; it != index_client_to_source_.end(); ++it) {
    int client_id = it->first;
    std::map<int,float>::const_iterator it_client = original_client_value_.find(client_id);
    if(it_client != original_client_value_.end()) {
      client_property_->set_value( it_client->second, client_id  );
    }
    else {
      client_property_->set_not_informed(client_id);
    }
  }
  return true;
}





//=========================================

Identical_grid_copier::Identical_grid_copier()
  : Property_copier() {
  grid_ = 0;
  client_property_ = 0;

  unset_harddata_flag_ = true;

}



bool Identical_grid_copier::copy( const Geostat_grid* server,
                                  const Grid_continuous_property* server_prop,
                                  Geostat_grid* client,
                                  Grid_continuous_property* client_prop ) {

  if( server != client ) return false;
  grid_ = server;

  copy_categorical_definition(server_prop,client_prop);

    appli_assert( server_prop->size() == client_prop->size() );
    for(int nodeid=0; nodeid<server->size();++nodeid) {
      if( server_prop->is_informed(nodeid) ) {

            //client_prop->set_value( server_prop->get_value( i ), i );
        client_prop->set_value( server_prop->get_value(nodeid), nodeid );
            if( mark_as_hard_ )   //added by Yongshe
                client_prop->set_harddata( true, nodeid); //added by Yongshe
          }
        else if(overwrite_)
            client_prop->set_not_informed( nodeid );
    }
    return true;


}




bool Identical_grid_copier::undo_copy() {
  for( unsigned int i = 0 ; i < backup_.size() ; i++ ) {
    client_property_->set_value( backup_[i].second, backup_[i].first );
    if( unset_harddata_flag_ )
      client_property_->set_harddata( false, backup_[i].first );
  }

  return true;
}




