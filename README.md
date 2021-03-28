# Seismic Toolbox
<p>
  <img src="https://img.shields.io/pypi/status/Django.svg" alt="stable"/>
</p>

<p>
Seismic Toolbox contains all different seismology algorithm (RTM currently). Algorithms are computationally intensive processes which requires propagating wave in 2D model using time domain finite differences wave equation solvers.
</p>

<p>
During the imaging process a forward-propagated source wave field is combined at regular time steps with a back-propagated receiver wave field. Traditionally, synchronization of both wave fields result in a very large volume of I/O, disrupting the efficiency of typical supercomputers. Moreover, the wave equation solvers are memory bandwidth bound due to low flop-per-byte ratio and non-contiguous memory access, resulting hence in a low utilization of available computing resources.
</p>

<p>
Alternatively, approaches to reduce the IO bottleneck or remove it completely to fully utilize the processing power are usually explored and utilized such as the use of compression to reduce the I/O volume. Another approach that eliminates the need for I/O would be to add another propagation in reverse-time to the forward propagated source wave field.
</p>

## Table of Contents
- [Features](#Features)
- [Prerequisites](#Prerequisites)
- [Setup The Environment](#Setup The Environment)
- [Docker](docs/manual/Docker.md#Docker)
    - [OpenMP docker](docs/manual/Docker.md#OpenMP Docker)
    - [OneAPI docker](docs/manual/Docker.md#OneAPI Docker)
    - [Additional Options](docs/manual/Docker.md#Additional Options)
- [Building & Running](docs/manual/BuildingAndRunning.md#Building-&-Running)
    - [OpenMP Version](docs/manual/BuildingAndRunning.md#OpenMP Version)
        - [Building OpenMP Version](docs/manual/BuildingAndRunning.md#Building OpenMP Version)
        - [Run OpenMP](docs/manual/BuildingAndRunning.md#Run OpenMP)
    - [OneAPI Version](docs/manual/BuildingAndRunning.md#OneAPI-version)
        - [Building OneAPI Version](docs/manual/BuildingAndRunning.md#building-OneAPI-version)
        - [Run OneAPI on CPU](docs/manual/BuildingAndRunning.md#Run OneAPI on CPU)
        - [Run OneAPI on Gen9 GPU](docs/manual/BuildingAndRunning.md#Run OneAPI on Gen9 GPU)
    - [CUDA Version](docs/manual/BuildingAndRunning.md#CUDA Version)
        - [Building CUDA Version](docs/manual/BuildingAndRunning.md#Building CUDA Version)
        - [Run CUDA](docs/manual/BuildingAndRunning.md#Run CUDA)
- [Advanced Running Options](docs/manual/AdvancedRunningOptions.md#Advanced-Running-Options)
    - [Program Arguments](docs/manual/AdvancedRunningOptions.md#Program Arguments)
    - [Configuration Files](docs/manual/AdvancedRunningOptions.md#Configuration Files)
        - [Structure](docs/manual/AdvancedRunningOptions.md#Structure)
        - [Computation Parameter Configuration Block](docs/manual/AdvancedRunningOptions.md#Computation Parameter Configuration Block)
        - [Engines Configurations Block](docs/manual/AdvancedRunningOptions.md#Engines Configurations Block)
        - [Callback Configuration Block](docs/manual/AdvancedRunningOptions.md#Callback Configuration Block)
- [Results Directories](docs/manual/ResultsDirectories.md#Results-Directories)
- [Tools](docs/manual/Tools.md#Tools)
    - [Build & Run](docs/manual/Tools.md#Build-&-Run)
    - [Available Tools](docs/manual/Tools.md#Available-Tools)
        - [Comparator](docs/manual/Tools.md#Comparator)
        - [Generator](docs/manual/Tools.md#Generator)
- [Versioning](#Versioning)
- [Changelog](#Changelog)
- [License](#License)


## Features

* An optimized OpenMP version:
    * Support the following boundary conditions:
        * CPML
        * Sponge
        * Random
        * Free Surface Boundary Functionality
    * Support the following stencil orders:
        * O(2)
        * O(4)
        * O(8)
        * O(12)
        * O(16)
    * Support 2D modeling and imaging
    * Support the following algorithmic approaches:
        * Two propagation, an I/O intensive approach where you would store all of the calculated wave fields while performing the forward propagation, then read them while performing the backward propagation.
        * We provide the option to use the ZFP compression technique in the two-propagation workflow to reduce the volume of data in the I/O.
        * Three propagation, a computation intensive approach where you would calculate the forward propagation storing only the last two time steps. You would then do a reverse propagation, propagate the wave field stored from the forward backward in time alongside the backward propagation.
    * Support solving the equation system in:
        * Second Order
        * Staggered First Order
        * Vertical Transverse Isotropic (VTI)
        * Tilted Transverse Isotropic (TTI)
    * Support manual cache blocking.
* An optimized DPC++ version:
    * Support the following boundary conditions:
        * None
        * Random
        * Sponge
        * CPML
    * Support the following stencil orders:
        * O(2)
        * O(4)
        * O(8)
        * O(12)
        * O(16)
    * Support 2D modeling  and imaging
    * Support the following algorithmic approaches:
        * Three propagation, a computation intensive approach where you would calculate the forward propagation storing only the last two time steps. You would then do a reverse propagation, propagate the wave field stored from the forward backward in time alongside the backward propagation.
    * Support solving the equation system in:
        * Second order
* Basic CUDA version:
    * Support the following boundary conditions:
        * None
    * Support the following stencil orders:
        * O(2)
        * O(4)
        * O(8)
        * O(12)
        * O(16)
    * Support 2D modeling  and imaging
    * Support the following algorithmic approaches:
        * Three propagation, a computation intensive approach where you would calculate the forward propagation storing only the last two time steps. You would then do a reverse propagation, propagate the wave field stored from the forward backward in time alongside the backward propagation.
    * Support solving the equation system in:
        * Second order


## Setup The Environment
1. Clone the basic project
    ```shell script
    git clone https://gitlab.brightskiesinc.com/parallel-programming/SeismicToolbox
    ```

2. Change directory to the project base directory
    ```shell script
    cd SeismicToolbox/
    ```
3. To install and download everything you can easily run the ```setup.sh``` script found in ```/prerequisites``` folder
    ```shell script
    ./prerequisites/setup.sh
    ```
   or refer to the ```README.md``` file in ```/prerequisites``` folder for more specific installations. 


## Prerequisites
* **CMake**\
```CMake``` version 3.5 or higher.

* **C++**\
```c++11``` standard supported compiler.

* **Catch2**\
Already included in the repository in ```prerequisites/catch```

* **OneAPI**\
[OneAPI](https://software.intel.com/content/www/us/en/develop/tools/oneapi.html) for the DPC++ version.

* **ZFP Compression**
    * Only needed with OpenMp technology
    * You can download it from a script found in ```prerequisites/utils/zfp``` folder
      
* **OpenCV**
    * Optional
    * v4.3 recommended
    * You can download it from a script found in ```prerequisites/frameworks/opencv``` folder


## Versioning

When installing Seismic Toolbox, require its version. For us, this is what ```major.minor.patch``` means:

- ```major``` - **MAJOR breaking changes**; includes major new features, major changes in how the whole system works, and complete rewrites; it allows us to _considerably_ improve the product, and add features that were previously impossible.
- ```minor``` - **MINOR breaking changes**; it allows us to add big new features.
- ```patch``` - **NO breaking changes**; includes bug fixes and non-breaking new features.


## Changelog

For previous versions, please see our [CHANGELOG](CHANGELOG.rst) file.


## License
This project is licensed under the The GNU Lesser General Public License, version 3.0 (LGPL-3.0) Legal License - see the [LICENSE](LICENSE.txt) file for details

