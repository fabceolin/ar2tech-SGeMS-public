#include <boost/test/unit_test.hpp>

#include "../utils.h"



/**************************************************
 *  RGrid
 **************************************************/
BOOST_AUTO_TEST_CASE(RGrid_ordered_path___begin___end___size)
{
  std::string grid_name("grid, RGrid_ordered_path___begin___end___size");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  const int grid_size = 3*3*3;
  grid->set_dimensions(3, 3, 3);
  SGrid_cursor cursor(*grid->cursor());
  
  // add property to grid
  std::string prop_name("prop, RGrid_ordered_path___begin___end___size");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create grid_ordered_path
  Grid_ordered_path * path = new Grid_ordered_path(grid, prop);
  BOOST_REQUIRE(path);

  // validate path
  BOOST_CHECK(path->size() == grid_size);
  BOOST_CHECK( two_geovalue_sets_equal(path->begin(), path->end(), grid->begin(), grid->end()) );

  // free memory
  delete path;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
}


BOOST_AUTO_TEST_CASE(RGrid_ordered_path___node_id___geovalue)
{
  std::string grid_name("grid, RGrid_ordered_path___node_id___geovalue");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  const int grid_size = 3*3*3;
  grid->set_dimensions(3, 3, 3);
  SGrid_cursor cursor(*grid->cursor());
  
  // add property to grid
  std::string prop_name("prop, RGrid_ordered_path___node_id___geovalue");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create grid_ordered_path
  Grid_ordered_path * path = new Grid_ordered_path(grid, prop);
  BOOST_REQUIRE(path);

  // validate path
  BOOST_CHECK(path->size() == grid_size);
  for (int i = 0; i < path->size(); i++) {
    GsTLInt node_id = path->node_id(i);
    Geovalue geo = path->geovalue(i);
    BOOST_CHECK( node_id == geo.node_id() );
  }

  // free memory
  delete path;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
}


BOOST_AUTO_TEST_CASE(RGrid_ordered_path___4threads)
{
  std::string grid_name("grid, RGrid_ordered_path___4threads");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  const int grid_size = 3*3*3;
  grid->set_dimensions(3, 3, 3);
  SGrid_cursor cursor(*grid->cursor());
  
  // add property to grid
  std::string prop_name("prop, RGrid_ordered_path___4threads");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create grid_ordered_path
  Grid_ordered_path * path = new Grid_ordered_path(grid, prop);
  BOOST_REQUIRE(path);

  // init parallel tasks (threads)
  const int num_tasks = 4;
  Grid_path_task<Grid_ordered_path::iterator>::Ptr tasks[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    tasks[i] = Grid_path_task<Grid_ordered_path::iterator>::Ptr_new();
    tasks[i]->grid_path_is(path);
  }
  Computational_task_manager::Ptr task_manager = Computational_task_manager::manager();
  Computational_task::Task_handler * task_handlers[num_tasks];

  // run tasks
  for (int i = 0; i < num_tasks; i++) {
    task_handlers[i] = const_cast<Computational_task::Task_handler *>( task_manager->task_add(tasks[i]) );
  }
  // wait for all parallel tasks to finish
  for (int i = 0; i < num_tasks; i++) {
    task_manager->task_join( *(task_handlers[i]) );
  }
  // retrieve result from tasks
  std::vector<Grid_ordered_path::iterator> nodes[num_tasks];
  std::vector<GsTLInt> node_ids[num_tasks];
  std::vector<Geovalue> geovalues[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    nodes[i] = tasks[i]->nodes();
    node_ids[i] = tasks[i]->node_ids();
    geovalues[i] = tasks[i]->geovalues();
  }
  
  // Validate results
  for (int i = 0; i < num_tasks; i++) {
    BOOST_CHECK( nodes[i].size() == grid_size );
    BOOST_CHECK( node_ids[i].size() == grid_size );
    BOOST_CHECK( geovalues[i].size() == grid_size );
  }
  for (int i = 1; i < num_tasks; i++) {
    BOOST_CHECK( two_lists_equal(nodes[i].begin(), nodes[i].end(), nodes[0].begin(), nodes[0].end()) );
  }

  // free memory
  delete path;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
  for (int i = 0; i < num_tasks; i++) {
    delete task_handlers[i];
  }
}



