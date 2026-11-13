# Running AllShowers

Explanation of how to run AllShowers in DDML.

## DDML setup

This is the same as what you would do if you followed the main `README.md`, with some minor notes on acceptable versions.

- Clone this repo
- In an ideal world we would chose some fixed version of key4HEP rather than use the nightly (with the nightly, you'd have to rebuild when it changes), for example `source /cvmfs/sw.hsf.org/key4hep/setup.sh -r YYYY-MM-DD`. However, to get the sparse attention mask to work, you have to have a sufficiently recent version of torch, at least torch 2.13. At the time of writing, this is only available on the nightly, however, it is anticipated that a new tag will fix this very soon.
- Build and install as normal (`mkdir build && cd build && cmake .. && make && make -j4 install`)
- Source the install (`source ../install/bin/thisDDML.sh`)
- Add things to the PYTHONPATH (`export PYTHONPATH=${PATH_TO_THIS_DDML}/python:${PATH_TO_THIS_DDML}/python/examples:$PYTHONPATH`)

## AllShowers setup

This assumes you did the DDML setup and have that env active. You need a local install of AllShowers. Lets say you want it in `/path/to/my/python/libraries/`, so you `export MY_PYTHON_LIBRARIES=/path/to/my/python/libraries`.

- Create the path and add to the PYTHONPATH; `mkdir -p ${MY_PYTHON_LIBRARIES} && export PYTHONPATH=${MY_PYTHON_LIBRARIES}:$PYTHONPATH`
- Install a commit of AllShowers that points to the unified sampling branch; `pip install --no-deps --target ${MY_PYTHON_LIBRARIES} git+https://github.com/FLC-QU-hep/AllShowers@05a458a1b7de0cb0e217266cecc13d42942c1c37`. Gotta use `--no-deps` to prevent AllShowers from getting it's own torch version.
- Install a corresponding commit of PointCountFM `pip install --no-deps --target=${MY_PYTHON_LIBRARIES} git+https://github.com/FLC-QU-hep/PointCountFM@a0a11c617ae79680a5426cb55afec18a1a32fb6e`
- Install the minimal dependencies needed to run this. `pip install --no-deps --target ${MY_PYTHON_LIBRARIES} showerdata pot rich`.
- Check if you can do `python -c "import AllShowers"`.

Note, if the local disk is small, it's advisable to also use the `--cache-dir` flag on pip. Set it to any location you have space, you can nuke it after the install.

## When rerunning

If you come back to this after installing and want to run again, you need to set the env up again;

- Source the same version of key4hep (e.g. `source /cvmfs/sw.hsf.org/key4hep/setup.sh -r 2026-04-08`, or `source setup_env_nightly.sh`)
- Source the install (`source ../install/bin/thisDDML.sh`)
- Add the python installs dir and the DDML dirs back to the PYTHONPATH (`export PYTHONPATH=/path/to/DDML/python:/path/to/DDML/python/examples:/path/to/my/python/libraries:$PYTHONPATH`)

## To run

Enter the scripts directory to find the steering files, and do;

`ddsim --steeringFile ddsim_steer_photon_gun_allshowerspy.py  --compactFile $k4geo_DIR/ILD/compact/ILD_l5_v02/ILD_l5_v02.xml`

- `ddsim_steer_photon_gun_allshowerspy.py` will only work if everything is built and run on some version with torch >= 2.13. For now that's the nighlies.
- `ddsim_steer_photon_gun_cc3py.py` is tested and confirmed to work on 2026-04-08
