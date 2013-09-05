add_library(ar2gems_actions
  algorithms_actions.cpp
  categorical_definition_actions.cpp
  categorical_property_actions.cpp
  distribution_action.cpp
  library_actions_init.cpp
  maskedgrid_actions.cpp
  misc_actions.cpp
  obj_manag_actions.cpp
  property_group_actions.cpp
  property_transformer_actions.cpp
  python_commands.cpp
  python_group_script.cpp
  python_script.cpp
  python_wrapper.cpp
  unary_action.cpp
)

#if (WIN32)
#	if(NOT PYTHON_DEBUG_LIBRARIES)
#   	 #Required for platforms like Linux that don't set PYTHON_DEBUG_LIBRARIES
#  		set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
#	endif(NOT PYTHON_DEBUG_LIBRARIES)
#endif (WIN32)

if(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)
   # Required for platforms like Linux that don't set PYTHON_DEBUG_LIBRARIES or 
   # on windows to use the release python lin in windows
  set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)

target_link_libraries(ar2gems_actions
  #general ${PYTHON_LIBRARIES}          # this cause linking release and debug version of python in debug build type of ar2gems
  optimized ${PYTHON_LIBRARIES}
  debug ${PYTHON_DEBUG_LIBRARIES}  
#  ${PYTHON_LIBRARIES} 
  ar2gems_appli
  ar2gems_filters
  ar2gems_geostat
  ar2gems_grid
  ar2gems_math
  ar2gems_utils
)

if (WIN32)
  add_definitions(-DACTIONS_EXPORTS)
 endif()

INSTALL(TARGETS ar2gems_actions DESTINATION "${INSTALL_DIR_LIB}")


