# Process all headers with Q_OBJECT
QT4_WRAP_CPP(utils_MOC
  gstl_item_model.h
)



add_library(ar2gems_utils
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

target_link_libraries(ar2gems_utils
  ${QT_LIBRARIES}
  ar2gems_qwt
)

if (WIN32)
 add_definitions(-DUTILS_EXPORTS)
endif()

INSTALL(TARGETS ar2gems_utils DESTINATION "${INSTALL_DIR_LIB}")
