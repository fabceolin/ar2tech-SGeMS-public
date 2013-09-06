AR2GEMS
=======

__AR2Tech geostatistical libraries and software__

-------------------------------------------------

This project is managed and owned by _Advanced Resources and Risk Technology, LLC (AR2Tech)_.
For any questions, please contact [Alex Boucher](aboucher@ar2tech.com).

__AR2GEMS__ is a branch of the open-source [SGeMS Software][1] under its x-free license.
This version cannot be integrated into existing software or distributed without
the explicit authorization of AR2Tech.

If you would like to contribute code to this project you can either:

1. License the new code with a [BSD license][2]
2. Transfer the copyright to AR2Tech

If you are interested into another option please contact AR2Tech.

### Academic research:

There are no restrictions for academic purposes.  Note that only the plugins can be
distributed by third-party, the software itself can only be distributed by AR2Tech.
This is done to reduce fragmentation of the software.

### Commercial plugins:

You are free to build proprietary plugins for commercial purposes within an organization
(no distribution) or to be freely distributed (no requirement to release the source code).
Please contact AR2Tech for licensing agreement if you intend to sell or distribute the
plugins with a fee.

Dependencies
------------

* [Boost](http://www.boost.org/)
* [Python 2.x](http://www.python.org/)
* [Qt](http://qt-project.org/)
* [VTK](http://www.vtk.org/)

Build instructions
------------------

SGeMS uses cmake (http://www.cmake.org/) for the build process.  


Building the dependencies on Windows
--------------------------------------------------

Note that for Visual Studio 2010 (msvc10) Visual Studio SP1 must be installed.
Required external libraries: Qt, VTK, Boost and Python.

Compiling Qt 64 bits
--------------------

1. Download Qt source code (a zip file) from: http://qt-project.org/downloads
2. Unzip it (e.g. c:\Qt\4.8.3-x64)
3. Open the prompt shell for visual studio 2010 (
   go to the MS start->All Programs->Microsoft Visual studio 2010
   ->Visual Studio Tools->Visual Studio x64 Win64 Command Prompt (2010))
4. Go to the Qt directory and run:

   ```
   configure -debug-and-release -no-webkit -platform win32-msvc2010 -no-script -no-scripttools -opensource
   ```
   (The option -no-webkit is to skip that package since it often generates compiling errors on msvc)

Compiling VTK
-------------

1. Download and install [CMake][3]
2. Get the VTK 6.0 source code either from Git or from the [website](http://vtk.org/VTK/resources/software.html).
   From git be sure to checkout the tag vtk6.0.0 
3. Build the project files with with cmake or cmake-gui.  
   * With cmake-gui: Check the box "Grouped" and "Advanced"
   * Select the group: VTK_Group_Qt, VTK_Group_Rendering, VTK_Group_Views, VTK_Group_StandAlone
   * CMake will generate an error if Qt-webkit has not been built in that case: 
    - Unselect VTK_Group_Qt
    - Go to Module and select all Qt related module except "Module_vtkGUISupportQtWebkit"
4. Open the VTK project files into Visual Studio and build the release and debug version.

Compiling Python 64 bits
------------------------

Installing Python 2.x from the installer only provide the release dll.  To get the
debug version, download the source code, open the project and build the debug version.
You can ignore all the errors.  Copy the debug .dll to the main Python
directory and the .lib into the libs folder along with the release version.

Building AR2GEMS
----------------

The software is built with [CMake][3]:
Open cmake-gui and set the proper path to Boost, VTK and Qt


### Windows

Open the the solution ar2gems.sln to build the program


### Linux (without cmake-gui)
   ```
   mkdir build
   cd build
   cmake ..
   make -j 8
   ```
   
Read CMakeLists.txt for more info.

[1]: http://sgems.sourceforge.net/
[2]: http://en.wikipedia.org/wiki/BSD_licenses/
[3]: http://www.cmake.org/
