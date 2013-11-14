cmake_minimum_required(VERSION 2.8)

project(ar2gems)

######################################################################
# Please set the variables with your preferences. The defaults should
# work out of the box.
######################################################################

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING
       "Choose the type of build, options are: Debug Release"
       FORCE )
endif()

set(ALLOW_BUILD_HEADERS 0 CACHE TYPE BOOL)
set(ALLOW_BUILD_PACKAGE 0 CACHE TYPE BOOL)

# SET RELEVANT VARS FOR PACKAGE
if (ALLOW_BUILD_PACKAGE)
	if (APPLE)
	elseif (UNIX)
	    SET(PACKAGE_TYPE "DEB")    # DEB or RPM
	endif (APPLE)
	
	SET(PACKAGE_MAINTAINER "Alex Santo <asanto@stereoofusion.com>")
	#SET(PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/about.txt")
	SET(PACKAGE_LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE-ar2tech.txt")
	    
	SET(PACKAGE_VERSION_MAJOR "1")
	SET(PACKAGE_VERSION_MINOR "0")
	SET(PACKAGE_VERSION_PATCH "0")
	    
	SET(PACKAGE_SYSTEM_NAME "amd64")
endif (ALLOW_BUILD_PACKAGE)
    
    
if (APPLE)
    set(INSTALL_DIR_BIN "/usr/local/bin")
    set(INSTALL_DIR_LIB "/usr/local/lib")
    set(INSTALL_DIR_PLUGINS "/usr/local/bin")
    set(INSTALL_DIR_HEADERS "/usr/local/include")
elseif (UNIX)
    set(INSTALL_DIR_BIN "/usr/bin")
    set(INSTALL_DIR_LIB "/usr/lib")
    SET(INSTALL_DIR_PLUGINS "/usr/bin")
    set(INSTALL_DIR_HEADERS "/usr/include")
elseif (WIN32)
    set(INSTALL_DIR_BIN "../ar2gems_package")
    set(INSTALL_DIR_LIB "../ar2gems_package")
    set(INSTALL_DIR_PLUGINS "../ar2gems_package")
	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}") # this is needed for proper headers path install. The WIn doesn't support abs path in install()
    set(INSTALL_DIR_HEADERS "include")
    	
	set(PATH_EXTERNALS "E:/workspace/ar2externals")
    	
	SET(BOOST_ROOT "d:/release-V1.49.0-VC2010" CACHE TYPE STRING)

	#SET(VTK_DIR "${PATH_EXTERNALS}/VTK/win64/debug-V6.1.0_master-VC2010")
	SET(VTK_DIR "d:/workspace/VTK-master/build" CACHE TYPE STRING)
    
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
    
