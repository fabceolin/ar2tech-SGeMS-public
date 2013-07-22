#include <boost/test/unit_test.hpp>

#include "../utils.h"



/**************************************************
 *  Point_set_neighborhood
 **************************************************/
BOOST_AUTO_TEST_CASE(Point_set_neighborhood___find_neighbors)
{
  std::string pset_name("pset, Point_set_neighborhood___find_neighbors");
  Point_set * pset = point_set(pset_name, 3*3*3);
  BOOST_REQUIRE(pset);
  std::vector< Point_set::location_type > point_locations;
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      for (int z = 0; z < 3; z++) {
        Point_set::location_type loc(x, y, z);
        point_locations.push_back(loc);
      }
  pset->point_locations( point_locations );
  
  // add property to grid
  std::string prop_name("prop, Point_set_neighborhood___find_neighbors");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);

  // creat neighborhood
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Point_set_neighborhood * nbh = point_set_neighborhood(pset, ranges, angles);
  BOOST_REQUIRE(nbh);
  
  // find neighbors
  // no neighbors informed
  Neighbors neighbors;
  nbh->find_neighbors( pset->geovalue(0), neighbors);
  BOOST_CHECK(neighbors.number_informed_neighbors() == 0);
  BOOST_CHECK(neighbors.size() == 0);
  BOOST_CHECK(nbh->size() == 0);
  nbh->find_neighbors( pset->geovalue(0) );
  BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors.begin(), neighbors.end()) );
  // all neighbors informed
  for (int i = 0; i < prop->size(); i++) 
    prop->set_value(i, i);
  nbh->find_neighbors( pset->geovalue(0), neighbors);
  BOOST_CHECK(neighbors.number_informed_neighbors() == 3);
  BOOST_CHECK(nbh->size() == 0);
  nbh->find_neighbors( pset->geovalue(0) );
  BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors.begin(), neighbors.end()) );
  
  // free memory
  delete nbh;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
}


BOOST_AUTO_TEST_CASE(Point_set_neighborhood___find_neighbors___4threads)
{
  std::string pset_name("pset, Point_set_neighborhood___find_neighbors___4threads");
  Point_set * pset = point_set(pset_name, 3*3*3);
  BOOST_REQUIRE(pset);
  std::vector< Point_set::location_type > point_locations;
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      for (int z = 0; z < 3; z++) {
        Point_set::location_type loc(x, y, z);
        point_locations.push_back(loc);
      }
  pset->point_locations( point_locations );
  
  // add property to grid
  std::string prop_name("prop, Point_set_neighborhood___find_neighbors___4threads");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);

  // creat neighborhood
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Point_set_neighborhood * nbh = point_set_neighborhood(pset, ranges, angles);
  BOOST_REQUIRE(nbh);
  
  // center
  Geovalue center = pset->geovalue(0);

  // init parallel tasks (threads)
  const int num_tasks = 4;
  Find_neighbors_task::Ptr tasks[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    tasks[i] = Find_neighbors_task::Ptr_new();
    tasks[i]->center_is(center);
    tasks[i]->neighborhood_is(nbh);
  }
  Computational_task_manager::Ptr task_manager = Computational_task_manager::manager();
  Computational_task::Task_handler * task_handlers[num_tasks];

  // find neighbors
  // all neighbors informed
  for (int i = 0; i < prop->size(); i++) 
    prop->set_value(i, i);
  // run tasks
  for (int i = 0; i < num_tasks; i++) {
    task_handlers[i] = const_cast<Computational_task::Task_handler *>( task_manager->task_add(tasks[i]) );
  }
  // wait for all parallel tasks to finish
  for (int i = 0; i < num_tasks; i++) {
    task_manager->task_join( *(task_handlers[i]) );
  }
  // retrieve result from tasks
  Neighbors * neighbors[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    neighbors[i] = const_cast<Neighbors *>( tasks[i]->neighbors() );
  }
  
  // Validate results
  for (int i = 0; i < num_tasks; i++) {
    BOOST_CHECK(neighbors[i]->number_informed_neighbors() == 3);
    BOOST_CHECK(neighbors[i]->size() == 3);
  }
  for (int i = 1; i < num_tasks; i++) {
    BOOST_CHECK( two_geovalue_lists_equal(neighbors[i]->begin(), neighbors[i]->end(), neighbors[0]->begin(), neighbors[0]->end()) );
  }
  BOOST_CHECK(nbh->size() == 0);
  nbh->find_neighbors(center);
  BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors[0]->begin(), neighbors[0]->end()) );

  // free memory
  delete nbh;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
  for (int i = 0; i < num_tasks; i++) {
    delete task_handlers[i];
  }
}

