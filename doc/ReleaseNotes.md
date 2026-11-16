# v00-04

* 2026-11-13 Henry Day-Hall ([PR#39](https://github.com/key4hep/DDML/pull/39))
  - Make it possible to also generate showers in the HCAL Endcaps by adding the corresponding layers

* 2026-11-13 Thomas Madlener ([PR#22](https://github.com/key4hep/DDML/pull/22))
  - Add a `EmbeddedPyInference` inference interface that allows to run model inference directly through an embedded python interpreter for ML models that cannot be exported to be loaded via ONNXruntime or Torch

* 2026-10-02 Thomas Madlener ([PR#19](https://github.com/key4hep/DDML/pull/19))
  - Add a `ddml` python module that provides `ddml_physics` as well as `add_shower_model` to streamline the setup of a fast sim model in ddsim steering files.
    - Plus some pre-configured configurations for included models

* 2026-10-01 Peter McKeown ([PR#34](https://github.com/key4hep/DDML/pull/34))
  - Add BIBAE two angle model for retrieval via `ExternalData`

* 2026-10-01 Thomas Madlener ([PR#33](https://github.com/key4hep/DDML/pull/33))
  - Make errors in model loading a bit easier to see from the log outputs

* 2026-10-01 Thomas Madlener ([PR#16](https://github.com/key4hep/DDML/pull/16))
  - Run the ML model initialization before the event loop
  - Complete the concepts to cover all the necessary functionality
  - Make sure to initialize the geometry late enough for DD4hep settings to be available
  - Default to non-hadron showers

* 2026-09-30 Thomas Madlener ([PR#31](https://github.com/key4hep/DDML/pull/31))
  - Switch to the the cmake targets that newer versions of onnxruntime provide (`onnxruntime::onnxruntime`) and remove the custom `FindOnnxRuntime.cmake` module. **This effectively mandates onnxruntime >= 1.16**.

* 2026-09-30 Thomas Madlener ([PR#30](https://github.com/key4hep/DDML/pull/30))
  - Start using the default `Key4hepConfig.cmake` for configuring builds the same way as all other Key4hep packages

* 2026-09-30 Thomas Madlener ([PR#28](https://github.com/key4hep/DDML/pull/28))
  - Switch to latest version of Key4hep CI

* 2026-09-30 Thomas Madlener ([PR#20](https://github.com/key4hep/DDML/pull/20))
  - Make sure to only copy data to the `output` that the model actually returns when running inference

* 2026-09-30 Thomas Madlener ([PR#18](https://github.com/key4hep/DDML/pull/18))
  - Make sure the example CaloClouds3 configuration is properly configured for the Endcap

* 2026-09-30 Thomas Madlener ([PR#17](https://github.com/key4hep/DDML/pull/17))
  - Add a very basic test setup that uses CaloClouds3 to verify that things run
  - "Fix" some reference counting issue by leaking a resource (with a proper fix deferred to the future)

# v00-03

* 2026-08-06 Juan Miguel Carceller ([PR#25](https://github.com/key4hep/DDML/pull/25))
  - Add a `doc/ReleaseNotes.md` file following the same structure as in [EDM4hep](https://github.com/key4hep/EDM4hep/tree/main/doc), listing the changes per released version based on the git history.

* 2026-06-16 Peter McKeown ([PR#24](https://github.com/key4hep/DDML/pull/24))
  - Update CaloClouds3 model

* 2026-04-28 Thomas Madlener ([PR#21](https://github.com/key4hep/DDML/pull/21))
  - Suppress a future cmake warning by providing a missing cmake argument

* 2026-04-23 Thomas Madlener ([PR#10](https://github.com/key4hep/DDML/pull/10))
  - Make all abstract base class interfaces concepts and use them instead, since we never use them in a way that would require abstract base classes
  - Remove the `HitMaker` as a customization point from the `FastMLModel` since we always use the same in any case at the moment.

* 2026-03-02 Thomas Madlener ([PR#9](https://github.com/key4hep/DDML/pull/9))
  - Switch to the `Geant4FastSimShowerModel::killParticle` method instead of re-implementing it
  - Remove unused boolean members that were remnants from a previous design

* 2026-02-05 Thomas Madlener ([PR#13](https://github.com/key4hep/DDML/pull/13))
  - Add the Apache 2 License text explicitly

* 2026-02-05 Thorsten Buss ([PR#12](https://github.com/key4hep/DDML/pull/12))
  - update title of `README.md`
  - add doi and build badge to `README.md`
  - add `CITATION.cff`

* 2025-12-18 Thomas Madlener ([PR#11](https://github.com/key4hep/DDML/pull/11))
  - Add workflows that build the package with instrumentation turned on
  - Fix build issues for instrumented builds

* 2025-11-26 Peter McKeown ([PR#7](https://github.com/key4hep/DDML/pull/7))
  - Use DD4hep logging facilities instead of printing to stdout and sterr directly

* 2025-11-25 Thomas Madlener ([PR#8](https://github.com/key4hep/DDML/pull/8))
  - Fix `clang-tidy` hook in `pre-commit` workflow

* 2025-11-25 Thomas Madlener ([PR#6](https://github.com/key4hep/DDML/pull/6))
  - Switch to the latest configuration of Key4hep for `.clang-format`
  - Add `modernize-use-equals-default` to the `clang-tidy` checks to enforce the usage of `= default` for empty constructors and destructors

* 2025-06-03 Thomas Madlener ([PR#2](https://github.com/key4hep/DDML/pull/2))
  - Add the centrally distributed github actions key4hep build workflow
  - Add a pre-commit workflow

# v0.2.0

* 2025-05-27 Thomas Madlener ([ilcsoft/ddfastshowerml!27](https://gitlab.cern.ch/ilcsoft/ddfastshowerml/-/merge_requests/27))
  - Specify ddfastshowerml as a CXX project

* 2025-03-26 Thomas Madlener ([ilcsoft/ddfastshowerml!24](https://gitlab.cern.ch/ilcsoft/ddfastshowerml/-/merge_requests/24))
  - Fix missing STL include

* 2025-02-17 Thorsten Lars Henrik Buss ([ilcsoft/ddfastshowerml!23](https://gitlab.cern.ch/ilcsoft/ddfastshowerml/-/merge_requests/23))
  - Add L2LFlows

# v0.1.1

* 2025-01-27 Peter McKeown ([ilcsoft/ddfastshowerml!22](https://gitlab.cern.ch/ilcsoft/ddfastshowerml/-/merge_requests/22))
  - Resolve "Unable to build in debug mode"

* 2024-11-14 Peter McKeown ([ilcsoft/ddfastshowerml!21](https://gitlab.cern.ch/ilcsoft/ddfastshowerml/-/merge_requests/21))
  - Merging CaloClouds 2A

* 2024-11-08 Peter McKeown ([ilcsoft/ddfastshowerml!19](https://gitlab.cern.ch/ilcsoft/ddfastshowerml/-/merge_requests/19))
  - Fix torch num threads

* 2024-11-07 Peter McKeown ([ilcsoft/ddfastshowerml!20](https://gitlab.cern.ch/ilcsoft/ddfastshowerml/-/merge_requests/20))
  - Update read me

# v0.1.0

* 2024-10-01 Peter McKeown ([ilcsoft/ddfastshowerml!15](https://gitlab.cern.ch/ilcsoft/ddfastshowerml/-/merge_requests/15))
  - Add example class for hdf5-based loading of shower library
