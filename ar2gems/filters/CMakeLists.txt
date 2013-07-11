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

add_library(ar2gems_filters
  simulacre_filter.cpp
  sgems_folder_filter.cpp
  csv_filter_qt_dialogs.cpp
  save_project_objects.cpp
  csv_filter.cpp
  library_filters_init.cpp
  sgems_folder_grid_geometry.cpp
  gslib/filter_qt_dialogs.cpp
  gslib/gslib_filter.cpp
  distribution_filter.cpp
  ${filters_MOC}
  ${filters_FORMS}
)

target_link_libraries(ar2gems_filters
  ${QT_LIBRARIES}
  ar2gems_actions
  ar2gems_appli
  ar2gems_grid
  ar2gems_utils
)