BOOST_AUTO_TEST_CASE(Point_set_neighborhood___center)
{
  std::string pset_name("pset, Point_set_neighborhood___center");
  Point_set * pset = point_set(pset_name, 3*3*3);
  BOOST_REQUIRE(pset);
  std::vector< Point_set::location_type > point_locations;
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      for (int z = 0; z < 3; z++) {
        Point_set::location_type loc(x, y, z);
        point_locations.push_back(loc);
      }
  pset->point_locations( point_locations );
  
  // add property to grid
  std::string prop_name("prop, Point_set_neighborhood___center");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);

  // creat neighborhood
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Point_set_neighborhood * nbh = point_set_neighborhood(pset, ranges, angles);
  BOOST_REQUIRE(nbh);
  
  // find neighbors
  Neighbors neighbors;
  int node_id = 1;
  nbh->find_neighbors( pset->geovalue(node_id), neighbors);
  BOOST_CHECK(neighbors.center().node_id() == node_id);

  // free memory
  delete nbh;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
}

BOOST_AUTO_TEST_CASE(Point_set_neighborhood___find_neighbors___openmp)
{
  std::string pset_name("pset, Point_set_neighborhood___find_neighbors___openmp");
  Point_set * pset = point_set(pset_name, 3*3*3);
  BOOST_REQUIRE(pset);
  std::vector< Point_set::location_type > point_locations;
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      for (int z = 0; z < 3; z++) {
        Point_set::location_type loc(x, y, z);
        point_locations.push_back(loc);
      }
  pset->point_locations( point_locations );
  
  // add property to grid
  std::string prop_name("prop, Point_set_neighborhood___find_neighbors___openmp");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);

  // creat neighborhood
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Point_set_neighborhood * nbh = point_set_neighborhood(pset, ranges, angles);
  BOOST_REQUIRE(nbh);
  
  // find neighbors
  // all neighbors informed
  for (int i = 0; i < prop->size(); i++) 
    prop->set_value(i, i);
  Geovalue center = pset->geovalue(0);
  #pragma omp parallel for
  for (int i = 0; i < 1000; i++) 
  {
    Neighbors neighbors;
    nbh->find_neighbors( center, neighbors);
    BOOST_CHECK(neighbors.number_informed_neighbors() == 3);
    #pragma omp critical
    {
      nbh->find_neighbors( center );
      BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors.begin(), neighbors.end()) );
    }
  }

  // free memory
  delete nbh;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
}



/**************************************************
 *  Point_set_rectangular_neighborhood
 **************************************************/
BOOST_AUTO_TEST_CASE(Point_set_rectangular_neighborhood___find_neighbors)
{
  std::string pset_name("pset, Point_set_rectangular_neighborhood___find_neighbors");
  Point_set * pset = point_set(pset_name, 3*3*3);
  BOOST_REQUIRE(pset);
  std::vector< Point_set::location_type > point_locations;
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      for (int z = 0; z < 3; z++) {
        Point_set::location_type loc(x, y, z);
        point_locations.push_back(loc);
      }
  pset->point_locations( point_locations );
  
  // add property to grid
  std::string prop_name("prop, Point_set_rectangular_neighborhood___find_neighbors");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);

  // creat neighborhood
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Point_set_rectangular_neighborhood * nbh = point_set_rectangular_neighborhood(pset, prop, ranges, angles);
  BOOST_REQUIRE(nbh);
  
  // find neighbors
  // no neighbors informed
  Neighbors neighbors;
  nbh->find_neighbors( pset->geovalue(0), neighbors);
  BOOST_CHECK(neighbors.number_informed_neighbors() == 0);
  BOOST_CHECK(neighbors.size() == 0);
  BOOST_CHECK(nbh->size() == 0);
  nbh->find_neighbors( pset->geovalue(0) );
  BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors.begin(), neighbors.end()) );
  // all neighbors informed
  for (int i = 0; i < prop->size(); i++) 
    prop->set_value(i, i);
  nbh->find_neighbors( pset->geovalue(0), neighbors);
  BOOST_CHECK(nbh->size() == 0);
  nbh->find_neighbors( pset->geovalue(0) );
  BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors.begin(), neighbors.end()) );
  
  // free memory
  delete nbh;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
}


