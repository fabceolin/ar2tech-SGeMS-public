QT4_ADD_RESOURCES(main_com_RESOURCES
  ../gui/resources.qrc
  ../qtplugins/resources.qrc
)

#This will generate the console version for windows
add_executable(ar2gems_com
  lib_initializer.cpp
  main.cpp
  lib_initializer.h
  ${main_RESOURCES}
)

SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION moc_.*cxx$)
SOURCE_GROUP("Uic Files" REGULAR_EXPRESSION ui_.*h)

if(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)
   # Required for platforms like Linux that don't set PYTHON_DEBUG_LIBRARIES or 
   # on windows to use the release python in windows
  set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)

target_link_libraries(ar2gems_com
  optimized ${PYTHON_LIBRARIES}
  debug ${PYTHON_DEBUG_LIBRARIES}  
  ${QT_LIBRARIES}
  ${VTK_LIBRARIES}
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

if (WIN32)
  if(RELEASE_PYTHON_IN_DEBUG)
    add_definitions(-DRELEASE_PYTHON_IN_DEBUG)
  endif()  
  add_definitions(-DGSTLAPPLI_SILENT)
  
endif()

INSTALL(TARGETS ar2gems_com DESTINATION "${INSTALL_DIR_BIN}")
