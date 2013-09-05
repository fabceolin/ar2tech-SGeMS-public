add_library(ar2gems_geostat
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
)

if(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)
   # Required for platforms like Linux that don't set PYTHON_DEBUG_LIBRARIES or 
   # on windows to use the release python lin in windows
  set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)

target_link_libraries(ar2gems_geostat
  ${QT_LIBRARIES}
  ${VTK_LIBRARIES}
  optimized ${PYTHON_LIBRARIES}
  debug ${PYTHON_DEBUG_LIBRARIES} 
  ar2gems_appli
  ar2gems_grid
  ar2gems_math
  ar2gems_utils
)

if (WIN32)
  add_definitions(-DGEOSTAT_EXPORTS)
endif()

INSTALL(TARGETS ar2gems_geostat DESTINATION "${INSTALL_DIR_LIB}")
