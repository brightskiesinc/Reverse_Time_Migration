==============================================
BSBase Release Notes
==============================================

v1.0.0
=======

Includes
----------------------

**Added**:

* Added logger module that supports multiple flushing streams (i.e. Console and file flushing).
* Added common module that includes the following features:
    * Different types of assertions.
    * Default definitions for general usages.
    * Default exit codes for general usages.
    * Singleton class to provide singleton design pattern.
* Added exceptions module to include the following exceptions:
    * ``AxisException``
    * ``DeviceNoSpaceException``
    * ``DeviceNotFoundException``
    * ``DirectionException``
    * ``FileNotFoundException``
    * ``IllogicalException``
    * ``IndexOutOfBoundsException``
    * ``NoKeyFoundException``
    * ``InvalidKeyValueException``
    * ``NotImplementedException``
    * ``NullPointerException``
    * ``UndefinedException``
    * ``UnsupportedFeatureException``
* Added configurations module to have configuration maps and interfaces to be implemented by any class to convert to a configurable one.
* Added memory module for different memory manipulation.
* Added tests for the desired modules.
* Added prerequisites directory that have all prerequisites libraries and files needed by different libraries.