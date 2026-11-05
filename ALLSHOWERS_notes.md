# Running AllShowers

Some notes on what to do to run AllShowers.

## DDML setup

- Clone this repo
- Chose some fixed version of key4HEP (do not use the nightly.... then you'd have to rebuild when it changes). For example `source /cvmfs/sw.hsf.org/key4hep/setup.sh -r 2026-04-08`.
- Build and install as normal (`mkdir build && cd build && cmake .. && make && make -j4 install`)
- Source the install (`source ../install/bin/thisDDML.sh`)

## AllShowers setup

This assumes you did the DDML setup and have that env active. You need a local install of AllShowers. Lets say you want it in `/path/to/my/python/libraries/`.

- Create the path and add to the PYTHONPATH; `mkdir -p /path/to/my/python/libraries && export PYTHONPATH=/path/to/my/python/libraries:$PYTHONPATH`
- Install a commit of AllShowers that points to the unified sampling branch; `pip install --target /path/to/my/python/libraries git+https://github.com/FLC-QU-hep/AllShowers@05a458a1b7de0cb0e217266cecc13d42942c1c37`. This may give an error, but check if you can do `python -c "import AllShowers"`, sometimes it secretly worked anyway.

Note, if the local disk is small, it's advisable to also use the `--cache-dir` flag on pip. Set it to any location you have space, you can nuke it after the install.

## When rerunning

If you come back to this after installing and want to run again, you need to set the env up again;

- Source the same version of key4hep (e.g. `source /cvmfs/sw.hsf.org/key4hep/setup.sh -r 2026-04-08`)
- Source the install (`source ../install/bin/thisDDML.sh`)
- Add the python installs dir back to the PYTHONPATH (`export PYTHONPATH=/path/to/my/python/libraries:$PYTHONPATH`)

