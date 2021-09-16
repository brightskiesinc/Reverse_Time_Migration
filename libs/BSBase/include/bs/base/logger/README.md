# Logger Module

<p>
spdlog is an open source, very fast, header only, C++ logging library with code address at https://github.com/gabime/spdlog  , the latest release version is 0.14.0. It provides the ability to output logs to streams, standard output, files, system logs, debuggers and other targets. It supports platforms including Ubuntu, Windows, Linux, Mac, Android.
</p>

## Logger Features

* Very fast, performance is its main goal.
* Only include header files.
* Enable Automatic Creating Log Files.
* Optional printf syntax support.
* Very fast asynchronous mode (optional), support asynchronous write log.
* System format.
* Multi-threaded / Single-threaded log.

## Module Architecture

### `Logger`

The interface class that has the pure virtual interface for the functions and data types * Data types :

```c++ 
enum Pattern {
    TIME,
    DATE_TIME,
    THREAD,
    DATE_TIME_THREAD
};
```

```c++ 
enum WriterMode {
    FILE_MODE, 
    CONSOLE_MODE, 
    FILE_CONSOLE
}; 
```

```c++ 
enum Operation{
    INFO,
    ERROR,
    CRITICAL,
    DEBUG
};
```   

#### Functions

* `Operate(std::string aStatement, Operation aOperation)`: Implement the needed operation for logging
* `WriterMode GetType()`:Returns the write mode.
* `void Configure(std::string aLoggerName)`: Configure the type of the ```sink``` and create ``` logger```
* `SetPattern (Pattern)`: Check the user format needed to print

### ```General Logger```

This class is inherited from the Logger interface * Data types :

```c++
/// The logger pointer
std::shared_ptr<spdlog::logger> mpLogger;
```

#### Functions

* `SetPattern (Pattern)`: Check the user format needed to print

### ```Console Logger ```

The class is inherited from the General Logger class :

#### Functions

* `Operate(std::string aStatement, Operation aOperation)`: Implement the needed operation for logging
* `WriterMode GetType()`: Return the write mode.
* `Configure (mLoggerName)`: Configure the type of the ```sink``` and create ``` logger```

### ```File Logger```

The class is inherited from the General Logger class :

```c++
/// The filepath
std::string mFilePath;
/// The statement
std::string mStatement;
/// The file stream
std::ofstream mFileStream;
```

#### Functions

* `Operate(std::string aStatement, Operation aOperation)`: Implement the needed operation for logging
* `WriterMode GetType()`: Return the write mode.
* `Configure (mLoggerName)`: Configure the type of the ```sink``` and create ``` logger```
* `static int HandleFilePath(const std::string &aFilePath)`:Handling path for the output file.

### `Logger Collection`

This is a wrapper class, that has all the loggers registered to the system but in a single object,

```c++
/// Vector of pointers to the available loggers
std::vector<Logger *> mpLoggersVector;
/// The logger mode
WriterMode mMode;
```

#### Functions :

* `RegisterLogger(Logger)`: Creates new object from ConsoleLogger or FileLogger and add it to ```Logger vector```
* `ConfigureLoggers(aLoggerName, aMode, aPattern)`: Handling the logger requirements
    * Select a Mode from ```WriterModes```, It's an optional parameter with ```FILE_CONSOLE``` as a default value
    * Select a pattern from ```Pattern ```, It's an optional parameter with ```DATE_TIME``` as a default value
* `Operate(std::string aStatement, Operation aOperation)`: Implements the needed operation for logging

### `Logger System`

The class implement the APIs to use the logger Module

```c++
///Instance of LoggerCollection Interface
LoggerCollection mpCollection;
///Vector of pointers to the different logger channels
std::vector <LoggerChannel*> mpLoggerChannels;
```

#### Functions :

* `RegisterLogger(Logger)`: Wrapper for the RegisterLogger function of the Logger collection.
* `ConfigureLoggers(aLoggerName, aMode, aPattern)`: Wrapper for the ConfigureLoggers function of the Logger collection.
* `LoggerChannel& Info()`: Chooses the Info logger channel.
* `LoggerChannel& Error()`: Chooses the Error logger channel.
* `LoggerChannel& Critical()`: Chooses the Critical logger channel.
* `LoggerChannel& Debug()`: Chooses the DEbug logger channel.

### `Logger Channel`

- This class is responsible for getting the message and logging it using the logger collection contained in the logger
  system.
- Each level info/error/etC having its own logger channel object.

```c++
/// The string stream that holds the input data
std::stringstream mStringStream;
///The Operation type
Operation mOperationType;
///Pointer to the LoggerCollection Interface
LoggerCollection* mpCollection;
```

## Usage

* `CMakeList.txt` files
    * Static lib & Ubuntu static version
    ```cmake
    if(NOT TARGET spdlog)
        find_package(spdlog REQUIRED)
    endif()
    ```

* Use the loggerCollection interface
    * Create an object from logger System
    * Call RegisterLogger function to pass the type of logger
    * Call ConfigureLoggers function to select the logger configurations
    * Call the severity level functions

    ```c++
    /* Creating the logger System. */
    LoggerSystem ls;

    /* Registering the needed loggers. */
    ls.RegisterLogger(new FileLogger("./Output1.txt"));
    ls.RegisterLogger(new ConsoleLogger());

    /* Configuring the logger. */
    ls.ConfigureLoggers("Logger1",FILE_CONSOLE);

    /* Testing different functionalities
    ls.Info()<<"Hello I'm the First info string , it appears in file & console with the default format ";
    ls.Error()<<"Hello I'm the First Error statement ,it appears in file & console with the default format ";
    ls.Critical()<< "Hello I'm the First critical statement , it appears in file & console with the default format ";
    ls.Debug()<< "Hello I'm the First Debug statement , it appears in file & console with the default format ";

    /* Registering a new output file. */
    ls.RegisterLogger(new FileLogger("./Output2.txt"));
    /* Configuring the new logger
    ls.ConfigureLoggers("Ostream",FILE_MODE);

    /* Testing different functionalities. */
    ls.Info()<<"Hello I'm the Ostream info string , it appears in file format only ";
    ls.Error()<<"Hello I'm the Ostream Error statement ,it appears in file format only";
    ls.Critical()<< "Hello I'm the Ostream critical statement , it appears in file format only";
    ls.Debug()<< "Hello I'm the Ostream Debug statement , it appears in file format only";


    /* Configuring the new logger. */
    ls.ConfigureLoggers("Console",CONSOLE_MODE);

    /* Testing different functionalities. */
    ls.Info()<<"Hello I'm the Console info string , it appears in Console format only ";
    ls.Error()<<"Hello I'm the Console Error statement ,it appears in Console format only";
    ls.Critical()<< "Hello I'm the Console critical statement , it appears in Console format only";
    ls.Debug()<< "Hello I'm the Console Debug statement , it appears in Console format only";

    ```
