QT4_ADD_RESOURCES(main_RESOURCES
  ../gui/resources.qrc
  ../qtplugins/resources.qrc
)

IF(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    add_executable(ar2gems WIN32
      lib_initializer.cpp
      main.cpp
      lib_initializer.h
      ${main_RESOURCES}
    )
  endif()
  else()
    add_executable(ar2gems
      lib_initializer.cpp
      main.cpp
      lib_initializer.h
      ${main_RESOURCES}
    )  
endif()

SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION moc_.*cxx$)
SOURCE_GROUP("Uic Files" REGULAR_EXPRESSION ui_.*h)

if(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)
   # Required for platforms like Linux that don't set PYTHON_DEBUG_LIBRARIES or 
   # on windows to use the release python lin in windows
  set(PYTHON_DEBUG_LIBRARIES ${PYTHON_LIBRARIES})
endif(NOT PYTHON_DEBUG_LIBRARIES OR RELEASE_PYTHON_IN_DEBUG)


<<<<<<< HEAD
target_link_libraries(ar2gems 
=======
if (CMAKE_BUILD_TYPE MATCHES  "Release")
    target_link_libraries(ar2gems 
>>>>>>> dev-cmake-debug-release-key
        optimized ${PYTHON_LIBRARIES}
        debug ${PYTHON_DEBUG_LIBRARIES}  
        ${QT_LIBRARIES}
        ${QT_QTMAIN_LIBRARY}  # to avoid the console windows
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
<<<<<<< HEAD
set_target_properties( ar2gems PROPERTIES OUTPUT_NAME_RELEASE ar2gems )
set_target_properties( ar2gems PROPERTIES OUTPUT_NAME_DEBUG   ar2gems_d )
=======
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    target_link_libraries(ar2gems 
        optimized ${PYTHON_LIBRARIES}
        debug ${PYTHON_DEBUG_LIBRARIES}  
        ${QT_LIBRARIES}
        ${QT_QTMAIN_LIBRARY}  # to avoid the console windows
        ${VTK_LIBRARIES}
        ar2gems_actions_d
        ar2gems_appli_d
        ar2gems_charts_d
        ar2gems_extragui_d
        ar2gems_filters_d
        ar2gems_geostat_d
        ar2gems_grid_d
        ar2gems_gui_d
        ar2gems_math_d
        ar2gems_widgets_d
        ar2gems_utils_d   
    )
endif ()
>>>>>>> dev-cmake-debug-release-key

if (WIN32)
  if(RELEASE_PYTHON_IN_DEBUG)
    add_definitions(-DRELEASE_PYTHON_IN_DEBUG)
  endif()  
endif()

INSTALL(TARGETS ar2gems DESTINATION "${INSTALL_DIR_BIN}")
