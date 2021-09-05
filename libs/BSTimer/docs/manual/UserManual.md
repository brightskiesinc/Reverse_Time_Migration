# User Manual

### Using Elastic Timer

###### Creating Timer

```cpp
auto timer = new ElasticTimer("Function Name");
```

###### Creating Timer for kernel

```cpp
auto timer = new ElastingTimer("Function Name", Grid Size,
Number of arrays,
Single precision flag,
Number of operations);
```

###### Starting Timer

```cpp
timer->Start();
```

###### Stopping Timer

```cpp
timer->Stop();
```

### Using Scope Timer

```cpp
{
...
ScopeTimer timer("Function Name");
///Block of code to be timed
}
```

### Using Lazy Timer

```cpp
LazyTimer::Evaluate([&]() {
/*Block of code to be timed*/
}, Print to console flag);
```

### Configuring Manager

```cpp
TimerManager::GetInstance()->Configure(JSONConfigurationMap);
```

### Reporting

```cpp
TimerManager::GetInstance()->Report(Print to console flag,
Export report to text file flag,
Filename);
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