BOOST_AUTO_TEST_CASE(Point_set_rectangular_neighborhood___find_neighbors___4threads)
{
  std::string pset_name("pset, Point_set_rectangular_neighborhood___find_neighbors___4threads");
  Point_set * pset = point_set(pset_name, 3*3*3);
  BOOST_REQUIRE(pset);
  std::vector< Point_set::location_type > point_locations;
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      for (int z = 0; z < 3; z++) {
        Point_set::location_type loc(x, y, z);
        point_locations.push_back(loc);
      }
  pset->point_locations( point_locations );
  
  // add property to grid
  std::string prop_name("prop, Point_set_rectangular_neighborhood___find_neighbors___4threads");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);

  // creat neighborhood
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Point_set_rectangular_neighborhood * nbh = point_set_rectangular_neighborhood(pset, prop, ranges, angles);
  BOOST_REQUIRE(nbh);
  
  // center
  Geovalue center = pset->geovalue(0);

  // init parallel tasks (threads)
  const int num_tasks = 4;
  Find_neighbors_task::Ptr tasks[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    tasks[i] = Find_neighbors_task::Ptr_new();
    tasks[i]->center_is(center);
    tasks[i]->neighborhood_is(nbh);
  }
  Computational_task_manager::Ptr task_manager = Computational_task_manager::manager();
  Computational_task::Task_handler * task_handlers[num_tasks];

  // find neighbors
  // all neighbors informed
  for (int i = 0; i < prop->size(); i++) 
    prop->set_value(i, i);
  // run tasks
  for (int i = 0; i < num_tasks; i++) {
    task_handlers[i] = const_cast<Computational_task::Task_handler *>( task_manager->task_add(tasks[i]) );
  }
  // wait for all parallel tasks to finish
  for (int i = 0; i < num_tasks; i++) {
    task_manager->task_join( *(task_handlers[i]) );
  }
  // retrieve result from tasks
  Neighbors * neighbors[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    neighbors[i] = const_cast<Neighbors *>( tasks[i]->neighbors() );
  }
  
  // Validate results
  for (int i = 0; i < num_tasks; i++) {
    BOOST_CHECK(neighbors[i]->number_informed_neighbors() == 0);
  }
  for (int i = 1; i < num_tasks; i++) {
    BOOST_CHECK( two_geovalue_lists_equal(neighbors[i]->begin(), neighbors[i]->end(), neighbors[0]->begin(), neighbors[0]->end()) );
  }
  BOOST_CHECK(nbh->size() == 0);
  nbh->find_neighbors(center);
  BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors[0]->begin(), neighbors[0]->end()) );

  // free memory
  delete nbh;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
  for (int i = 0; i < num_tasks; i++) {
    delete task_handlers[i];
  }
}

BOOST_AUTO_TEST_CASE(Point_set_rectangular_neighborhood___center)
{
  std::string pset_name("pset, Point_set_rectangular_neighborhood___center");
  Point_set * pset = point_set(pset_name, 3*3*3);
  BOOST_REQUIRE(pset);
  std::vector< Point_set::location_type > point_locations;
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      for (int z = 0; z < 3; z++) {
        Point_set::location_type loc(x, y, z);
        point_locations.push_back(loc);
      }
  pset->point_locations( point_locations );
  
  // add property to grid
  std::string prop_name("prop, Point_set_rectangular_neighborhood___center");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);

  // creat neighborhood
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Point_set_rectangular_neighborhood * nbh = point_set_rectangular_neighborhood(pset, prop, ranges, angles);
  BOOST_REQUIRE(nbh);
  
  // find neighbors
  Neighbors neighbors;
  int node_id = 1;
  nbh->find_neighbors( pset->geovalue(node_id), neighbors);
  BOOST_CHECK(neighbors.center().node_id() == node_id);

  // free memory
  delete nbh;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
}


BOOST_AUTO_TEST_CASE(Point_set_rectangular_neighborhood___find_neighbors___openmp)
{
  std::string pset_name("pset, Point_set_rectangular_neighborhood___find_neighbors___openmp");
  Point_set * pset = point_set(pset_name, 3*3*3);
  BOOST_REQUIRE(pset);
  std::vector< Point_set::location_type > point_locations;
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      for (int z = 0; z < 3; z++) {
        Point_set::location_type loc(x, y, z);
        point_locations.push_back(loc);
      }
  pset->point_locations( point_locations );
  
  // add property to grid
  std::string prop_name("prop, Point_set_rectangular_neighborhood___find_neighbors___openmp");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);

  // creat neighborhood
  GsTLTriplet ranges(1, 1, 1), angles(0, 0, 0);
  Point_set_rectangular_neighborhood * nbh = point_set_rectangular_neighborhood(pset, prop, ranges, angles);
  BOOST_REQUIRE(nbh);
  
  // find neighbors
  // all neighbors informed
  for (int i = 0; i < prop->size(); i++) 
    prop->set_value(i, i);
  Geovalue center = pset->geovalue(0);
  #pragma omp parallel for
  for (int i = 0; i < 1000; i++) 
  {
    Neighbors neighbors;
    nbh->find_neighbors( center, neighbors);
    BOOST_CHECK(neighbors.number_informed_neighbors() == 0);
    #pragma omp critical
    {
      nbh->find_neighbors( center );
      BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors.begin(), neighbors.end()) );
    }
  }

  // free memory
  delete nbh;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
}