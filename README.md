# Reverse Time Migration (RTM)
## Table of content
- [Description](#description)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Docker instructions](#docker-instructions)
    - [OpenMP docker](#openmp-docker)
    - [DPC++ docker](#dpc-docker)
    - [Additional options](#additional-options)
- [Building & Running](#building-running)
    - [OpenMP Version](#openmp-version)
        - [Building OpenMP Version](#building-openmp-version)
        - [Run OpenMP](#run-openmp)
    - [DPC++ Version](#dpc-version)
        - [Building DPC++ Version](#building-dpc-version)
        - [Run DPC++ on CPU](#run-dpc-on-cpu)
        - [Run DPC++ on Gen9 GPU](#run-dpc-on-gen9-gpu)
- [Advanced Running Options](#advanced-running-options)
    - [Program Arguments](#program-arguments)
    - [Computation Parameter Configuration](#computation-parameter-configuration)
    - [RTM Engine Configuration](#rtm-engine-configuration)
        -[Effect on timing](#effect-on-timing) 
    - [Models Configuration File](#models-configuration-file)
    - [Traces Configuration File](#traces-configuration-file)
    - [Callback Configuration](#callback-configuration)
- [Results directory explanation](#results-directory-explanation)

---
## Description
*  RTM is a computationally intensive process which requires propagating wave in 2D model using time domain finite differences wave equation solvers. 
*  During the imaging process a forward-propagated source wavefield is combined at regular time steps with a back-propagated receiver wavefield.
*  Traditionally, synchronization of both wavefields result in a very large volume of IO, disrupting the efficiency of typical supercomputers.
*  Moreover, the wave equation solvers are memory bandwidth bound due to low flop-per-byte ratio and non-contiguous memory access, resulting hence in a low utilization of available computing resources.
*  Alternatively, approaches to reduce the IO bottleneck or remove it completely to fully utilize the processing power are usually explored and utilized such as the use of compression to reduce the IO volume. 
    *  Another approach that eliminates the need for IO would be to add another propagation in reverse-time to the forward propagated source wavefield.
---
## Features
* An optimized OpenMP version:
    * Support the following boundary conditions :
        * CPML
        * Sponge
        * Random
        * Free surface boundary functionality
    * Support the following stencil orders :
        * O(2)
        * O(4)
        * O(8)
        * O(12)
        * O(16)
    * Support 2D modeling and imaging
    * Support the following algorithmic approaches :
        * Two propagation, an IO intensive approach where you would store all of the calculated wavefields while performing the forward propagation, then read them while performing the backward propagation.
        * We provide the option to use the ZFP compression technique in the two-propagation workflow to reduce the volume of data in the IO.
        * Three propagation, a computation intensive approach where you would calculate the forward propagation storing only the last two timesteps. You would then do a reverse propagation, propagate the wavefield stored from the forward backward in time alongside the backward propagation.
    * Support solving the equation system in :
        * Second order
        * Staggered first order
    * Support manual cache blocking.
* An optimized DPC++ version:
    * Support the following boundary conditions :
        * Random
    * Support the following stencil orders :
        * O(2)
        * O(4)
        * O(8)
        * O(12)
        * O(16)
    * Support 2D modeling  and imaging
    * Support the following algorithmic approaches :
        * Three propagation, a computation intensive approach where you would calculate the forward propagation storing only the last two timesteps. You would then do a reverse propagation, propagate the wavefield stored from the forward backward in time alongside the backward propagation.
    * Support solving the equation system in :
        * Second order
---
## Prerequisites
* g++(V7.2 or newer) or icpc compiler (icpc recommended for best performance).
* CMake (V3.10 or newer recommended).
* [oneAPI](https://software.intel.com/content/www/us/en/develop/tools/oneapi.html) for the DPC++ version.
* **ZFP compression(only needed with OpenMp technology)**:
    1.  Download ZFP source code archive from any of the following sites.
    
        ZFP 0.5.2: wget https://computation.llnl.gov/projects/floating-point-compression/download/zfp-0.5.2.tar.gz

        ZFP 0.5.4: wget https://computation.llnl.gov/projects/floating-point-compression/download/zfp-0.5.4.tar.gz
    
        ZFP 0.5.5: wget https://computation.llnl.gov/projects/floating-point-compression/download/zfp-0.5.5.tar.gz 
        
    2.  Unarchive the source code.
    
        ```
        tar xf zfp-0.5.x.tar.gz
        ```
    3.  Change directory to the zfp directory:
        ```
        cd zfp-0.5.x
        ```
        
    4.  **Optional** patching with IPP (**icpc only**).
    
        a.echo $IPPROOT
	this command will output a path in the terminal(only if icpc is used).
	
	b.change directory to this output path.
	
	c.Go to the examples directory.
        ```
        cd examples
        ```
	
	d.Unarchive the folder components_and_examples_lin_ps.tgz.
	```
        tar xf components_and_examples_lin_ps.tgz
        ```
	
	e.Change directory to the zfp patch files directory.
	```
        cd components/interfaces/ipp_zfp
        ```
	
	f.Copy any zfp-0.5.x.patch to your zfp directory.
        ```
        cp ./zfp-0.5.x.patch ./"zfp_directory"
        ```
	
	g.Patch with any IPP version.
        ```
        patch -p1 < ./zfp-0.5.x.patch.
        ```
        
    5. Build the library with standard make utility:
    
        a. make with original zfp 
        ```
        make [ ARCH=ia32|intel64 ]
        ```
        The option "ARCH=ia32|intel64" defines set of Intel IPP libraries (32- or 64-bits) to be used in build of executables. By default, 64-bit libraries are used.
        
        b. make Original ZFP compression with IPP patching(Compression with IPP)(**icpc only**)
        ```
        make WITH_IPP=yes [ ARCH=ia32|intel64 ]
        ```
        The option "WITH_IPP=yes" sets "-DWITH_IPP -I$IPPROOT/include" options in compiler command line.
        The above command assumes that IPPROOT environment variable points to the head of Intel IPP location.
* **Optional** : OpenCV (V4.3 recommended).
    * Building from source is recommended, the following are the steps to install on an Ubuntu 18.04 system.
    ```
    $ sudo apt install build-essential cmake git pkg-config libgtk-3-dev \
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy \
    libtbb2 libtbb-dev libdc1394-22-dev
    $ mkdir ~/opencv_build && cd ~/opencv_build
    $ git clone https://github.com/opencv/opencv.git
    $ git clone https://github.com/opencv/opencv_contrib.git
    $ cd ~/opencv_build/opencv
    $ mkdir build && cd build
    $ cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_C_EXAMPLES=ON \
    -D INSTALL_PYTHON_EXAMPLES=ON \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    -D OPENCV_EXTRA_MODULES_PATH=~/opencv_build/opencv_contrib/modules \
    -D BUILD_EXAMPLES=ON ..
    $ make -j8
    $ sudo make install
    ```
---
## Installation
1. Clone the basic project
    * Using ssh
    ```
    git clone git@gitlab.brightskiesinc.com:parallel-programming/reversetimemigration.git
    ```
    * Or using https
    ```
    git clone https://gitlab.brightskiesinc.com/parallel-programming/reversetimemigration.git
    ```

2. Change directory to the project base directory
    ```
    cd reversetimemigration/
    ```
3. Download the seismic data (2004 BP Model, refer [here](https://wiki.seg.org/wiki/2004_BP_velocity_estimation_benchmark_model) for more information):
    * Download the full 2004 bp data : this will download around 8 GB of data.
    ```
    ./Scripts/downloading_bp_data.sh
    ```
    * Or download the minimal amount of files needed to run : this will download around 1 GB of data. Only one shot file will be downloaded.
    ```
    ./Scripts/downloading_bp_data_minimal.sh
    ```
---
## Docker instructions
This guide assumes:
1.   you have your terminal opened with the current working directory being the directory of the project.
2.   Docker engine is installed in your machine.

### OpenMP docker
1. Change directory to the OpenMP docker file directory
    ```
    cd Docker_containers/Oneapi_docker/
    ```
2. Build image from Docker file included in the current directory and give a tag for the image.
    ```
   docker build -t <tag of image> .
    ```
    Note: this step may need sudo privilege.
3. Build a container from the image and give it a name:this step looks for the license file for intel parallel studio in the directory /opt/src/license/ inside the container so replace this directory by the path to the license file in your machine 
by adding -v
    ```
   docker run -v /<pathTo>/intel_parrallel_studio_license:/opt/src/license/  --name <name of container> -it <tag of image> 
    ```

### DPC++ docker
1. Change directory to the DPC++ docker file directory
    ```
    cd Docker_containers/OpenMp_docker/
    ```
2. Build image from Docker file included in the current directory and give a tag for the image.
    ```
   docker build -t <tag of image> .
    ```
    Note: this step may need sudo privilege.
3. Build a container from the image and give it a name.
    ```
   docker run --name <name of container> -it <tag of image> 
    ```
### Additional options:
1.  To enable characterization inside the docker using Intel tools add --privileged=true to the docker run command.
    ```
    docker run --privileged=true --name <name of container> -it <tag of image> 
    ```
2.  To store what is inside a specific directory in docker in an external directory on your machine use the -v option with docker run command.
    ```
    docker run -v  /<pathTo>/local directory:/<path_to_directroy_inside_the_docker> --name <name of container> -it <tag of image>  
    ```
## Building & Running
This guide assumes you have your terminal opened with the current working directory being the directory of the project.
### OpenMP Version
#### Building OpenMP Version
* Using Intel Compiler (Heavily recommended)
    1. Source Intel parallel studio to be able to use icpc command.
    2. Run configuration script and give it the path to zfp compression directory.
    ```
    ./config.sh -t omp -c "path_to_zfp_compression"
    ```
    This is will run the RTM using OpenMP technology and intel compiler and without OpenCV. **For OpenCV, add '-i on' to enable OpenCV**.
    For more details about the options for the config script, press [here](https://gitlab.brightskiesinc.com/parallel-programming/reversetimemigration/-/wikis/projects/openmp_rtm/config.sh)
    
    3. Run the building script.

    ```
    ./clean_build.sh
    ```
    For more details about the options for the build script, press [here](https://gitlab.brightskiesinc.com/parallel-programming/reversetimemigration/-/wikis/projects/openmp_rtm/clean_build.sh)
* Using gcc compiler(Not Recommended)
    1. Run configuration script and give it the path to zfp compression directory.
    ```
    ./config.sh -t omp -g -c "path_to_zfp_compression"
    ```
    This is will run the RTM using OpenMP technology and gcc compiler and without OpenCV. **For OpenCV, add '-i on' to enable OpenCV**.
    For more details about the options for the config script, press [here](https://gitlab.brightskiesinc.com/parallel-programming/reversetimemigration/-/wikis/projects/openmp_rtm/config.sh)
    
    2. Run the building script.
    ```
    ./clean_build.sh
    ```
    This will build the acoustic_engine(Application for migration) and acoustic_modeller(Application for modelling) in parallel by default.
    For more details about the options for the build script, press [here](https://gitlab.brightskiesinc.com/parallel-programming/reversetimemigration/-/wikis/projects/openmp_rtm/clean_build.sh)
#### Run OpenMP
1. Export the compact KMP_AFFINITY for OpenMp threads.
```
export KMP_AFFINITY=compact
```
2. Export the OMP_NUM_THREADS and make it equal to the number of threads wanted to run. For example, here we show if we want to make the rtm run using 36 threads.
```
export OMP_NUM_THREADS=36
```
Or

2. Export the KMP_HW_SUBSET multiplication of cores and therads and make it equal to the number of threads wanted to run. For example, here we show if we want to make the rtm run using 36 threads on 36 cores and 1 thread on each core.
```
export KMP_HW_SUBSET=36c,1t
```
**Warning**:the OMP_NUM_THREADS overrides the KMP_HW_SUBSET values.

3. Run the rtm engine.
```
./bin/acoustic_engine
```
* OpenMP : utilizes cache blocking to improve performance, this is provided by the user and might vary according to the model, optimal values were found to be 5500 in x, 55 in z on the BP model.


### DPC++ Version
#### Building DPC++ Version
1. Source Intel oneAPI to be able to use dpcpp command. If it is already sourced, this step won't be needed.
2. Run configuration script.
```
./config.sh -t dpc
```
This is will run the RTM using DPC++ technology and without OpenCV. **For OpenCV, add '-i on' to enable OpenCV**.
For more details about the options for the config script, press [here](https://gitlab.brightskiesinc.com/parallel-programming/reversetimemigration/-/wikis/projects/openmp_rtm/config.sh)

3. Run the building script.
```
./clean_build.sh
```
This will build the acoustic_engine(Application for migration) and acoustic_modeller(Application for modelling) in parallel by default.
For more details about the options for the build script, press [here](https://gitlab.brightskiesinc.com/parallel-programming/reversetimemigration/-/wikis/projects/openmp_rtm/clean_build.sh)
#### Run DPC++ on CPU
```
./bin/acoustic_engine -p ./workloads/bp_model/computation_parameters_dpc_cpu.txt
```
* Optimal workgroup sizes were found to be 512 in x, 2 in z.
#### Run DPC++ on Gen9 GPU
```
./bin/acoustic_engine -p ./workloads/bp_model/computation_parameters_dpc_gen9.txt
```
* Optimal workgroup sizes were found to be 128 in x, 16 in z. Notice that for gpu, the x dimension is limited by the maximum workgroup size(256 for Gen9).
---
## Advanced Running Options
### Program Arguments
```
Usage : ./bin/acoustic_engine

Performs reverse time migration according to the parameters parsed from the different configuration files.

Optional flags : 
	-m <workload Path>: choose path to load workload configurations from, by default: ./workloads/bp_model
	-p <computation parameter file> : the file to parse for computation parameters eg: ./workloads/bp_model/computation_parameters.txt
	-s <system configuration file> : the file to parse for system configuration eg: ./workloads/bp_model/rtm_configuration.txt
	-c <callback configuration file> : the file to parse for callback configuration eg: ./workloads/bp_model/callback_configuration.txt
	-w <write path> : the path to write the results to eg : ./results
	-h : print the options for this command
```

### Computation Parameter Configuration
The computation parameter configuration file is a text file which has the following format:
```
stencil-order=8
boundary-length=20
source-frequency=20
dt-relax=0.9
block-x=5500
block-z=55
block-y=1
cor-block=256
device=cpu
```
* Stencil order signifies the order of the approximation of the finite difference for space derivatives. The supported values are 2, 4, 8, 12, 16.
* Boundary length is a number that signifies the boundary layer thickness, can have any integer value >= 0.
* Source frequency should be specified in Hz. Should be a value > 0.
* dt-relax is the factor to be multiplied in the dt calculated by the stability criteria as an extra measure of safety, should be > 0 and < 1, normally 0.9.
* block-x, block-z and block-y parameters control the cache blocking in OpenMP and the workgroup/elements per workitem in DPC++, they have different constraints according to the device or technology used(The constraint is told in the running part for each device).
* cor-block is a DPC++ only parameter that controls the workgroup size for the correlation operation.
* device is a DPC++ only parameter that can take the value of 'cpu', 'gpu', 'gpu-semi-shared' and 'gpu-shared'. 
The different gpu options will select different kernel optimizations to run. Both 'gpu' and 'gpu-shared' give the best performance when the blocking is tuned correctly.
* A sample of this file is available in 'workloads/bp_model/computation_parameters.txt'.
### RTM Engine Configuration
* The main rtm configuration file will define the properties to use in the different computations. A sample of this file is available in 'workloads/bp_model
rtm_configuration.txt'.
```
########################## Ideally version control settings ahead ##################
## Supported values for physics: acoustic
physics=acoustic
## Supported values for approximation: isotropic
approximation=isotropic
## Supported values for equation order : second | first
equation-order=second
## Supported values for grid-sampling : uniform
grid-sampling=uniform
############################ Component Settings ahead #######################
#### Model handler possible values : homogenous | segy
model-handler=segy
#### Source Injectior possible values : ricker
source-injector=ricker
#### Boundary manager possible values : none | random | cpml | sponge
boundary-manager=random
#### Use top boundary for forward propagation or not - Option only effective when using CPML/Sponge boundary conditions ####
#### By default yes , supported options yes | no #####
boundary-manager.use-top-layer=yes
#### Uncomment the following to fine tune some parameters for the boundary conditions
#boundary-manager.reflect-coeff=0.05
#boundary-manager.shift-ratio=0.2
#boundary-manager.relax-cp=0.9
#### Correlation kernel possible values : cross-correlation
correlation-kernel=cross-correlation
#### Forward collector possible values : two | three | two-compression | optimal-checkpointing
forward-collector=three
#### Uncomment the following to fine tune some parameters for the compression
#forward-collector.zfp-tolerance=0.05
## ZFP parallel can only be 1 or 0
#forward-collector.zfp-parallel=0
## ZFP relative can only be 1 or 0
#forward-collector.zfp-relative=1
#### Trace manager possible values : binary | segy
trace-manager=segy
############################# File directories ahead ###########################################
#### traces-list should point to a text file that contains the starting shot id in the first line
#### Ending shot id in the second line(exclusive).
#### Then the paths of the trace files with each one taking a line.
traces-list=workloads/bp_model/traces.txt

#### models-list should point to a text file containing the model files directories each in a line.
models-list=workloads/bp_model/models.txt
```
#### Effect on timing:
*   Supported values for equation order : second | first
    * first wave equation timing in 2x of second wave equation.  
* Forward collector possible values : two | three | two-compression
    * three is the fastest approach in timing.
    * two :is the slowest one  as it depends on th IO of the machine.
    * two-compression : timing is intermediate between three and two and also depends on the IO and compression used.

### Models Configuration File
* Models file as indicated in the RTM configuration. 
* A sample of this file is available in 'workloads/bp_model/models.txt'.
* This is a simple file comprised of two lines:
    * First line contains path to velocity model segy file.
    * Second line which (will only be used in case of staggered) is for the density model segy file.
* An example for a valid models configuration file :
```
data/vel_z6.25m_x12.5m_exact.segy
data/density_z6.25m_x12.5m.segy
```
### Traces Configuration File
* Traces file as indicated in the RTM configuration. A sample of this file is available in 'workloads/bp_model/traces.txt'.
* This is a simple file comprised of multiple lines:
    * First line contains the minimum shot id to start migration from. This is inclusive meaning the shot with the corresponding number will be processed. If we have no minimum limit wanted, write none on the first line.
    * Second line contains the maximum shot id to stop processing after. This is an inclusive meaning the shot with the corresponding number will be processed. If we have no maximum limit wanted, write none on the second line.
    * Multiple lines each having a trace file path.
* An example for a valid traces configuration file which will process shot 601 and 602 :
```
601
602
data/shots0001_0200.segy
data/shots0201_0400.segy
data/shots0401_0600.segy
data/shots0601_0800.segy
data/shots0801_1000.segy
data/shots1001_1200.segy
data/shots1201_1348.segy
```
### Callback Configuration
* Callback configuration file to produce intermediate files for visualization or value tracking. A sample of this file is available in 'workloads/bp_model/callback_configuration.txt'.
* Note: images will be generated only if opencv is enabled in the configurations(./config.sh -i on)
```
# Only enables the callback if value is yes
enable-image=no
enable-csv=no
enable-norm=no
enable-segy=no
enable-su=no
enable-binary=yes
## Format option
su.write_in_little_endian=no
## What to write for all write callbacks.
write_backward=no
write_forward=no
write_reverse=no
write_single_shot_correlation=no
write_each_stacked_shot=no
write_traces_raw=no
write_traces_preprocessed=no
write_re_extended_velocity=no
write_migration=yes
write_velocity=no
## Show-each
image.show_each=2000
csv.show_each=2000
norm.show_each=100
segy.show_each=2000
su.show_each=2000
binary.show_each=2000
## Percentile setting(value from 0 to 100).
image.percentile=98.5


```

* Note : for visualization of the segy/su files, the seismic unix can be used.
    * Eg: "segyread tape=file.segy | suximage perc=98.5" to visualize segy file with percentile of 98.5
    * Eg: "suximage <file.su perc=98.5" to visualize su file with percentile of 98.5
* Note : for visualization of the binary files, ximage can be used directly.
    * Eg: "ximage <file.bin n1=1951" to visualize a file where each trace length is 1951, notice in binary you'd need to provide the trace length in the command to visualize it accurately.
* Example : enable-csv enables writing snapshots and results in an ASCII format.
    * this is not encouraged and should be disabled as it is very inefficient in term of storage.
* Example : show_forwad stores snapshot of the wavefield during the forward propagation.
    * this will add overhead during the forward propagation, will takes storage dependent on the number of snapshots taken which is controlled by the show each.
---

## Results directory explanation
1.  **csv**: (comma-separated values) file format of the output folder.(to enable it make **enable-csv=yes** in callback configuration)
    * backward :includes the csv files for backward propagation at each number of time steps defined in the call back configuration (csv.show_each=2000)
    * forward :includes the csv files for forward propagation at each number of time steps defined in the call back configuration (csv.show_each=2000)
    * reverse :includes the csv files for reverse propagation at each number of time steps defined in the call back configuration (csv.show_each=2000)
        * In case of 2 propagation:it will include the reread data of forward propagation.
    * shots :includes the csv files of the correlation result of each shot separately.
    * stacked_shots :includes the csv files of the stacked correlation result of consecutive shots.
        * stacked_correlation_0.csv: the correlation of the first shot only 
        * stacked_correlation_1.csv: the stacking of the correlation results of the first and second shots.
    * traces: includes the csv files of the traces of each shot with some modifications if applied.
    * traces_raw: includes the csv files of the raw traces of each shot without any modifications .
    * velocities: includes the csv files of the forward and backward velocity with boundary extension for each shot.
    * migration.csv:the final migrated image in csv format.
    * velocity.csv:the used velocity model with boundary extension in csv format.
2.  **images**: png format of the output folder.(only enables if opencv is used and **enable-image=yes** in callback configuration)
    * backward :includes the png files for backward propagation at each number of time steps defined in the call back configuration (image.show_each=2000)
    * forward :includes the png files for forward propagation at each number of time steps defined in the call back configuration (image.show_each=2000)
    * reverse :includes the png files for reverse propagation at each number of time steps defined in the call back configuration (image.show_each=2000)
        * In case of 2 propagation:it will include the reread data of forward propagation.
    * shots :includes the png files of the correlation result of each shot separately.
    * stacked_shots :includes the png files of the stacked correlation result of consecutive shots.
        * stacked_correlation_0.png: the correlation of the first shot only 
        * stacked_correlation_1.png: the stacking of the correlation results of the first and second shots.
    * traces: includes the png files of the traces of each shot with some modifications if applied.
    * traces_raw: includes the png files of the raw traces of each shot without any modifications .
    * velocities: includes the png files of the forward and backward velocity with boundary extension for each shot.
    * migration.png:the final migrated image in png format.
    * velocity.png:the used velocity model with boundary extension in png format.
3.  **segy**: segy format of the output folder.(to enable it make **enable-segy=yes** in callback configuration)
    * backward :includes the segy files for backward propagation at each number of time steps defined in the call back configuration (segy.show_each=2000)
    * forward :includes the segy files for forward propagation at each number of time steps defined in the call back configuration (segy.show_each2000)
    * reverse :includes the segy files for reverse propagation at each number of time steps defined in the call back configuration (segy.show_each=2000)
        * In case of 2 propagation:it will include the reread data of forward propagation.
    * shots :includes the segy files of the correlation result of each shot separately.
    * stacked_shots :includes the segy files of the stacked correlation result of consecutive shots.
        * stacked_correlation_0.segy: the correlation of the first shot only 
        * stacked_correlation_1.segy: the stacking of the correlation results of the first and second shots.
    * traces: includes the segy files of the traces of each shot with some modifications if applied.
    * traces_raw: includes the segy files of the raw traces of each shot without any modifications .
    * velocities: includes the segy files of the forward and backward velocity with boundary extension for each shot.
    * migration.segy:the final migrated image in segy format.
    * velocity.segy:the used velocity model with boundary extension in segy format.
4.  **su**: su format of the output folder.(to enable it make **enable-su=yes** in callback configuration)
    * backward :includes the su files for backward propagation at each number of time steps defined in the call back configuration (su.show_each=2000)
    * forward :includes the su files for forward propagation at each number of time steps defined in the call back configuration (su.show_each2000)
    * reverse :includes the su files for reverse propagation at each number of time steps defined in the call back configuration (su.show_each=2000)
        * In case of 2 propagation:it will include the reread data of forward propagation.
    * shots :includes the su files of the correlation result of each shot separately.
    * stacked_shots :includes the su files of the stacked correlation result of consecutive shots.
        * stacked_correlation_0.su: the correlation of the first shot only 
        * stacked_correlation_1.su: the stacking of the correlation results of the first and second shots.
    * traces: includes the su files of the traces of each shot with some modifications if applied.
    * traces_raw: includes the su files of the raw traces of each shot without any modifications .
    * velocities: includes the su files of the forward and backward velocity with boundary extension for each shot.
    * migration.su:the final migrated image in su format.
    * velocity.su:the used velocity model with boundary extension in su format.
5.  **raw_binary**: binary format of the output folder.(to enable it make **enable-binary=yes** in callback configuration)
    * backward :includes the binary files for backward propagation at each number of time steps defined in the call back configuration (binary.show_each=2000)
    * forward :includes the binary files for forward propagation at each number of time steps defined in the call back configuration (binary.show_each2000)
    * reverse :includes the binary files for reverse propagation at each number of time steps defined in the call back configuration (binary.show_each=2000)
        * In case of 2 propagation:it will include the reread data of forward propagation.
    * shots :includes the binary files of the correlation result of each shot separately.
    * stacked_shots :includes the binary files of the stacked correlation result of consecutive shots.
        * stacked_correlation_0.bin: the correlation of the first shot only 
        * stacked_correlation_1.bin the stacking of the correlation results of the first and second shots.
    * traces: includes the binary files of the traces of each shot with some modifications if applied.
    * traces_raw: includes the binary files of the raw traces of each shot without any modifications .
    * velocities: includes the binary files of the forward and backward velocity with boundary extension for each shot.
    * migration.bin:the final migrated image in binary format.
    * velocity.bin:the used velocity model with boundary extension in binary format.
6.  **filtered_migration.segy**: the segy format of the final migration after applying some filters to it.
7.  **timing_results.txt**: timings of different functions, also include Gpts/s , GB/s , Gflops/s of the different kernels.
8.  **norm**: includes the norm values for the forward, backward and reverse propagation in tsv(Tab Separated Values) format.
---
