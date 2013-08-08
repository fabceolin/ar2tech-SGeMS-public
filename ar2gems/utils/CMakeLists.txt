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
)

target_link_libraries(ar2gems_utils
  ${QT_LIBRARIES}
  ar2gems_qwt
)