# Timer (BSTimer)

## Table of content

- [Description](#description)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Project Hierarchy](#project-hierarchy)
- [Versioning](#versioning)
- [User Manual](#user-manual)
- [Changelog](#changelog)
- [License](#license)

## Description

General timer library for evaluating runtimes of different kernels and functions aimed to serve various backends. This
library is designed to fit in C, C++ and Python codes.

## Prerequisites

* **CMake**\
  Cmake version 3.5 or higher.

* **C++**\
  C++11 standard supported compiler.

* **Catch2**\
  Already included in the repository in ```libs/BSBase/prerequisites/libraries/catch```

* **BSBase**\
  Brightskies Base Package for multiple backends supports.

## Features

* **Basic Timer**\
  Timer which could be started anywhere in code, and takes a snapshot of the time, then stopped\
  at another line of code to record end time.
* **Lazy Timer**\
  A variation of the timer, where you can pass a block of code as a parameter to evaluate\
  its runtime
* **Scope Timer**\
  A variation of the timer, where it starts at creation, and stops as it goes out of scope
* **Support for different technologies**
    * OpenMP
* **Analyzing runtimes**\
  Timers registered to same 'Channel' -which corresponds to a function to be timed- can be analyzed to review maximum,
  minimum, and average runtimes. Also, for kernels, you can pass as parameter: grid size, number of arrays, and number
  of operations in order to get and review the maximum, minimum and average bandwidths and throughput.
* **Reporting**
    * Console Reporting
    * Exporting report to text file
    * Viewing charts of runtimes and bandwidths, produced by Python module in the project

## Project Hierarchy

* **```prerequisites```**\
  Folder containing the prerequisites needed for the project, or default scripts to install them.

* **```include```**\
  The folder containing all the headers of the system. Contains ReadMe explaining the internal file structure of the
  project.

* **```src```**\
  The folder containing all the source files of the system. Follows same structure as the include.

* **```python```**\
  The folder containing all the **`.py`** files of the system.

* **```tests```**\
  The folder containing all the tests of the system. Follows same structure as the include.

* **```examples```**\
  The folder containing all the demo code showcasing how the framework is used within applications, and the capabilities
  of the framework.

* **```clean_build.sh```**\
  Script used to build the system tests after running the config.sh, and by default build all the different modules of
  the project.

* **```config.sh```**\
  Script used to generate the building system inside a 'bin' directory.

* **```CMakeLists.txt```**\
  The top level `CMake` file to configure the build system.

## User Manual

* [User Manual](docs/manual/UserManual.md)

## Versioning

When installing the Timer library, require it's version. For us, this is what ```major.minor.patch``` means:

- ```major``` - **MAJOR breaking changes**; includes major new features, major changes in how the whole system works,
  and complete rewrites; it allows us to _considerably_ improve the product, and add features that were previously
  impossible.
- ```minor``` - **MINOR breaking changes**; it allows us to add big new features.
- ```patch``` - **NO breaking changes**; includes bug fixes and non-breaking new features.

## Changelog

For previous versions, please see our [CHANGELOG](CHANGELOG.rst) file.

## License

This project is licensed under the The GNU Lesser General Public License, version 3.0 (LGPL-3.0) Legal License - see
the [LICENSE](LICENSE.txt) file for details
