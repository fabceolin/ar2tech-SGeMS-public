add_library(ar2gems_extragui
  library_extragui_init.cpp
  qt_accessors.cpp
)

target_link_libraries(ar2gems_extragui
  ar2gems_appli
  ar2gems_gui
  ar2gems_qtplugins
  ar2gems_utils
)
