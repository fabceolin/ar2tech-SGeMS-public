cmake_minimum_required(VERSION 2.8)

project(ar2gems)

######################################################################
# Please set the variables with your preferences. The defaults should
# work out of the box.
######################################################################

set(CMAKE_BUILD_TYPE Release) # Debug, Release, RelWithDebInfo or MinSizeRel

### BOOST SETTINGS ###################################################

# Uncomment if Boost isn't in your path:
#set(Boost_NO_SYSTEM_PATHS TRUE)
#set(BOOST_ROOT /opt/boost)

### VTK SETTINGS #####################################################

# Uncomment if VTK isn't in your path:
#set(VTK_DIR /opt/vtk/lib/cmake/vtk-6.0)

######################################################################

add_subdirectory(ar2gems)
