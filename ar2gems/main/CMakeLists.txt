QT4_ADD_RESOURCES(main_RESOURCES
  ../gui/resources.qrc
  ../qtplugins/resources.qrc
)

add_executable(ar2gems
  lib_initializer.cpp
  main.cpp
  ${main_RESOURCES}
)

if(NOT PYTHON_DEBUG_LIBRARIES)
   # Required for platforms like Linux that don't set PYTHON_DEBUG_LIBRARIES
  set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif(NOT PYTHON_DEBUG_LIBRARIES)

target_link_libraries(ar2gems
  #general ${PYTHON_LIBRARIES}          # this cause linking release and debug version of python in debug build type of ar2gems
  optimized ${PYTHON_LIBRARIES}
  debug ${PYTHON_DEBUG_LIBRARIES}  
  ${QT_LIBRARIES}
  ${VTK_LIBRARIES}
#  ${PYTHON_LIBRARIES}
  ar2gems_actions
  ar2gems_appli
  ar2gems_charts
  ar2gems_extragui
  ar2gems_filters
  ar2gems_geostat
  ar2gems_grid
  ar2gems_gui
  ar2gems_math
  ar2gems_widgets
  ar2gems_utils
)

INSTALL(TARGETS ar2gems DESTINATION "${INSTALL_DIR_BIN}")