/**************************************************
 *  Point-set
 **************************************************/
BOOST_AUTO_TEST_CASE(Point_set_ordered_path___begin___end___size)
{
  std::string pset_name("pset, Point_set_ordered_path___begin___end___size");
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
  std::string prop_name("prop, Point_set_ordered_path___begin___end___size");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);
  
  // create grid_ordered_path
  Grid_ordered_path * path = new Grid_ordered_path(pset, prop);
  BOOST_REQUIRE(path);

  // validate path
  BOOST_CHECK(path->size() == pset->size());
  BOOST_CHECK( two_geovalue_sets_equal(path->begin(), path->end(), pset->begin(), pset->end()) );

  // free memory
  delete path;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
}


BOOST_AUTO_TEST_CASE(Point_set_ordered_path___node_id___geovalue)
{
  std::string pset_name("pset, Point_set_ordered_path___node_id___geovalue");
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
  std::string prop_name("prop, Point_set_ordered_path___node_id___geovalue");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);
  
  // create grid_ordered_path
  Grid_ordered_path * path = new Grid_ordered_path(pset, prop);
  BOOST_REQUIRE(path);

  // validate path
  BOOST_CHECK(path->size() == pset->size());
  for (int i = 0; i < path->size(); i++) {
    GsTLInt node_id = path->node_id(i);
    Geovalue geo = path->geovalue(i);
    BOOST_CHECK( node_id == geo.node_id() );
  }

  // free memory
  delete path;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
}


BOOST_AUTO_TEST_CASE(Point_set_ordered_path___4threads)
{
  std::string pset_name("pset, Point_set_ordered_path___4threads");
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
  std::string prop_name("prop, Point_set_ordered_path___4threads");
  Grid_continuous_property * prop = pset->add_property(prop_name);
  BOOST_REQUIRE(prop);
  pset->select_property(prop_name);
  
  // create grid_ordered_path
  Grid_ordered_path * path = new Grid_ordered_path(pset, prop);
  BOOST_REQUIRE(path);

  // init parallel tasks (threads)
  const int num_tasks = 4;
  Grid_path_task<Grid_ordered_path::iterator>::Ptr tasks[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    tasks[i] = Grid_path_task<Grid_ordered_path::iterator>::Ptr_new();
    tasks[i]->grid_path_is(path);
  }
  Computational_task_manager::Ptr task_manager = Computational_task_manager::manager();
  Computational_task::Task_handler * task_handlers[num_tasks];

  // run tasks
  for (int i = 0; i < num_tasks; i++) {
    task_handlers[i] = const_cast<Computational_task::Task_handler *>( task_manager->task_add(tasks[i]) );
  }
  // wait for all parallel tasks to finish
  for (int i = 0; i < num_tasks; i++) {
    task_manager->task_join( *(task_handlers[i]) );
  }
  // retrieve result from tasks
  std::vector<Grid_ordered_path::iterator> nodes[num_tasks];
  std::vector<GsTLInt> node_ids[num_tasks];
  std::vector<Geovalue> geovalues[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    nodes[i] = tasks[i]->nodes();
    node_ids[i] = tasks[i]->node_ids();
    geovalues[i] = tasks[i]->geovalues();
  }
  
  // Validate results
  for (int i = 0; i < num_tasks; i++) {
    BOOST_CHECK( nodes[i].size() == pset->size() );
    BOOST_CHECK( node_ids[i].size() == pset->size() );
    BOOST_CHECK( geovalues[i].size() == pset->size() );
  }
  for (int i = 1; i < num_tasks; i++) {
    BOOST_CHECK( two_lists_equal(nodes[i].begin(), nodes[i].end(), nodes[0].begin(), nodes[0].end()) );
  }

  // free memory
  delete path;
  pset->remove_property(prop->name());
  delete_grid(pset->name());
  for (int i = 0; i < num_tasks; i++) {
    delete task_handlers[i];
  }
}
