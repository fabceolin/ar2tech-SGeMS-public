#include <boost/test/unit_test.hpp>

#include "../utils.h"


/**************************************************
 *  Rgrid_window_neighborhood
 **************************************************/
BOOST_AUTO_TEST_CASE(Rgrid_window_neighborhood___find_neighbors)
{
  std::string grid_name("grid, Rgrid_window_neighborhood___find_neighbors");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  grid->set_dimensions(3, 3, 3);
  SGrid_cursor cursor(*grid->cursor());
  
  // add property to grid
  std::string prop_name("prop, Rgrid_window_neighborhood___find_neighbors");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create neighborhood
  Rgrid_window_neighborhood * nbh = six_face_rgrid_window_neighborhood(grid);
  BOOST_REQUIRE(nbh);
  BOOST_REQUIRE(nbh->geometry().size() == 6);

  // find neighbors
  Neighbors neighbors;
  // 1 neighbors informed
  grid->geovalue(cursor.node_id(1, 0, 0)).set_property_value(1);
  int node_id = cursor.node_id(0, 0, 0);
  nbh->find_neighbors( grid->geovalue(node_id), neighbors);
  BOOST_CHECK(neighbors.number_informed_neighbors() == 1);
  BOOST_CHECK(neighbors.size() == 6);
  BOOST_CHECK(nbh->size() == 0);
  // 2 neighbors informed
  grid->geovalue(cursor.node_id(0, 1, 0)).set_property_value(1);
  grid->geovalue(cursor.node_id(1, 1, 1)).set_property_value(1);
  node_id = cursor.node_id(0, 0, 0);
  nbh->find_neighbors( grid->geovalue(node_id), neighbors);
  BOOST_CHECK(neighbors.number_informed_neighbors() == 2);
  BOOST_CHECK(neighbors.size() == 6);
  BOOST_CHECK(nbh->size() == 0);
  // all neighbors informed
  for (int i = 0; i < prop->size(); i++) 
    prop->set_value(i, i);
  node_id = cursor.node_id(1, 1, 1);
  nbh->find_neighbors( grid->geovalue(node_id), neighbors);
  BOOST_CHECK(neighbors.number_informed_neighbors() == 6);
  BOOST_CHECK(neighbors.size() == 6);
  BOOST_CHECK(nbh->size() == 0);

  // free memory
  delete nbh;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
}


BOOST_AUTO_TEST_CASE(Rgrid_window_neighborhood___find_neighbors___4threads)
{
  std::string grid_name("grid, Rgrid_window_neighborhood___find_neighbors___4threads");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  grid->set_dimensions(3, 3, 3);
  SGrid_cursor cursor(*grid->cursor());
  
  // add property to grid
  std::string prop_name("prop, Rgrid_window_neighborhood___find_neighbors___4threads");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create neighborhood
  Rgrid_window_neighborhood * nbh = six_face_rgrid_window_neighborhood(grid);
  BOOST_REQUIRE(nbh);
  BOOST_REQUIRE(nbh->geometry().size() == 6);

  // center
  int node_id = cursor.node_id(1, 1, 1);
  Geovalue center = grid->geovalue(node_id);

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
    BOOST_CHECK(neighbors[i]->number_informed_neighbors() == 6);
    BOOST_CHECK(neighbors[i]->size() == 6);
  }
  for (int i = 1; i < num_tasks; i++) {
    BOOST_CHECK( two_geovalue_lists_equal(neighbors[i]->begin(), neighbors[i]->end(), neighbors[0]->begin(), neighbors[0]->end()) );
  }
  BOOST_CHECK(nbh->size() == 0);
  nbh->find_neighbors(grid->geovalue(node_id));
  BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors[0]->begin(), neighbors[0]->end()) );

  // free memory
  delete nbh;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
  for (int i = 0; i < num_tasks; i++) {
    delete task_handlers[i];
  }
}



