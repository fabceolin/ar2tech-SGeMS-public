# Process all headers with Q_OBJECT
QT4_WRAP_CPP(filters_MOC
  csv_filter_qt_dialogs.h
  gslib/filter_qt_dialogs.h
)

QT4_WRAP_UI(filters_FORMS
  csv_masked_import.ui
  csv_pointset_import.ui
  csv_grid_import.ui
  gslib/gslibgridimport.ui
  gslib/gslibmaskedimport.ui
  gslib/gslibimportwizard.ui
  gslib/gslib_pointset_import.ui
  gslib/importwizard.ui
)

if (CMAKE_BUILD_TYPE MATCHES  "Release")
    set(LIB_NAME ar2gems_filters)
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    set(LIB_NAME ar2gems_filters_d)
endif ()

add_library(${LIB_NAME}
  csv_filter.cpp
  csv_filter_qt_dialogs.cpp
  distribution_filter.cpp
  gslib/filter_qt_dialogs.cpp
  gslib/gslib_filter.cpp
  io_actions.cpp
  library_filters_init.cpp
  save_project_objects.cpp
  sgems_folder_filter.cpp
  sgems_folder_grid_geometry.cpp
  simulacre_filter.cpp
  csv_filter.h
  csv_filter_qt_dialogs.h
  distribution_filter.h
  gslib/filter_qt_dialogs.h
  gslib/gslib_filter.h
  io_actions.h
  library_filters_init.h
  save_project_objects.h
  sgems_folder_filter.h
  sgems_folder_grid_geometry.h
  simulacre_filter.h  
  ${filters_MOC}
  ${filters_FORMS}
)

SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION moc_.*cxx$)
SOURCE_GROUP("Uic Files" REGULAR_EXPRESSION ui_.*h)

<<<<<<< HEAD
target_link_libraries(ar2gems_filters
=======
if (CMAKE_BUILD_TYPE MATCHES  "Release")
    target_link_libraries(${LIB_NAME}
>>>>>>> dev-cmake-debug-release-key
      ${QT_LIBRARIES}
      ar2gems_appli
      ar2gems_grid
      ar2gems_math
      ar2gems_utils
    )
<<<<<<< HEAD
set_target_properties( ar2gems_filters PROPERTIES OUTPUT_NAME_RELEASE ar2gems_filters )
set_target_properties( ar2gems_filters PROPERTIES OUTPUT_NAME_DEBUG   ar2gems_filters_d )
=======
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    target_link_libraries(${LIB_NAME}
      ${QT_LIBRARIES}
      ar2gems_appli_d
      ar2gems_grid_d
      ar2gems_math_d
      ar2gems_utils_d
    )
endif ()


>>>>>>> dev-cmake-debug-release-key

if (WIN32)
  add_definitions(-DFILTERS_EXPORTS)
endif()

        
<<<<<<< HEAD
INSTALL(TARGETS ar2gems_filters DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
=======
INSTALL(TARGETS ${LIB_NAME} DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
>>>>>>> dev-cmake-debug-release-key
