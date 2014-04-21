add_library(ar2gems_grid
  cartesian_grid.cpp
  combined_neighborhood.cpp
  coordinate_mapper.cpp
  cross_variog_computer.cpp
  distribution_utilities.cpp
  geostat_grid.cpp
  geovalue.cpp
  graph_connections.cpp
  grid_categorical_property.cpp
  grid_downscaler.cpp
  grid_filter.cpp
  grid_path.cpp
  grid_path_multigrid.cpp
  grid_property.cpp
  grid_property_manager.cpp
  grid_property_set.cpp
  grid_region_manager.cpp
  grid_weight_property.cpp
  gstl_kdtree2.cpp
  library_grid_init.cpp
  log_data_grid.cpp
  mgrid_neighborhood.cpp
  neighborhood.cpp
  neighbors.cpp
  point_set.cpp
  point_set_neighborhood.cpp
  property_copier.cpp
  property_transformer.cpp
  reduced_grid.cpp
  rgrid.cpp
  cartesian_grid_geometry.cpp
  rgrid_gval_accessor.cpp
  rgrid_neighborhood.cpp
  structured_grid.cpp
  utilities.cpp
  
  cartesian_grid.h
  combined_neighborhood.h
  coordinate_mapper.h
  cross_variog_computer.h
  distribution_utilities.h
  geostat_grid.h
  geovalue.h
  graph_connections.h
  grid_categorical_property.h
  grid_downscaler.h
  grid_filter.h
  grid_path.h
  grid_path_multigrid.h
  grid_property.h
  grid_property_manager.h
  grid_property_set.h
  grid_region_manager.h
  grid_weight_property.h
  gstl_kdtree2.h
  log_data_grid.h
  mgrid_neighborhood.h
  neighborhood.h
  neighbors.h
  point_set.h
  point_set_neighborhood.h
  property_copier.h
  property_transformer.h
  reduced_grid.h
  rgrid.h
  rgrid_geometry.h
  cartesian_grid_geometry.h
  rgrid_gval_accessor.h
  rgrid_neighborhood.h
  structured_grid.h
  utilities.h  
)

target_link_libraries(ar2gems_grid
      ${VTK_LIBRARIES}
      ${QT_LIBRARIES}
      ar2gems_appli
      ar2gems_math
      ar2gems_utils
    )
set_target_properties( ar2gems_grid PROPERTIES OUTPUT_NAME_RELEASE ar2gems_grid )
set_target_properties( ar2gems_grid PROPERTIES OUTPUT_NAME_DEBUG   ar2gems_grid_d )


if (WIN32)
  add_definitions(-DGRID_EXPORTS)
endif()

         
INSTALL(TARGETS ar2gems_grid DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
