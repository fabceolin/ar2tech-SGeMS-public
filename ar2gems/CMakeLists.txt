######################################################################
############### DON'T NEED CHANGE ANYTHING HERE ######################
######################################################################

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -O3 -march=native")
  set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -ggdb -march=native")
elseif(MSVC)
  # customize flags for Visual Studio
endif()

### Enabling Boost ###################################################
find_package(Boost REQUIRED)
if(Boost_FOUND)
  message(STATUS "Boost found in ${BOOST_ROOT}")
  include_directories(${Boost_INCLUDE_DIRS})
endif()

### Enabling Qt4 #####################################################
find_package(Qt4 REQUIRED COMPONENTS
             QtCore QtGui QtXml QtSvg QtDesigner QtUiTools)
if(QT4_FOUND)
  message(STATUS "Qt4 found in ${QT_INCLUDE_DIR}")
  include(${QT_USE_FILE})
endif()

### Enabling VTK #####################################################
find_package(VTK REQUIRED)
if(VTK_FOUND)
  message(STATUS "VTK found in ${VTK_DIR}")
  include(${VTK_USE_FILE})
endif()

### Enabling Python ##################################################
find_package(PythonLibs 2.7 REQUIRED)
if(PYTHONLIBS_FOUND)
  message(STATUS "Python found in ${PYTHON_INCLUDE_DIR}")
  include_directories(${PYTHON_INCLUDE_DIR})
endif()

### Enabling Eigen ###################################################
include_directories(${CMAKE_SOURCE_DIR})

### Enabling GsTL ####################################################
include_directories(${CMAKE_SOURCE_DIR}/ar2GsTL)

### Includes prefixed with the directory name ########################
# The convention is to prefix the subdirectory (e.g. utils/foo.h) when
# referring to headers.
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

### Project tree #####################################################
set(PROJ_DIRS
  actions appli charts filters geostat grid gui
  main math qtplugins qwt utils widget_accessor
)

foreach(dir ${PROJ_DIRS})
  add_subdirectory(${dir})
endforeach()
