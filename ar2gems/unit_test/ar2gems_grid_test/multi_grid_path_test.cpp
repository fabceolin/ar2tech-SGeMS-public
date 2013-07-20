#include <boost/test/unit_test.hpp>

#include "../utils.h"



/**************************************************
 *  Multi_grid_path
 **************************************************/
BOOST_AUTO_TEST_CASE(Multi_grid_path___begin___end___size)
{
  std::string grid_name("grid, Multi_grid_path___begin___end___size");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  const int grid_size = 6*6*6;
  grid->set_dimensions(6, 6, 6);
  
  // add property to grid
  std::string prop_name("prop, Multi_grid_path___begin___end___size");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create rgrid_random_path
  Multi_grid_path * path = new Multi_grid_path(grid, prop);
  BOOST_REQUIRE(path);

  // validate path
  // multigrid_level = 1 (by default)
  BOOST_CHECK(path->size() == grid_size);
  BOOST_CHECK( two_geovalue_sets_equal(path->begin(), path->end(), grid->begin(), grid->end()) );
  // multigrid_level = 1
  int multigrid_level = 1;
  path->multigrid_level_is(multigrid_level);
  BOOST_CHECK(path->size() == grid_size);
  BOOST_CHECK( two_geovalue_sets_equal(path->begin(), path->end(), grid->begin(), grid->end()) );
  // multigrid_level = 2
  multigrid_level = 2;
  path->multigrid_level_is(multigrid_level);
  BOOST_CHECK(path->size() == 3*3*3); // 3 nodes in each dimension
  std::vector<Geovalue> validated_path;
  int multigrid_spacing = pow((double)2, (double)(multigrid_level - 1));
  for (int z = 0; z < grid->nz(); z += multigrid_spacing)
    for (int y = 0; y < grid->ny(); y += multigrid_spacing)
      for (int x = 0; x < grid->nx(); x += multigrid_spacing)
        validated_path.push_back( grid->geovalue(x, y, z) );
  BOOST_CHECK( !two_geovalue_sets_equal(path->begin(), path->end(), validated_path.begin(), validated_path.end()) );
  BOOST_CHECK( !two_geovalue_sets_equal(path->begin(), path->end(), grid->begin(), grid->end()) ); // path and grid are different now
  // multigrid_level = 3
  multigrid_level = 3;
  path->multigrid_level_is(multigrid_level);
  BOOST_CHECK(path->size() == 2*2*2); // 2 nodes in each dimension
  validated_path.clear();
  multigrid_spacing = pow((double)2, (double)(multigrid_level - 1));
  for (int z = 0; z < grid->nz(); z += multigrid_spacing)
    for (int y = 0; y < grid->ny(); y += multigrid_spacing)
      for (int x = 0; x < grid->nx(); x += multigrid_spacing)
        validated_path.push_back( grid->geovalue(x, y, z) );
  BOOST_CHECK( !two_geovalue_sets_equal(path->begin(), path->end(), validated_path.begin(), validated_path.end()) );
  BOOST_CHECK( !two_geovalue_sets_equal(path->begin(), path->end(), grid->begin(), grid->end()) ); // path and grid are different now

  // free memory
  delete path;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
}


