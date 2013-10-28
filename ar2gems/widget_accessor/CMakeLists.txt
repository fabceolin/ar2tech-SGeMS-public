if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(LIB_NAME ar2gems_extragui)
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(LIB_NAME ar2gems_extragui_d)
endif ()

add_library(${LIB_NAME}
  library_extragui_init.cpp
  qt_accessors.cpp
  qt_accessors.h  
)

if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_link_libraries(${LIB_NAME}
      ${QT_LIBRARIES}
      ar2gems_appli
      ar2gems_gui
      ar2gems_widgets
      ar2gems_utils
    )
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_link_libraries(${LIB_NAME}
      ${QT_LIBRARIES}
      ar2gems_appli_d
      ar2gems_gui_d
      ar2gems_widgets_d
      ar2gems_utils_d
    )
endif ()




if (WIN32)
  add_definitions(-DEXTRAGUI_EXPORTS)
endif()

INSTALL(TARGETS ${LIB_NAME} DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
