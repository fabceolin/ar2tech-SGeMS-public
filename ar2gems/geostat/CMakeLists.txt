if (CMAKE_BUILD_TYPE MATCHES  "Release")
    set(LIB_NAME ar2gems_geostat)
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    set(LIB_NAME ar2gems_geostat_d)
endif ()

add_library(${LIB_NAME}
  kriging_cross_validation.cpp
  kriging_mean.cpp
  cosisim.cpp
  trans.cpp
  moving_window.cpp
  utilities.cpp
  cosgsim.cpp
  kriging.cpp
  library_geostat_init.cpp
  dssim.cpp
  pset_variog_computer.cpp
  snesim_std/snesim_std.cpp
  snesim_std/NodeDropped.cpp
  parameters_handler_impl.cpp
  cokriging_cross_validation.cpp
  filtersim_std/TI_manipulation.cpp
  filtersim_std/mapping_pattern_id.cpp
  filtersim_std/filtersim_cont.cpp
  filtersim_std/pattern_paster.cpp
  filtersim_std/filters.cpp
  filtersim_std/partition.cpp
  filtersim_std/filtersim.cpp
  filtersim_std/filtersim_cate.cpp
  filtersim_std/patch_helper.cpp
  filtersim_std/dev_finder.cpp
  LU_sim.cpp
  nuTauModel.cpp
  kmeans_clustering.cpp
  inverse_distance_estimator.cpp
  difference_with_base.cpp
  transcat.cpp
  Postsim.cpp
  cokriging.cpp
  indicator_kriging.cpp
  Postsim_categorical.cpp
  sgsim.cpp
  ImageProcess.cpp
  declus.cpp
  clustering_from_centroids.cpp
  sisim.cpp
  PostKriging.cpp
  nearest_neighbor_estimator.cpp
  grid_variog_computer.cpp
  
  kriging_cross_validation.h
  kriging_mean.h
  cosisim.h
  trans.h
  moving_window.h
  utilities.h
  cosgsim.h
  kriging.h
  dssim.h
  pset_variog_computer.h
  snesim_std/snesim_std.h
  snesim_std/NodeDropped.h
  parameters_handler_impl.h
  cokriging_cross_validation.h
  filtersim_std/TI_manipulation.h
  filtersim_std/mapping_pattern_id.h
  filtersim_std/filtersim_cont.h
  filtersim_std/pattern_paster.h
  filtersim_std/filters.h
  filtersim_std/partition.h
  filtersim_std/filtersim.h
  filtersim_std/filtersim_cate.h
  filtersim_std/patch_helper.h
  filtersim_std/dev_finder.h
  LU_sim.h
  nuTauModel.h
  kmeans_clustering.h
  inverse_distance_estimator.h
  difference_with_base.h
  transcat.h
  Postsim.h
  cokriging.h
  indicator_kriging.h
  Postsim_categorical.h
  sgsim.h
  ImageProcess.h
  declus.h
  clustering_from_centroids.h
  sisim.h
  PostKriging.h
  nearest_neighbor_estimator.h
  grid_variog_computer.h  
)


if(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)
   # Required for platforms like Linux that don't set PYTHON_DEBUG_LIBRARIES or 
   # on windows to use the release python lin in windows
  set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)

if (CMAKE_BUILD_TYPE MATCHES  "Release")
    target_link_libraries(${LIB_NAME}
      ${QT_LIBRARIES}
      ${VTK_LIBRARIES}
      optimized ${PYTHON_LIBRARIES}
      debug ${PYTHON_DEBUG_LIBRARIES} 
      ar2gems_appli
      ar2gems_grid
      ar2gems_math
      ar2gems_utils
    )
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    target_link_libraries(${LIB_NAME}
      ${QT_LIBRARIES}
      ${VTK_LIBRARIES}
      optimized ${PYTHON_LIBRARIES}
      debug ${PYTHON_DEBUG_LIBRARIES} 
      ar2gems_appli_d
      ar2gems_grid_d
      ar2gems_math_d
      ar2gems_utils_d
    )
endif ()


if (WIN32)
  add_definitions(-DGEOSTAT_EXPORTS)
endif()

         
INSTALL(TARGETS ${LIB_NAME} DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
