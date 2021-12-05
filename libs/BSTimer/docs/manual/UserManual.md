# User Manual

User manual for how to use BS Timer.

## Timer Variants

BS Timer library includes different variants of timers for different purposes, each one will be discussed in more
details in later part in this document. Variant available are `ElasticTimer`, `ScopeTimer` and `LazyTimer`. All timers
support timing for various backends, they also support timing for both CPU (a.k.a. Host) and GPU (a.k.a. Device)

### Elastic Timer

Used when functions or kernels are long or when wanting to start timing at a scope and ending it at another one.

#### Creating Elastic Timer

Creating `ElasticTimer` can go with two different approaches; one in which only a function name is provided and that
when timing normal functions. Another way for constructing an `ElasticTimer` is by providing extra information for
bandwidth calculations and this is used for kernels timing.

**Default Constructor:**

```cpp
auto timer = new ElasticTimer(<function-name>, <target-flag>[OPTIONAL]);
```

**Elaborative Constructor:**

```cpp
auto timer = new ElasticTimer(<function-name>, <grid-size>, <number-of-arrays>, <single-precision-flag>, <number-of-operations>, <target-flag>[OPTIONAL]);
```

**Using Elastic Timer:**

```cpp
timer->Start();

/* Block of code to be timed. */

timer->Stop();
```

### Scope Timer

Used when functions or kernels are a bit small and available in only one scope. Pros of using such variant is that it
gets auto destructed by exiting scope and that no need for starting or ending functions here.

```cpp
{

/* Block of code that won't be timed. */

ScopeTimer timer(<function-name>);

/* Block of code to be timed. */

}
```

### Lazy Timer

Used when functions or kernels are more of a scope functions. Usually used for tests purposes only. Note that codes
timed by `LazyTimer` won't be reported by streams.

```cpp
LazyTimer::Evaluate([&]() {

/* Block of code to be timed. */

}, <show-results-flag>[OPTIONAL], <time-unit>[OPTIONAL], <target-flag>[OPTIONAL]);
```

### Configuring Manager

```cpp
TimerManager::GetInstance()->Configure(JSONConfigurationMap);
```

### Reporting

```cpp
TimerManager::GetInstance()->Report(std::ostream &aOutputStream);
```

Or

```cpp
TimerManager::GetInstance()->Report(std::vector< std::ostream *> &aStreams);
```

## Building

### Building OpenMP Version

* Using Intel Compiler
    1. Source Intel parallel studio to be able to use ```icpc``` command.
    2. Run configuration script.
    ```shell script
    ./config.sh -b omp 
    ```
    3. Run the building script.

    ```shell script
    ./clean_build.sh
    ```
* Using gcc compiler
    1. Run configuration script.
    ```shell script
    ./config.sh -b omp -g 
    ```

    2. Run the building script.
    ```shell script
    ./clean_build.sh
    ```

## Running Python Module

#### Prerequisite

Setting up environment for python module

```shell script
cd <path-to-python-module>
./setup_enviroment.sh
```

#### Running

```shell script
python3 <path-to-main.py> -i <path-to-input-file> -o <path-to-output-dir>
```