BOOST_AUTO_TEST_CASE(Rgrid_window_neighborhood___find_all_neighbors)
{
  std::string grid_name("grid, Rgrid_window_neighborhood___find_all_neighbors");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  grid->set_dimensions(3, 3, 3);
  SGrid_cursor cursor(*grid->cursor());
  
  // add property to grid
  std::string prop_name("prop, Rgrid_window_neighborhood___find_all_neighbors");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create neighborhood
  Rgrid_window_neighborhood * nbh = six_face_rgrid_window_neighborhood(grid);
  BOOST_REQUIRE(nbh);
  BOOST_REQUIRE(nbh->geometry().size() == 6);

  // find all neighbors
  Neighbors neighbors;
  // no neighbors informed
  int node_id = cursor.node_id(1, 1, 1);
  nbh->find_all_neighbors( grid->geovalue(node_id), neighbors);
  BOOST_CHECK(neighbors.size() == 6);
  BOOST_CHECK(neighbors.number_informed_neighbors() == 0);
  BOOST_CHECK(nbh->size() == 0);
  // (2,2,2) node
  node_id = cursor.node_id(2, 2, 2);
  nbh->find_all_neighbors( grid->geovalue(node_id), neighbors);
  BOOST_CHECK(neighbors.size() == 6);
  BOOST_CHECK(neighbors.number_informed_neighbors() == 0);
  BOOST_CHECK(nbh->size() == 0);
  // all neighbors informed
  for (int i = 0; i < prop->size(); i++) 
    prop->set_value(i, i);
  node_id = cursor.node_id(1, 1, 1);
  nbh->find_all_neighbors( grid->geovalue(node_id), neighbors);
  BOOST_CHECK(neighbors.size() == 6);
  BOOST_CHECK(neighbors.number_informed_neighbors() == 6);
  BOOST_CHECK(nbh->size() == 0);

  // free memory
  delete nbh;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
}


BOOST_AUTO_TEST_CASE(Rgrid_window_neighborhood___find_all_neighbors___4threads)
{
  std::string grid_name("grid, Rgrid_window_neighborhood___find_all_neighbors___4threads");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  grid->set_dimensions(3, 3, 3);
  SGrid_cursor cursor(*grid->cursor());
  
  // add property to grid
  std::string prop_name("prop, Rgrid_window_neighborhood___find_all_neighbors___4threads");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create neighborhood
  Rgrid_window_neighborhood * nbh = six_face_rgrid_window_neighborhood(grid);
  BOOST_REQUIRE(nbh);
  BOOST_REQUIRE(nbh->geometry().size() == 6);

  // center
  int node_id = cursor.node_id(1, 1, 1);
  Geovalue center = grid->geovalue(node_id);

  // init parallel tasks (threads)
  const int num_tasks = 4;
  Find_all_neighbors_task::Ptr tasks[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    tasks[i] = Find_all_neighbors_task::Ptr_new();
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
    BOOST_CHECK(neighbors[i]->number_informed_neighbors() == 6);
    BOOST_CHECK(neighbors[i]->size() == 6);
  }
  for (int i = 1; i < num_tasks; i++) {
    BOOST_CHECK( two_geovalue_lists_equal(neighbors[i]->begin(), neighbors[i]->end(), neighbors[0]->begin(), neighbors[0]->end()) );
  }
  BOOST_CHECK(nbh->size() == 0);
  nbh->find_neighbors(grid->geovalue(node_id));
  BOOST_CHECK( two_geovalue_lists_equal(nbh->begin(), nbh->end(), neighbors[0]->begin(), neighbors[0]->end()) );

  // free memory
  delete nbh;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
  for (int i = 0; i < num_tasks; i++) {
    delete task_handlers[i];
  }
}

BOOST_AUTO_TEST_CASE(Rgrid_window_neighborhood___center)
{
  std::string grid_name("grid, Rgrid_window_neighborhood___center");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  grid->set_dimensions(3, 3, 3);
  SGrid_cursor cursor(*grid->cursor());
  
  // add property to grid
  std::string prop_name("prop, Rgrid_window_neighborhood___center");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create neighborhood
  Rgrid_window_neighborhood * nbh = six_face_rgrid_window_neighborhood(grid);
  BOOST_REQUIRE(nbh);
  BOOST_REQUIRE(nbh->geometry().size() == 6);

  // find neighbors
  Neighbors neighbors;
  int node_id = cursor.node_id(1, 1, 1);
  nbh->find_neighbors( grid->geovalue(node_id), neighbors);
  BOOST_CHECK(neighbors.center().node_id() == node_id);

  // free memory
  delete nbh;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
}