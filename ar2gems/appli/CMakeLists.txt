# Process all headers with Q_OBJECT
QT4_WRAP_CPP(appli_MOC
  project.h
  root_model.h
)

add_library(ar2gems_appli
  library_appli_init.cpp
  project.cpp
  root_model.cpp
  utilities.cpp
  ${appli_MOC}
)

target_link_libraries(ar2gems_appli
  ar2gems_utils
)