# CREATE PACKAGE    
if (ALLOW_BUILD_PACKAGE)
	if (APPLE)
	    # ...
	elseif (UNIX)
	    install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/plugins
	             DESTINATION ${INSTALL_DIR_PLUGINS} 
                 COMPONENT plugins
	            )

	    install( DIRECTORY ${QT_PLUGINS_DIR}/designer
	             DESTINATION ${INSTALL_DIR_PLUGINS}/plugins/ 
                 COMPONENT plugins
	            )
                
	    install( DIRECTORY ${QT_PLUGINS_DIR}/imageformats
	             DESTINATION ${INSTALL_DIR_PLUGINS}/plugins/ 
                 COMPONENT plugins
	            )
                                                      	
	    FILE(INSTALL DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/package/linux/ TYPE EXECUTABLE FILES ${CMAKE_CURRENT_SOURCE_DIR}/package/linux/ar2gems.sh)
	    install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/package/linux/ar2gems.sh
	             DESTINATION ${INSTALL_DIR_BIN} 
                 COMPONENT scripts
	            )
                	                  
	    # COMMON    
	    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER ${PACKAGE_MAINTAINER})
	    
	    SET(CPACK_PACKAGE_DESCRIPTION_FILE ${PACKAGE_DESCRIPTION_FILE})
	    SET(CPACK_RESOURCE_FILE_LICENSE ${PACKAGE_LICENSE_FILE})
	    
	    SET(CPACK_PACKAGE_VERSION_MAJOR ${PACKAGE_VERSION_MAJOR})
	    SET(CPACK_PACKAGE_VERSION_MINOR ${PACKAGE_VERSION_MINOR})
	    SET(CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_VERSION_PATCH})
	    
	    SET(CPACK_SYSTEM_NAME ${PACKAGE_SYSTEM_NAME})
	    
	    # DEB SPECIFIC
	    if (PACKAGE_TYPE STREQUAL "DEB")
	        SET(CPACK_GENERATOR "DEB")
	        SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON) # autogenerate dependency information (package d-shlibs should be installed)
	        SET(CPACK_DEBIAN_PACKAGE_DEPENDS "vtk (>= 6.1), libboost-system1.49.0 (>= 1.49.0)")
	    endif (PACKAGE_TYPE STREQUAL "DEB")
	    
	    # RPM SPECIFIC
	    if (PACKAGE_TYPE STREQUAL "RPM")
	        SET(CPACK_GENERATOR "RPM")
	    
	    endif(PACKAGE_TYPE STREQUAL "RPM")
	    
	    
	    INCLUDE(CPack)
	              
	elseif (WIN32)
        set(CPACK_COMPONENT_ALL application) # this neccessery to avoid install target not relevant for package (unit test, headers(will be managed by NSIS))
       
       
		set(CPACK_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
	    set(CPACK_GENERATOR "NSIS")
	    
		SET(CPACK_PACKAGE_FILE_NAME "SGeMS-x64-beta-ar2tech-installer")
		SET(CPACK_NSIS_INSTALLER_OUTFILE_NAME ${CPACK_PACKAGE_FILE_NAME})
		
		# pass NATIVE PATHS to NSIS template
		string(REPLACE "/" "\\\\" WINNATIVE_CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	    set(CPACK_NSIS_AR2GEMS_SOURCE_DIR ${WINNATIVE_CMAKE_CURRENT_SOURCE_DIR})

		string(REPLACE "/" "\\\\" WINNATIVE_CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
	    set(CPACK_NSIS_AR2GEMS_BINARY_DIR ${WINNATIVE_CMAKE_CURRENT_BINARY_DIR})
	    set(CPACK_NSIS_AR2GEMS_BUILD_TYPE ${CMAKE_BUILD_TYPE})

		string(REPLACE "/" "\\\\" WINNATIVE_AR2GEMS_PLUGINS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/plugins)
	    set(CPACK_NSIS_AR2GEMS_PLUGINS_DIR ${WINNATIVE_AR2GEMS_PLUGINS_DIR})
	    

		# Qt4
		string(REPLACE "/" "\\\\" WINNATIVE_QT_BINARY_DIR ${QT_BINARY_DIR})
	    set(CPACK_NSIS_QT_BINARY_DIR ${WINNATIVE_QT_BINARY_DIR})

		string(REPLACE "/" "\\\\" WINNATIVE_QT_PLUGINS_DIR ${QT_PLUGINS_DIR})
	    set(CPACK_NSIS_QT_PLUGINS_DIR ${WINNATIVE_QT_PLUGINS_DIR})
	    
	    # VTK
		string(REPLACE "/" "\\\\" WINNATIVE_VTK_LIB_DIR ${VTK_DIR}/bin/${CMAKE_BUILD_TYPE})
	    set(CPACK_NSIS_VTK_LIB_DIR ${WINNATIVE_VTK_LIB_DIR})
	    
	    # Python
		string(REPLACE "/" "\\\\" WINNATIVE_PYTHON_DIR ${PYTHON_INCLUDE_DIR}/..)
	    set(CPACK_NSIS_PYTHON27_DIR ${WINNATIVE_PYTHON_DIR})
 		
 		# VC100
	    include(InstallRequiredSystemLibraries)
		string(REPLACE "/" "\\\\" WINNATIVE_VC100_CRT ${MSVC10_REDIST_DIR}/x64/Microsoft.VC100.CRT)
	    set(CPACK_NSIS_VC100_CRT ${WINNATIVE_VC100_CRT})
	    	     
	    INCLUDE(CPack)
	endif (APPLE)
	
    if (ALLOW_BUILD_HEADERS)
        #install headers  
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ar2gems/ 
                DESTINATION ${INSTALL_DIR_HEADERS}/ar2gems/ 
                COMPONENT headers
                FILES_MATCHING PATTERN "*.h"
        )
        
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ar2GsTL/
                DESTINATION ${INSTALL_DIR_HEADERS}/ar2GsTL/ 
                COMPONENT headers
                FILES_MATCHING PATTERN "*.h"
        )
    
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ar2GsTL/GsTL/utils/smartptr.cc
                DESTINATION ${INSTALL_DIR_HEADERS}/ar2GsTL/GsTL/utils/
                COMPONENT headers
        )    
    
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ar2GsTL/GsTL/kriging/SK_constraints.cc
                DESTINATION ${INSTALL_DIR_HEADERS}/ar2GsTL/GsTL/kriging/
                COMPONENT headers
        )
    
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ar2GsTL/GsTL/kriging/OK_constraints.cc
                DESTINATION ${INSTALL_DIR_HEADERS}/ar2GsTL/GsTL/kriging/
                COMPONENT headers
        )
    
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ar2GsTL/GsTL/kriging/KT_constraints.cc
                DESTINATION ${INSTALL_DIR_HEADERS}/ar2GsTL/GsTL/kriging/
                COMPONENT headers
        )

        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Eigen/Core
                DESTINATION ${INSTALL_DIR_HEADERS}/Eigen/
                COMPONENT headers
        )
        
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ar2GsTL/GsTL/geometry/covariance.cc
                DESTINATION ${INSTALL_DIR_HEADERS}/ar2GsTL/GsTL/geometry/
                COMPONENT headers
        ) 
       
        if (WIN32)
            add_custom_target(headers    
                COMMAND ${CMAKE_COMMAND}
                -D COMPONENT=headers
                -P cmake_install.cmake
            )

        endif (WIN32)
                
    endif (ALLOW_BUILD_HEADERS)  
        
endif (ALLOW_BUILD_PACKAGE)    
