#!/bin/bash

## commands to be run in singularity container

source setup_env_stable.sh;

### check build et al. are up to date and additional source
#cd build;
#cmake ..;
#make -j$(nproc) install;
#source install/bin/thisDDML.sh;
#cd ./scripts/;

cd ./ILDConfig/StandardConfig/production;

#Marlin MarlinStdReco_WITH_GAPS.xml --constant.lcgeo_DIR=$lcgeo_DIR --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=~/Tau_pi0_G4_files/Rec/G4_REC_tau_pi0_250-SetA_2f_leptonic_eL_pR --global.LCIOInputFiles=~/Tau_pi0_G4_files/G4_tau_pi0_250-SetA_2f_leptonic_eL_pR.slcio --constant.RunBeamCalReco=false;

#Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Tau_pi0_G4_files/Rec/G4_REC_tau_pi0_250-SetA_2f_leptonic_eL_pR --global.LCIOInputFiles=../../../Tau_pi0_G4_files/G4_tau_pi0_250-SetA_2f_leptonic_eL_pR.slcio --constant.RunBeamCalReco=false;

#### Reconstruction for two photon Tau samples
#Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Tau_pi0_G4_files/Rec/G4_REC_tau_pi0_twophotons_trigger_250-SetA_2f_leptonic_eL_pR_file_61 --global.LCIOInputFiles=../../../Tau_pi0_G4_files/G4_tau_pi0_twophotons_trigger_250-SetA_2f_leptonic_eL_pR_file_61.slcio --constant.RunBeamCalReco=false;

##### Reconstruction for two photon Tau samples with seed 2
#Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Tau_pi0_G4_files/Rec/G4_REC_tau_pi0_twophotons_trigger_250-SetA_2f_leptonic_eL_pR_file_61_SEED_2 --global.LCIOInputFiles=../../../Tau_pi0_G4_files/G4_tau_pi0_twophotons_trigger_250-SetA_2f_leptonic_eL_pR_file_61_SEED_2.slcio --constant.RunBeamCalReco=false;

#### Reconstruction for two photon Tau samples with seed 3
Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Tau_pi0_G4_files/Rec/G4_REC_tau_pi0_twophotons_trigger_250-SetA_2f_leptonic_eL_pR_file_61_SEED_3 --global.LCIOInputFiles=../../../Tau_pi0_G4_files/G4_tau_pi0_twophotons_trigger_250-SetA_2f_leptonic_eL_pR_file_61_SEED_3.slcio --constant.RunBeamCalReco=false;