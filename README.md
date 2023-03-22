# DDFastShowerML

Package with utilities and plugins that allow to run fast simulation in Geant4 from ML inference
within ddsim (DDG4).


## Installation 

Prerequisites:
- iLCSoft (or key4hep):
	- DD4hep, Geant4, podio, EDM4hep, LCIO, ...
- ONNXRuntime

- init software environment, e.g.:

```
. /usr/local/ilcsoft/HEAD/init_ilcsoft.sh
```

- then follow the usual cmake build procedure, e.g.:

```
mkdir build
cd build
cmake -C $ILCSOFT/ILCSoft.cmake ..
make -j 4 install
```