BOOST_AUTO_TEST_CASE(Multi_grid_path___node_id___geovalue)
{
  std::string grid_name("grid, Multi_grid_path___node_id___geovalue");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  const int grid_size = 6*6*6;
  grid->set_dimensions(6, 6, 6);
  SGrid_cursor cursor(*grid->cursor());
  
  // add property to grid
  std::string prop_name("prop, Multi_grid_path___node_id___geovalue");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create rgrid_random_path
  Multi_grid_path * path = new Multi_grid_path(grid, prop);
  BOOST_REQUIRE(path);

  // validate path
  // multigrid_level = 1
  BOOST_CHECK(path->size() == grid_size);
  for (int i = 0; i < path->size(); i++) {
    GsTLInt node_id = path->node_id(i);
    Geovalue geo = path->geovalue(i);
    BOOST_CHECK( node_id == geo.node_id() );
  }
  // multigrid_level = 2
  int multigrid_level = 2;
  path->multigrid_level_is(multigrid_level);
  for (int i = 0; i < path->size(); i++) {
    GsTLInt node_id = path->node_id(i);
    Geovalue geo = path->geovalue(i);
    BOOST_CHECK( node_id == geo.node_id() );
  }
  // multigrid_level = 3
  multigrid_level = 3;
  path->multigrid_level_is(multigrid_level);
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


BOOST_AUTO_TEST_CASE(Multi_grid_path___4threads)
{
  std::string grid_name("grid, Multi_grid_path___4threads");
  Cartesian_grid* grid = cartesian_grid(grid_name);
  BOOST_REQUIRE(grid);
  const int grid_size = 6*6*6;
  grid->set_dimensions(6, 6, 6);
  
  // add property to grid
  std::string prop_name("prop, Multi_grid_path___4threads");
  Grid_continuous_property * prop = grid->add_property(prop_name);
  BOOST_REQUIRE(prop);
  grid->select_property(prop_name);
  
  // create rgrid_random_path
  Multi_grid_path * path = new Multi_grid_path(grid, prop);
  BOOST_REQUIRE(path);

  // init parallel tasks (threads)
  const int num_tasks = 4;
  Grid_path_task<Multi_grid_path::iterator>::Ptr tasks[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    tasks[i] = Grid_path_task<Multi_grid_path::iterator>::Ptr_new();
    tasks[i]->grid_path_is(path);
  }
  Computational_task_manager::Ptr task_manager = Computational_task_manager::manager();
  Computational_task::Task_handler * task_handlers[num_tasks];

  /* multigrid_level = 1 */
  // run tasks
  for (int i = 0; i < num_tasks; i++) {
    task_handlers[i] = const_cast<Computational_task::Task_handler *>( task_manager->task_add(tasks[i]) );
  }
  // wait for all parallel tasks to finish
  for (int i = 0; i < num_tasks; i++) {
    task_manager->task_join( *(task_handlers[i]) );
  }
  // retrieve result from tasks
  std::vector<Multi_grid_path::iterator> nodes[num_tasks];
  std::vector<GsTLInt> node_ids[num_tasks];
  std::vector<Geovalue> geovalues[num_tasks];
  for (int i = 0; i < num_tasks; i++) {
    nodes[i] = tasks[i]->nodes();
    node_ids[i] = tasks[i]->node_ids();
    geovalues[i] = tasks[i]->geovalues();
  }
  // validate results
  int path_size = grid_size;
  for (int i = 0; i < num_tasks; i++) {
    BOOST_CHECK( nodes[i].size() == path_size );
    BOOST_CHECK( node_ids[i].size() == path_size );
    BOOST_CHECK( geovalues[i].size() == path_size );
  }
  for (int i = 1; i < num_tasks; i++) {
    BOOST_CHECK( two_lists_equal(nodes[i].begin(), nodes[i].end(), nodes[0].begin(), nodes[0].end()) );
  }
  // free task_handers
  for (int i = 0; i < num_tasks; i++) {
    delete task_handlers[i];
  }

  /* multigrid_level = 2 */
  int multigrid_level = 2;
  path->multigrid_level_is(multigrid_level);
  // run tasks
  for (int i = 0; i < num_tasks; i++) {
    task_handlers[i] = const_cast<Computational_task::Task_handler *>( task_manager->task_add(tasks[i]) );
  }
  // wait for all parallel tasks to finish
  for (int i = 0; i < num_tasks; i++) {
    task_manager->task_join( *(task_handlers[i]) );
  }
  // retrieve result from tasks
  for (int i = 0; i < num_tasks; i++) {
    nodes[i] = tasks[i]->nodes();
    node_ids[i] = tasks[i]->node_ids();
    geovalues[i] = tasks[i]->geovalues();
  }
  // validate results
  path_size = 3*3*3; // 3 nodes in each dimension
  for (int i = 0; i < num_tasks; i++) {
    BOOST_CHECK( nodes[i].size() == path_size );
    BOOST_CHECK( node_ids[i].size() == path_size );
    BOOST_CHECK( geovalues[i].size() == path_size );
  }
  for (int i = 1; i < num_tasks; i++) {
    BOOST_CHECK( two_lists_equal(nodes[i].begin(), nodes[i].end(), nodes[0].begin(), nodes[0].end()) );
  }
  // free task_handers
  for (int i = 0; i < num_tasks; i++) {
    delete task_handlers[i];
  }

  /* multigrid_level = 3 */
  multigrid_level = 3;
  path->multigrid_level_is(multigrid_level);
  // run tasks
  for (int i = 0; i < num_tasks; i++) {
    task_handlers[i] = const_cast<Computational_task::Task_handler *>( task_manager->task_add(tasks[i]) );
  }
  // wait for all parallel tasks to finish
  for (int i = 0; i < num_tasks; i++) {
    task_manager->task_join( *(task_handlers[i]) );
  }
  // retrieve result from tasks
  for (int i = 0; i < num_tasks; i++) {
    nodes[i] = tasks[i]->nodes();
    node_ids[i] = tasks[i]->node_ids();
    geovalues[i] = tasks[i]->geovalues();
  }
  // validate results
  path_size = 2*2*2; // 2 nodes in each dimension
  for (int i = 0; i < num_tasks; i++) {
    BOOST_CHECK( nodes[i].size() == path_size );
    BOOST_CHECK( node_ids[i].size() == path_size );
    BOOST_CHECK( geovalues[i].size() == path_size );
  }
  for (int i = 1; i < num_tasks; i++) {
    BOOST_CHECK( two_lists_equal(nodes[i].begin(), nodes[i].end(), nodes[0].begin(), nodes[0].end()) );
  }
  // free task_handers
  for (int i = 0; i < num_tasks; i++) {
    delete task_handlers[i];
  }

  // free memory
  delete path;
  grid->remove_property(prop->name());
  delete_grid(grid->name());
}
