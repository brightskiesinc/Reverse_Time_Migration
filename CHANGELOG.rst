==============================================
Seismic Toolbox Release Notes
==============================================


v4.1.0
=======

**Added**:

* Added ``BSTimer`` package.
* Do a full ``BSTimer`` library integration with the Seismic Toolbox, updating the timing utilities to be fully dependent on ``BSTimer`` and remove old library dependency.
* Added new random boundary approach.
* Added support to staggered first order computation kernel for DPC++.
* Added support to staggered first order CPML boundary manager for DPC++.
* Added support to staggered first order computation kernel for OpenMP Offload.
* Added support to second order and staggered first order CPML boundary manager for OpenMP Offload.
* Added backend unification module for easing development process for different backend.
* Updated BS IO submodule's HEAD, and change codebase accordingly.
* Added ``BSBase`` package.
* Completely deprecated ``Helpers`` submodule.
* Added new Random Boundaries approach to all three technologies (OpenMP, OpenMP Offload and OneAPI).


v4.0.0
=======

**Added**:

* Added Two propagation features to OneAPI.
* Added CFL condition to OpenMP Offload.
* Added Two propagation features to OpenMP Offload.
* Added support to none, sponge, and second order CPML boundary manager for OpenMP Offload.
* Added ZFP compression feature to OpenMP Offload and OneAPI.

v3.0.2
=======

**Added**:

* Do a full I/O (Thoth) library integration with the Seismic Toolbox, updating the read/write utilities to be fully dependent on Thoth and remove old library dependency.
* Update modelling to depend on the I/O (Thoth) library as well.
* Added source maximum frequency resampling and user specified maximum frequency amplitude percentage.
* Added dip angle applying before stacking in correlation kernel.
* Added boundary managers tests:
    * None boundary manager
    * Random boundary manager
    * Sponge boundary manager
    * CPML boundary manager


v3.0.1
=======

**Fixed**:

* CMake now doesn't specify a device for OneAPI
* First touch is now specific to cpu.
* Corrected the computational grid size.
* Fix typo in OneAPI generator.


v3.0.0
=======

**Added**:

* Added CFL condition to OpenMP.
* Added CFL condition to OneAPI.
* Added tests for all components.
* Added Interpolator feature.
* Added Sampler feature.
* Added Compressor feature.
* Added `Thoth` (I/O) library to existing code (N.B. Old I/O still included and should completely be deprecated in later releases)
* Modified some existing features to scale up the code on clusters.
* Renewed `Helpers` library.
* Reintroduced `Generators` module.

**Fixed**:

* Fixed MPI in OpenMP.
* Fixed MPI in OneAPI.
* Fixed bugs in `CorrelationKernel`.
* Fixed bugs in `TraceManager`.
* Fixed `CrossCorrelation`'s compensation.
* Fixed `TwoPropagation`.
* Fixed `TwoPropagation` w/Compression.


v2.2.2
=======

**Added**:

* Added shots stride for trace reading.


v2.2.1
=======

**Added**:

* Removed ZFP submodule.
* Renewed `Helpers` library
* Changed libraries from `SHARED` to `STATIC`

**Fixed**:

* Fixed testing directives.


v2.2.0
=======

**Added**:

* Restructured CMake.
* Added testing structure.
* Restructured Seismic Toolbox library to abid to `Google C++ Style Guide`_
* Introduced tools directory. Includes various standalone tools to help ease testing.
    * Comparator
    * Convertor
    * Generator
* Split parsers to parsers and generators for better backward compatibility.
* Reintroduced configurations structure.
* Reintroduced configurations data structures in code.
* Added user manual (Initial).


v2.1.0
=======

**Added**: 

* Added ``compile`` stage in CI/CD.
* Added ``prerequisites`` folder.

**Fixed**:

* Fixed OneAPI codebase (`#3`_)
* Fixed MPI bug in all variants (`#5`_)
* Fixed OpenMP ``ReversePropagation`` bug (`#7`_)


v2.0.0
=======

**Added**:

* OpenMP working
* Wave Approximations
    * Isotropic First Order
    * Isotropic Second Order
* Migration Accommodators
    * Cross Correlation
* Boundary Conditions
    * CPML (*Isotropic First Order / Isotropic Second Order*)
    * Sponge (*All*)
    * None (*All*)
    * Random (*All*)
* Forward Collectors
    * Reverse Propagation
    * Two Propagation
*  Model Handlers
    * Seismic Model Handler
    * Synthetic Model Handler
* Source Injectors
    * Ricker Source Injector

**Bugs**:

*  OneAPI broken
*  Modeller needs some final tweaks



.. _`Google C++ Style Guide`: https://google.github.io/styleguide/cppguide.html#Run-Time_Type_Information__RTTI_).
