add_library(ar2gems_extragui
  library_extragui_init.cpp
  qt_accessors.cpp
  qt_accessors.h  
)

target_link_libraries(ar2gems_extragui
  ${QT_LIBRARIES}
  ar2gems_appli
  ar2gems_gui
  ar2gems_widgets
  ar2gems_utils
)


if (WIN32)
  add_definitions(-DEXTRAGUI_EXPORTS)
endif()

INSTALL(TARGETS ar2gems_extragui DESTINATION "${INSTALL_DIR_LIB}")
