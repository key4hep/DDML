# Running AllShowers

Explanation of how to run AllShowers in DDML.

## DDML setup

This is the same as what you would do if you followed the main `README.md`, with some minor notes on acceptable versions.

- Clone this repo
- In an ideal world we would chose some fixed version of Key4hep rather than use the nightly (with the nightly, you'd have to rebuild when it changes), for example `source /cvmfs/sw.hsf.org/key4hep/setup.sh -r YYYY-MM-DD`. However, to get the sparse attention mask to work, you have to have a sufficiently recent version of torch, at least torch 2.13. At the time of writing, this is only available on the nightly, however, it is anticipated that a new tag will fix this very soon.
- Build and install as normal (`mkdir build && cd build && cmake .. && make && make -j4 install`)
- Source the install (`source ../install/bin/thisDDML.sh`). This also puts the installed `ddml` python package and the `allshowers_wrapper` on the `PYTHONPATH`. The AllShowers model files are downloaded by the build and installed to `install/share/DDML/models`, where the wrapper finds them, so there is no need to add anything from the source tree to the `PYTHONPATH` (doing so would shadow the installed package; in that case `DDML_MODEL_DIR` has to point to a directory containing the models, e.g. `build/models`).

## AllShowers setup

This assumes you did the DDML setup and have that env active. AllShowers, PointCountFM and the few dependencies they need on top of Key4hep are installed with [uv](https://docs.astral.sh/uv/) by sourcing

```
source scripts/setup_allshowers.sh [/path/to/my/python/libraries]
```

The packages are installed into the given directory (default: `${ALLSHOWERS_INSTALL_DIR}` if set, otherwise `allshowers_libs` in the current directory), which is also put on the `PYTHONPATH`. Only packages that are not yet present in that directory are installed, so sourcing the script again with the same directory just sets up the environment. Everything is installed with `--no-deps` to prevent AllShowers from getting its own torch version.

The installed commits can be changed via `ALLSHOWERS_REF` and `POINTCOUNTFM_REF`. If the local disk is small, point `UV_CACHE_DIR` to any location where you have space; you can nuke it after the install.

## When rerunning

If you come back to this after installing and want to run again, you need to set the env up again;

- Source the same version of key4hep (e.g. `source /cvmfs/sw.hsf.org/key4hep/setup.sh -r 2026-04-08`, or `source setup_env_nightly.sh`)
- Source the install (`source ../install/bin/thisDDML.sh`)
- Source the AllShowers setup again with the same directory (`source scripts/setup_allshowers.sh /path/to/my/python/libraries`)

## To run

The steering files are in the `scripts` directory, but they can be run from anywhere, e.g. from within `scripts`;

`ddsim --steeringFile ddsim_steer_photon_gun_allshowerspy.py  --compactFile $k4geo_DIR/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml`

- `ddsim_steer_photon_gun_allshowerspy.py` will only work if everything is built and run on some version with torch >= 2.13. For now that's the nighlies.
- `ddsim_steer_photon_gun_cc3py.py` is tested and confirmed to work on 2026-04-08
