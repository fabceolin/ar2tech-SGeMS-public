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

The software is built with [CMake][3]:

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
