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
defined for which to run the fast ML shower simulation, e.g:


```diff
diff --git a/ILD/compact/ILD_common_v02/SEcal06_hybrid_Barrel.xml b/ILD/compact/ILD_common_v02/SEcal06_hybrid_Barrel.xml
index 5ff2e50..65822af 100644
--- a/ILD/compact/ILD_common_v02/SEcal06_hybrid_Barrel.xml
+++ b/ILD/compact/ILD_common_v02/SEcal06_hybrid_Barrel.xml
@@ -1,7 +1,15 @@
 <lccdd>
+
+  <regions>
+    <region name="SimpleCaloRegion" eunit="MeV" lunit="mm" cut="0.001" threshold="0.001">
+      <limitsetref name="cal_limits"/>
+    </region>
+  </regions>
+
   <detectors>
 
-    <detector name="EcalBarrel" type="SEcal06_Barrel" id="ILDDetID_ECAL" readout="EcalBarrelCollection" vis="BlueVis" >
+    <detector name="EcalBarrel" type="SEcal06_Barrel" id="ILDDetID_ECAL" readout="EcalBarrelCollection" vis="BlueVis"
+     region="SimpleCaloRegion" limits="cal_limits">
 
       <comment>EM Calorimeter Barrel</comment>
```

- then run the simulation as usual w/ ddsim:

```
cd ./scripts
ddsim --steeringFile ddsim_steer.py --compactFile /usr/local/ilcsoft/HEAD/lcgeo/HEAD/ILD/compact/ILD_l5_o1_v05/ILD_l5_o1_v05.xml
```

- visualize the events

```
python /usr/local/ilcsoft/HEAD/CEDViewer/HEAD/bin/ced2go -d gear_ILD_l5_o1_v02.xml dummyOutput.slcio
```

