if (CMAKE_BUILD_TYPE MATCHES  "Release")
    set(LIB_NAME ar2gems_math)
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    set(LIB_NAME ar2gems_math_d)
endif ()

add_library(${LIB_NAME}
  Linear_interpolator_1d.cpp
  box.cpp
  categorical_distribution.cpp
  continuous_distribution.cpp
  correlation_measure.cpp
  correlation_measure_computer.cpp
  direction_3d.cpp
  discrete_function.cpp
  gstlappli_math_init.cpp
  histogram.cpp
  library_math_init.cpp
  non_parametric_distribution.cpp
  qpplot.cpp
  random_numbers.cpp
  scatterplot.cpp
  
  Linear_interpolator_1d.h
  box.h
  categorical_distribution.h
  continuous_distribution.h
  correlation_measure.h
  correlation_measure_computer.h
  direction_3d.h
  discrete_function.h
  histogram.h
  library_math_init.h
  non_parametric_distribution.h
  qpplot.h
  random_numbers.h
  scatterplot.h  
)


<<<<<<< HEAD
target_link_libraries(ar2gems_math
        ar2gems_utils
    )
set_target_properties( ar2gems_math PROPERTIES OUTPUT_NAME_RELEASE ar2gems_math )
set_target_properties( ar2gems_math PROPERTIES OUTPUT_NAME_DEBUG   ar2gems_math_d )
=======
if (CMAKE_BUILD_TYPE MATCHES  "Release")
    target_link_libraries(${LIB_NAME}
        ar2gems_utils
    )
elseif (CMAKE_BUILD_TYPE MATCHES  "Debug")
    target_link_libraries(${LIB_NAME}
        ar2gems_utils_d
)
endif ()
>>>>>>> dev-cmake-debug-release-key
      


if (WIN32)
  add_definitions(-DMATH_EXPORTS)
endif()

INSTALL(TARGETS ${LIB_NAME} DESTINATION "${INSTALL_DIR_LIB}" COMPONENT application)
