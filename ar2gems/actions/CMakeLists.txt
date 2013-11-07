add_library(ar2gems_actions
  add_property_to_grid.cpp
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
  
  add_property_to_grid.h
  algorithms_actions.h
  categorical_definition_actions.h
  categorical_property_actions.h
  distribution_action.h
  library_actions_init.h
  maskedgrid_actions.h
  misc_actions.h
  obj_manag_actions.h
  property_group_actions.h
  property_transformer_actions.h
  python_commands.h
  python_group_script.h
  python_script.h
  python_wrapper.h
  unary_action.h  
)

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
  if(RELEASE_PYTHON_IN_DEBUG)
    add_definitions(-DRELEASE_PYTHON_IN_DEBUG)
  endif()
 endif()

INSTALL(TARGETS ar2gems_actions DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)


