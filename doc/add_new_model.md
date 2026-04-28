# Adding a new model

In order to add a new ML model a few things have to be considered:
- For which detector model(s) should this new model be applicable?
- What is the plan for running inference inside `ddsim`?
- Which parts of the existing infrastructure can I reuse?

Some of these questions can be answered without any consideration for *DDML* but
are entirely driven by the model itself, e.g.
- if the model cannot be exported via torch or onnx, running via the *embedded
  python inference* is the only way.
- the detector / calorimeter that has been used for training the model probably
  also defines rather strong constraints onto where the model can be applied

## Main components for adding new models

Addig a new model (plugin) usually consists of taking some existing parts and
combining them with new parts. All model (plugins) are defined in
`MLModelActions.cc` where the minimal requirements are to define a *type* for
the plugin and to tell DD4hep about that type, i.e.

```cpp
typedef FastMLShower<FastMLModel<ddml::TorchInference, ddml::RegularGridGANModel, ddml::EndcapGeometry>>
    RegularGridGANEndcapTorchModel;

DECLARE_GEANT4ACTION_NS(ddml, RegularGridGANEndcapTorchModel)
```

declares the `RegularGridGANEndcapTorchModel` as a model that 
- uses the `TorchInference` class for running inference by loading a torch
  (jitted) model,
- uses the `RegularGridGANModel` to prepare the inputs and convert the outputs
- which are pre- and post-processed by the `EndcapGeometry` (which in this case
  works for ILD)

Optionally, one could pass in a *Trigger* as a 4th template argument. The main
purpose of that is to exclude certain geometrical regions as many other checks
(e.g. particle and energy) are already checked earlier (inside DD4hep).

### *ModelInterface*

Assuming that you are targetting an existing detector geometry (or a calorimeter
thereof) the main thing that needs to be added is a `Model` class that fulfills
the `ModelInterface` concept, i.e. it nees to have the following methods
- `prepareInput` - which extracts the (input) conditining from the particle and
  puts that into the input tensor that will be passed to the actual ML model. It
  should also properly pre-size the output tensor and provide the correct shape
  information for the inputs
- `convertOutput` - which gets the outputs of the ML model and converts them
  into (a vector of) `SpacePoints` (i.e. `(X, Y, Z), E`, and optionally `T`).
  This output will be consumed by a corresponding `Geometry` class so that
  essentially defines the structure (e.g. for CALICE like calorimeter, all
  SpacePoints are grouped into the calorimeter layers and the Geometry will assign the corresponding `z` position)
- `declareProperties` - The way to expose properties that should be configurable
  from the python side
