QT4_ADD_RESOURCES(main_RESOURCES
  ../gui/resources.qrc
  ../qtplugins/resources.qrc
)

add_executable(ar2gems
  lib_initializer.cpp
  main.cpp
  ${main_RESOURCES}
)

target_link_libraries(ar2gems
  ${PYTHON_LIBRARIES}
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
  ar2gems_qtplugins
  ar2gems_utils
)
