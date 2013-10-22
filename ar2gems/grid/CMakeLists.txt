if (CMAKE_BUILD_TYPE MATCHES  "Release")
    set(LIB_NAME ar2gems_grid)
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    set(LIB_NAME ar2gems_grid_d)
endif ()

add_library(${LIB_NAME}
  cartesian_grid.cpp
  combined_neighborhood.cpp
  coordinate_mapper.cpp
  cross_variog_computer.cpp
  distribution_utilities.cpp
  geostat_grid.cpp
  geovalue.cpp
  grid_categorical_property.cpp
  grid_downscaler.cpp
  grid_filter.cpp
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
  rgrid_geometry.cpp
  rgrid_gval_accessor.cpp
  rgrid_neighborhood.cpp
  structured_grid.cpp
  superblock.cpp
  utilities.cpp
  
  cartesian_grid.h
  combined_neighborhood.h
  coordinate_mapper.h
  cross_variog_computer.h
  distribution_utilities.h
  geostat_grid.h
  geovalue.h
  grid_categorical_property.h
  grid_downscaler.h
  grid_filter.h
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
  rgrid_gval_accessor.h
  rgrid_neighborhood.h
  structured_grid.h
  superblock.h
  utilities.h  
)

if (CMAKE_BUILD_TYPE MATCHES  "Release")
    target_link_libraries(${LIB_NAME}
      ${VTK_LIBRARIES}
      ${QT_LIBRARIES}
      ar2gems_appli
      ar2gems_math
      ar2gems_utils
    )
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    target_link_libraries(${LIB_NAME}
      ${VTK_LIBRARIES}
      ${QT_LIBRARIES}
      ar2gems_appli_d
      ar2gems_math_d
      ar2gems_utils_d
    )
endif ()

if (WIN32)
  add_definitions(-DGRID_EXPORTS)
endif()

         
INSTALL(TARGETS ${LIB_NAME} DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
