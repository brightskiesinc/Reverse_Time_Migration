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

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Setup The Environment](#setup-the-environment)
- [Docker](docs/manual/Docker.md#docker)
    - [OpenMP docker](docs/manual/Docker.md#OpenMP-Docker)
    - [OneAPI docker](docs/manual/Docker.md#OneAPI-Docker)
    - [OpenmpOffload](docs/manual/Docker.md#OneAPI-Docker)
    - [Additional Options](docs/manual/Docker.md#additional-options)
- [Building & Running](docs/manual/BuildingAndRunning.md#building-&-running)
    - [OpenMP Version](docs/manual/BuildingAndRunning.md#openmp-version)
        - [Building OpenMP Version](docs/manual/BuildingAndRunning.md#building-openmp-version)
        - [Run OpenMP](docs/manual/BuildingAndRunning.md#run-openmp)
    - [OneAPI Version](docs/manual/BuildingAndRunning.md#oneapi-version)
        - [Building OneAPI Version](docs/manual/BuildingAndRunning.md#building-oneapi-version)
        - [Run OneAPI on CPU](docs/manual/BuildingAndRunning.md#run-oneapi-on-cpu)
        - [Run OneAPI on Gen9 GPU](docs/manual/BuildingAndRunning.md#run-oneapi-on--gen9-gpu)
    - [OpenMP Offload Version](docs/manual/BuildingAndRunning.md#openmp-offload-version)
        - [Building OpenMP Offload Version](docs/manual/BuildingAndRunning.md#building-openmp-offload-version)
        - [Run OpenMP Offload](docs/manual/BuildingAndRunning.md#run-openmp-offload)
- [Advanced Running Options](docs/manual/AdvancedRunningOptions.md#advanced-running-options)
    - [Program Arguments](docs/manual/AdvancedRunningOptions.md#program-arguments)
    - [Configuration Files](docs/manual/AdvancedRunningOptions.md#configuration-files)
        - [Structure](docs/manual/AdvancedRunningOptions.md#structure)
        - [Computation Parameter Configuration Block](docs/manual/AdvancedRunningOptions.md#computation-parameter-configuration-block)
        - [Engines Configurations Block](docs/manual/AdvancedRunningOptions.md#engines-configurations-block)
        - [Callback Configuration Block](docs/manual/AdvancedRunningOptions.md#callback-configuration-block)
- [Results Directories](docs/manual/ResultsDirectories.md#Results-Directories)
- [Tools](docs/manual/Tools.md#Tools)
    - [Build & Run](docs/manual/Tools.md#build-&-run)
    - [Available Tools](docs/manual/Tools.md#available-tools)
        - [Comparator](docs/manual/Tools.md#comparator)
        - [Generator](docs/manual/Tools.md#generator)
- [Versioning](#versioning)
- [Changelog](#changelog)
- [License](#license)

## Features

* An optimized OpenMP version:
    * Support the following boundary conditions:
        * CPML
        * Sponge
        * Random
    * Support the following stencil orders:
        * O(2)
        * O(4)
        * O(8)
        * O(12)
        * O(16)
    * Support 2D modeling and imaging
    * Support the following algorithmic approaches:
        * Two propagation, an I/O intensive approach where you would store all of the calculated wave fields while
          performing the forward propagation, then read them while performing the backward propagation.
        * We provide the option to use the ZFP compression technique in the two-propagation workflow to reduce the
          volume of data in the I/O.
        * Three propagation, a computation intensive approach where you would calculate the forward propagation storing
          only the last two time steps. You would then do a reverse propagation, propagate the wave field stored from
          the forward backward in time alongside the backward propagation.
    * Support solving the equation system in:
        * Second Order
        * Staggered First Order
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
    * Support 2D modeling and imaging
    * Support the following algorithmic approaches:
        * Three propagation, a computation intensive approach where you would calculate the forward propagation storing
          only the last two time steps. You would then do a reverse propagation, propagate the wave field stored from
          the forward backward in time alongside the backward propagation.
    * Support solving the equation system in:
        * Second order
* Basic OpenMp Offload version:
    * Support the following boundary conditions:
        * None
    * Support the following stencil orders:
        * O(2)
        * O(4)
        * O(8)
    * Support 2D modeling and imaging
    * Support the following algorithmic approaches:
        * Two propagation, an I/O intensive approach where you would store all of the calculated wave fields while
          performing the forward propagation, then read them while performing the backward propagation.
        * Three propagation, a computation intensive approach where you would calculate the forward propagation storing
          only the last two time steps. You would then do a reverse propagation, propagate the wave field stored from
          the forward backward in time alongside the backward propagation.
    * Support solving the equation system in:
        * Second order

## Setup The Environment

1. Clone the basic project
    ```shell script
    git clone https://github.com/brightskiesinc/Reverse_Time_Migration
    ```

2. Change directory to the project base directory
    ```shell script
    cd Reverse_Time_Migration/
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

