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


## Running the example

For running the example with the ILD detector there has to be a region
defined for which to run the fast ML shower simulation.
For example modify in your local copy of lcgeo/k4geo:


```diff
diff --git a/ILD/compact/ILD_common_v02/SEcal06_hybrid_Barrel.xml b/ILD/compact/ILD_common_v02/SEcal06_hybrid_Barrel.xml
index 5ff2e50..084f7ea 100644
--- a/ILD/compact/ILD_common_v02/SEcal06_hybrid_Barrel.xml
+++ b/ILD/compact/ILD_common_v02/SEcal06_hybrid_Barrel.xml
@@ -1,7 +1,14 @@
 <lccdd>
+
+  <regions>
+    <region name="EcalBarrelRegion">
+    </region>
+  </regions>
+
   <detectors>
 
-    <detector name="EcalBarrel" type="SEcal06_Barrel" id="ILDDetID_ECAL" readout="EcalBarrelCollection" vis="BlueVis" >
+    <detector name="EcalBarrel" type="SEcal06_Barrel" id="ILDDetID_ECAL" readout="EcalBarrelCollection" vis="BlueVis"
+     region="EcalBarrelRegion">
 
       <comment>EM Calorimeter Barrel</comment>
 
```

- this model has to be activated in the `ddsim_steer.py` file:

```python
def aiDance(kernel):
   from g4units import GeV, MeV  # DO NOT REMOVE OR MOVE!!!!! (EXCLAMATION MARK)
   from DDG4 import DetectorConstruction, Geant4, PhysicsList
   geant4 = Geant4(kernel)
   
   seq, act = geant4.addDetectorConstruction('Geant4DetectorGeometryConstruction/ConstructGeo')
   act.DebugMaterials = True
   act.DebugElements = False
   act.DebugVolumes = True
   act.DebugShapes = True

   # Apply sensitive detectors
   sensitives = DetectorConstruction(kernel, str('Geant4DetectorSensitivesConstruction/ConstructSD'))
   sensitives.enableUI()
   seq.adopt(sensitives)

   model = DetectorConstruction(kernel, str('FancyMLShowerModel/ShowerModel'))

##   # Mandatory model parameters
   model.RegionName = 'EcalBarrelRegion'
   model.Enable = True
   # Energy boundaries are optional: Units are GeV
   model.ApplicableParticles = {'e+','e-','gamma'}
   model.Etrigger = {'e+': 5. * GeV, 'e-': 5. * GeV, 'gamma': 5. * GeV}
   model.ModelPath = "../models/francisca_gan.onnx"

   model.enableUI()
   seq.adopt(model)

   # Now build the physics list:
   phys = kernel.physicsList()
   ph = PhysicsList(kernel, str('Geant4FastPhysics/FastPhysicsList'))
   ph.EnabledParticles = ['e+', 'e-','gamma']
   ph.BeVerbose = True
   ph.enableUI()
   phys.adopt(ph)
   phys.dump()

SIM.physics.setupUserPhysics( aiDance)
```



- then run the simulation as usual w/ ddsim:

```
cd ./scripts
ddsim --steeringFile ddsim_steer.py --compactFile /usr/local/ilcsoft/HEAD/lcgeo/HEAD/ILD/compact/ILD_l5_o1_v02/ILD_l5_o1_v02.xml
```

- visualize the events

```
python /usr/local/ilcsoft/HEAD/CEDViewer/HEAD/bin/ced2go -d gear_ILD_l5_o1_v02.xml dummyOutput.slcio
```

