==============================================
BS Timer Release Notes
==============================================

v.1.0.1
=======

**Fixed**:

* Fixed tests random failure.
* Fixed GFLOPs reporting.


v.1.0.0
=======

**Added**:

* Added ``ElasticTimer``, ``ScopeTimer`` and ``LazyTimer`` for different purpose usages.
* Added support to different backends (i.e. OpenMP and CUDA) and support for serial version as well.
* Added ``StatisticsHelper`` for easing statistical calculations for timing results.
* Added ``Channel`` property.
* Added generic configurations addition to ``Manager`` component.
* Formatted stream (i.e. Console, file or any other stream) reporting.
* Flushing result as text files.
* Special purpose data file flushing for visualization via python module.
* Added example directory that includes end-to-end example for different backends and components.
* Added user manual.
* Added test files.
* Added ``BSBase`` package.
