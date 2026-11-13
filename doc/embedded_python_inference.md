# Embedded Python inference

The embedded Python inference backend (`ddml::EmbeddedPyInference`) runs ML
inference by calling a user-supplied Python function from inside the Geant4
fast simulation. It is an alternative to the C++ `TorchInference` and
`ONNXInference` backends and is intended for models that

- cannot (easily) be exported to TorchScript or ONNX,
- need Python-side pre- or post-processing that is impractical to port to C++,
- or are still under active development, where re-exporting after every change
  is a burden.

The backend only replaces the *inference* step. Preparing the inputs from the
`G4FastTrack` (`Model::prepareInput`), all other steps are still run through
their respective c++ counterparts. Hence, those also define the handing and
pre-processing of the inputs and outputs.

## Building

The backend is controlled by a CMake option, which is `ON` by default:

```bash
cmake -DDDML_ENABLE_EMBEDDED_PYINFERENCE=ON ..
```

When enabled, it requires

- Python 3 (>= 3.9) including the development headers/library and NumPy
  (`find_package(Python3 3.9 COMPONENTS Interpreter Development NumPy)`),
- pybind11 (>= 2.10, found via its CMake config).

## Architecture

`EmbeddedPyInference` satisfies the same `InferenceInterface` concept as the
other backends, so it slots into the `FastMLModel` template like any other
inference implementation, e.g.

```cpp
typedef FastMLShower<FastMLModel<ddml::EmbeddedPyInference, ddml::CaloCloudsTwoAngleModel,
                                 ddml::PolyhedraBarrelGeometry, ddml::OctogonalBarrelTrigger>>
    CaloCloudsTwoAngleModelPolyhedraBarrelPyEmbeddedModel;
```

Most importantly the Python module that does the inference is not part of the
plugin type. As long as the input and outputs are produced as expected by the
rest of the complete model exchanging the actual ML model through the python
module can be done without re-compiling, similar to the other inference
backends.

## Configuration

### Plugin properties

| Property | Type | Default | Description |
|---|---|---|---|
| `PythonModule` | `string` | *(empty, required)* | Name of the module to import, e.g. `cc3_sf_2a_wrapper`. This is a module name for `import`, **not** a file path. Dotted names (`pkg.module`) work. |
| `EntryPoint` | `string` | `run_inference` | Name of the top-level callable in that module that is called for every shower. |

All other plugin properties (`RegionName`, `Detector`, `ApplicableParticles`,
`Etrigger`, model and geometry properties, ...) are the same as for the other
backends.

### Python module requirements

The module is imported with a regular Python `import`. It must be on `sys.path`,
i.e. usually via `PYTHONPATH`. **DDML does not modify `sys.path`**, so it is on
the user to ensure that this is setup properly.

## Lifecycle

The backend hooks into the standard `FastMLShower` lifecycle:

| Stage | When | What happens |
|---|---|---|
| Construction | Plugin is created in the steering file (`DetectorConstruction(kernel, "<Plugin>/<name>")`) | Ensures a Python interpreter exists (see [Interpreter](#interpreter)). Declares the `PythonModule` and `EntryPoint` properties. |
| `initialize()` | `FastMLShower::constructSensitives`, i.e. during Geant4 `ConstructSDandField()`, **before** the first event | Imports `PythonModule`, looks up `EntryPoint` and stores a reference to the callable. Module-level code runs here, so this is where model weights should be loaded. |
| `runInference()` | Once per triggered shower in `FastMLShower::modelShower`, between `Model::prepareInput` and `Model::convertOutput` | Wraps the inputs as NumPy arrays, calls the callable, copies the result into the output buffer. |
| Destruction | Plugin teardown | Drops the Python references while holding the GIL. The interpreter itself is never finalized. |

Initialization is eager. Configuration errors (module not found, missing entry
point, exceptions while importing) abort the simulation setup, not the first
event.

Per shower, the data flow is:

```
G4FastTrack ─► Model::prepareInput ─► InputVecs + TensorDimVecs ─┐
                                       (output pre-sized, zeroed) │
                                                                  ▼
                                      EmbeddedPyInference::runInference
                                        list[np.ndarray] ─► run_inference(...) ─► np.ndarray
                                                                  │  memcpy
                                                                  ▼
               Geometry::localToGlobal ◄─ Model::convertOutput ◄─ std::vector<float> output
```

## The Python contract

The entry point is called with a single positional argument and must return a
single array (assuming the default `EntryPoint`):

```python
def run_inference(inputs: list[np.ndarray]) -> np.ndarray: ...
```

### Inputs

- `inputs` is a `list` with one entry per input vector produced by the C++
  `Model::prepareInput`, in the same order.
- Each entry is a `float32` NumPy array whose shape is the corresponding entry
  of the `TensorDimVecs` set by `prepareInput`.
- The arrays are **zero-copy views** of the C++ `std::vector<float>` buffers.
  **They do not own their memory**.

Because of the zero-copy wrapping, the following rules apply:

1. **Do not modify the input arrays.** They are technically writeable, and
   writes go straight into DDML's input buffers. Copy first if you need to
   modify them (`a.copy()`). Note that `torch.from_numpy` also aliases the
   buffer.
2. **Do not keep references to the arrays (or tensors aliasing them) beyond
   the call.** The arrays and the list are cached and reused across showers.
   They are only rebuilt when the address of a buffer or its shape changes.
   A retained reference will therefore either observe the inputs of later
   showers or, if the C++ vector reallocated in the meantime, point to freed
   memory.

The list object passed to the function is the same object on every call (it is
updated in place), so do not mutate or store it either.

### Return value

The return value is converted with the equivalent of
`py::array_t<float, py::array::c_style>`, i.e. it must be convertible to a
C-contiguous `float32` array **without an unsafe cast**:

| Returned value | Result |
|---|---|
| `float32` `np.ndarray`, any shape | Accepted. Flattened in C order. |
| Non-contiguous `float32` array (e.g. a transposed view) | Accepted. Copied to C order first. |
| Python list of floats | Accepted. |
| `float64` or integer `np.ndarray` | **Rejected** with a `TypeError` (`Cannot cast array data from dtype('float64') to dtype('float32') according to the rule 'safe'`). |
| `None` (e.g. a forgotten `return`) | **Rejected** with a `TypeError` (`'<PythonModule>.<EntryPoint>' returned None, expected a float32 array (missing return statement?)`). |

A safe choice is

```python
return out.detach().cpu().numpy().astype(np.float32, copy=False).ravel()
```

### Output sizing

The output buffer is owned by DDML and pre-sized (and zero-filled) by
`Model::prepareInput`. The returned values are copied into the beginning of this
buffer:

- **Fewer values than the buffer size:** fine. The remaining entries stay
  zero. Variable-length outputs (like point clouds) rely on this.
- **More values than the buffer size:** a `WARNING` is printed and the output
  is **truncated** to the buffer size. The buffer is intentionally not
  enlarged, because sizing it is the responsibility of the C++ `Model`, and
  growing it silently could hide a configuration or model mismatch.

The layout of the values is entirely defined by the C++ `Model::convertOutput`
that consumes them.

## Interpreter

There is exactly one Python interpreter per process, shared by all plugin
instances:

- **Running under `ddsim`** (or any other Python host): the interpreter
  already exists (`Py_IsInitialized()` is true) and is reused. The model module
  runs in the same interpreter as the steering file, so it sees the same
  `sys.path`, environment and already-imported modules.
- **Running from a pure C++ host:** the first `EmbeddedPyInference` instance
  creates the interpreter (`pybind11::scoped_interpreter`). It is never
  finalized, because finalizing CPython after NumPy (and typically torch) have
  been loaded is unreliable.

Some consequences:

- **Modules are imported once.** Python caches imported modules in
  `sys.modules`, so if several plugins use the same `PythonModule` (as the
  barrel and endcap presets do), they share the same module object, including
  any model loaded at module level. This saves memory and loading time, but
  module-level state is shared too. Use separate modules (or separate entry
  points with separate state) if the plugins need different models.
- **Threading:** every call acquires the GIL, so calls from different threads
  are serialized at the Python level. In Geant4 multi-threaded mode, inference
  will not scale beyond what the GIL (and the library used inside the call)
  allows. MT mode is not covered by the tests.
  Intra-op threading is configured in the module.

## Writing a new Python model

1. Pick (or write) the C++ `Model` class that defines inputs and output
   layout, and a plugin type combining it with `EmbeddedPyInference`, a
   `Geometry` and optionally a `Trigger`
   (see [Adding a new model](add_new_model.md)). If an existing plugin matches,
   no C++ changes are needed.
2. Write a Python module along these lines:

   ```python
   import numpy as np

   # Runs once, at initialize() time: load weights, set threads, etc.
   _model = load_my_model()


   def run_inference(inputs):
       # inputs: list of float32 arrays, zero-copy views. Do not modify or keep.
       energy, theta, phi = (a.copy() for a in inputs)
       out = _model.generate(energy, theta, phi)
       return np.ascontiguousarray(out, dtype=np.float32).ravel()
   ```

3. Make the module importable (e.g. via adding it to `PYTHONPATH`).
4. Add a `ModelConfig` with `PythonModule` (and `EntryPoint` if it is not
   `run_inference`) in `plugin_properties`, either in
   `python/ddml/configs.py` or directly in a steering file.

### Pitfalls

- **Returning `float64`.** NumPy and many libraries default to double
  precision. Convert with `.astype(np.float32)` before returning.
- **Mutating or storing inputs.** See [Inputs](#inputs).
- **Exceeding the output buffer.** Watch for the
  `model returned more values than were pre-allocated` warning: the output is
  truncated, and the point counts at the beginning of the buffer no longer
  match the data.
- **Expensive work inside the entry point.** Anything that does not depend on
  the inputs (loading weights, building graphs, `torch.set_num_threads`, …)
  belongs at module level. The entry point is called once per shower.

## Testing

If the backend is enabled, the CTest suite contains `run_cc3_py_interface_ild`.
It runs 10 single-particle-gun events in ILD with the
`CC3_BARREL_PY_INTERFACE` and `CC3_ENDCAP_PY_INTERFACE` presets, with
`python/examples` added to `PYTHONPATH`. `check_test_outputs` then verifies that
the output file contains MC particles created by the fast simulation.

To compare timings with the C++ backends, build with
`-DDDML_INSTRUMENT_MODEL_SHOWER=1` added to the compile flags. Each
`FastMLShower` then writes `times_<plugin name>.root` with per-shower timings of
`prepareInput`, `runInference`, `convertOutput`, `localToGlobal` and the hit
maker.

## Limitations

- One interpreter per process. All Python models share it, the GIL and
  `sys.modules`.
- Batch size is whatever the C++ `Model` produces. Currently that is one shower
  per call. There is no batching across showers.
- No support for selecting a device or thread count from the plugin properties.
  This has to be handled in the Python module.
- Inputs are restricted to `float32` (`InputVecs` is
  `std::vector<std::vector<float>>`), and the output is a single flat `float32`
  array.
