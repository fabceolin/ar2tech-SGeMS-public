# Process all headers with Q_OBJECT
QT4_WRAP_CPP(utils_MOC
  gstl_item_model.h
)

<<<<<<< HEAD
add_library(ar2gems_utils
=======
if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(LIB_NAME ar2gems_utils)
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(LIB_NAME ar2gems_utils_d)
endif ()


add_library(${LIB_NAME}
>>>>>>> dev-cmake-debug-release-key
  error_messages_handler.cpp
  gstl_messages_private.cpp
  manager.cpp
  manager_repository.cpp
  progress_notifier.cpp
  simpleps.cpp
  string_manipulation.cpp
  ${utils_MOC}
  gstl_item_model.h
  error_messages_handler.h
  gstl_messages_private.h
  manager.h
  manager_repository.h
  progress_notifier.h
  simpleps.h
  string_manipulation.h  
)

SOURCE_GROUP("Moc Files" REGULAR_EXPRESSION moc_.*cxx$)
SOURCE_GROUP("Uic Files" REGULAR_EXPRESSION ui_.*h)

<<<<<<< HEAD

target_link_libraries(ar2gems_utils
      ${QT_LIBRARIES}
      ar2gems_qwt
    )
set_target_properties( ar2gems_utils PROPERTIES OUTPUT_NAME_RELEASE ar2gems_utils )
set_target_properties( ar2gems_utils PROPERTIES OUTPUT_NAME_DEBUG   ar2gems_utils_d )
=======
if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_link_libraries(${LIB_NAME}
      ${QT_LIBRARIES}
      ar2gems_qwt
    )
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
    target_link_libraries(${LIB_NAME}
      ${QT_LIBRARIES}
      ar2gems_qwt_d
    )
endif ()
>>>>>>> dev-cmake-debug-release-key

if (WIN32)
 add_definitions(-DUTILS_EXPORTS)
endif()

INSTALL(TARGETS ${LIB_NAME} DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
