cmake_minimum_required(VERSION 2.8)

project(ar2gems)

######################################################################
# Please set the variables with your preferences. The defaults should
# work out of the box.
######################################################################

set(CMAKE_BUILD_TYPE Debug) # Debug, Release, RelWithDebInfo or MinSizeRel

if (APPLE)
     # ...
elseif (UNIX)
    SET(INSTALL_DIR_BIN "/usr/bin")
    SET(INSTALL_DIR_LIB "/usr/lib")
elseif (WIN32)
	SET(PATH_EXTERNALS "E:/workspace/ar2externals")
	
	SET(BOOST_ROOT "${PATH_EXTERNALS}/Boost/win64/release-V1.49.0-VC2010")
	#SET(VTK_DIR "${PATH_EXTERNALS}/VTK/win64/debug-V6.1.0_master-VC2010")

# Don't rename the Qt installation path after you install Qt. This is a well known way to have problems. If you want Qt in C:/Qt/QtSDL then INSTALL it in that path or build it in that path.
# There are various ways to mess around with a qt.conf file that may help out but the path to the Qt installation is hard coded in ALL the qt libraries and executables.
# http://www.cmake.org/pipermail/cmake/2012-September/051925.html
	#SET(ENV{QTDIR} "${PATH_EXTERNALS}/Qt/win64/release-VC2010")
	#SET(QT_BINARY_DIR  "${PATH_EXTERNALS}/Qt/win64/release-VC2010/bin")
	#SET(QT_LIBRARY_DIR "${PATH_EXTERNALS}/Qt/win64/release-VC2010/lib")
	#SET(QT_QMAKE_EXECUTABLE  "${QT_BINARY_DIR}/qmake.exe")	
		
#
	#SET(PYTHON_DEBUG_LIBRARY "${PATH_EXTERNALS}/Python/win64/release-V2.7.5-VC2010/libs")	
	#SET(PYTHON_LIBRARY       "${PATH_EXTERNALS}/Python/win64/release-V2.7.5-VC2010/libs")	
	#SET(PYTHON_INCLUDE_DIR   "${PATH_EXTERNALS}/Python/win64/release-V2.7.5-VC2010/include")	
		
    SET(INSTALL_DIR_BIN "../ar2gems_package")
    SET(INSTALL_DIR_LIB "../ar2gems_package")
endif (APPLE)

if (APPLE)
    SET(INSTALL_DIR_BIN "/usr/local/bin")
    SET(INSTALL_DIR_LIB "/usr/local/lib")
elseif (UNIX)
    SET(INSTALL_DIR_BIN "/usr/bin")
    SET(INSTALL_DIR_LIB "/usr/lib")
elseif (WIN32)
     # ...
endif (APPLE)

### BOOST SETTINGS ###################################################
#FIND_PACKAGE( Boost REQUIRED )
#set(BOOST_ROOT "c:/code-dev/boost_1_48_0")
# Uncomment if Boost isn't in your path:
#set(Boost_NO_SYSTEM_PATHS TRUE)
#set(BOOST_ROOT /opt/boost)

### VTK SETTINGS #####################################################

#FIND_PACKAGE( VTK 6.1 REQUIRED )
#include(${VTK_USE_FILE})
# Uncomment if VTK isn't in your path:
#set(VTK_DIR /opt/vtk/lib/cmake/vtk-6.0)

######################################################################

add_subdirectory(ar2gems)       
                           
if (APPLE)
    # ...
elseif (UNIX)
    # create DEB package
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alex Santo <asanto@stereoofusion.com>") #required
    
    SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/about.txt")
    #file(READ ${CPACK_PACKAGE_DESCRIPTION_FILE} CPACK_PACKAGE_DESCRIPTION_SUMMARY) 
    SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE-ar2tech.txt")
    
    SET(CPACK_PACKAGE_VERSION_MAJOR "1")
    SET(CPACK_PACKAGE_VERSION_MINOR "0")
    SET(CPACK_PACKAGE_VERSION_PATCH "0")
    
    SET(CPACK_SYSTEM_NAME "amd64")
    
    # autogenerate dependency information
    set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)  # package d-shlibs should be installed
    
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "vtk (>= 6.1), libboost-system1.49.0 (>= 1.49.0)")
    
    INCLUDE(CPack)  
elseif (WIN32)
	set (CPACK_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    set (CPACK_GENERATOR "NSIS")
    INCLUDE(CPack)
endif (APPLE)


