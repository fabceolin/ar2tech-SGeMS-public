# Process all headers with Q_OBJECT
QT4_WRAP_CPP(appli_MOC
  project.h
  root_model.h
)

if (CMAKE_BUILD_TYPE MATCHES  "Release")
    set(LIB_NAME ar2gems_appli)
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    set(LIB_NAME ar2gems_appli_d)
endif ()

add_library(${LIB_NAME}
  library_appli_init.cpp
  project.cpp
  root_model.cpp
  utilities.cpp
  project.h
  root_model.h
  utilities.h  
  ${appli_MOC}
)

SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION moc_.*cxx$)
SOURCE_GROUP("Uic Files" REGULAR_EXPRESSION ui_.*h)

if (CMAKE_BUILD_TYPE MATCHES  "Release")
    target_link_libraries(${LIB_NAME}
      ar2gems_utils
    )
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    target_link_libraries(${LIB_NAME}
      ar2gems_utils_d
    )
endif ()


if (WIN32)
  add_definitions(-DAPPLI_EXPORTS)
endif()

INSTALL(TARGETS ${LIB_NAME} DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
