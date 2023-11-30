#!/bin/bash

source setup_env_stable.sh;

### check build et al. are up to date and additional source
#cd build;
#cmake ..;
#make -j$(nproc) install;
#source install/bin/thisDDML.sh;
#cd ./scripts/;

cd ./ILDConfig/StandardConfig/production;

#Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Version_G4_check/DDML_data_G4/REC_irreg_DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1 --global.LCIOInputFiles=../../../Version_G4_check/DDML_data_G4/sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1.slcio --constant.RunBeamCalReco=false;

# BIBAE
#Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Version_G4_check/BIBAE_DDML/REC_BIBAE_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1 --global.LCIOInputFiles=../../../Version_G4_check/BIBAE_DDML/BIBAE_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1.slcio --constant.RunBeamCalReco=false;

# BIBAE 6by6 split
#Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Version_G4_check/BIBAE_DDML_6by6_split/REC_BIBAE_6by6_split_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1 --global.LCIOInputFiles=../../../Version_G4_check/BIBAE_DDML_6by6_split/BIBAE_6by6_split_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1.slcio --constant.RunBeamCalReco=false;

# BIBAE 8by8 split
Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Version_G4_check/BIBAE_DDML_8by8_split/REC_BIBAE_8by8_split_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1 --global.LCIOInputFiles=../../../Version_G4_check/BIBAE_DDML_8by8_split/BIBAE_8by8_split_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1.slcio --constant.RunBeamCalReco=false;

# BIBAE 6by6 split 1.5MIP threshold
#Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Version_G4_check/BIBAE_DDML_6by6_split_thresh_1p5MIP/REC_BIBAE_6by6_split_thresh_1p5MIP_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1 --global.LCIOInputFiles=../../../Version_G4_check/BIBAE_DDML_6by6_split_thresh_1p5MIP/BIBAE_6by6_split_thresh_1p5MIP_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1.slcio --constant.RunBeamCalReco=false;

# BIBAE 6by6 split position 2
#Marlin MarlinStdReco_WITH_GAPS.xml --constant.DetectorModel=ILD_l5_o1_v02 --constant.OutputBaseName=../../../Version_G4_check/BIBAE_DDML_6by6_split_position2/REC_BIBAE_6by6_split_poisition_2_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1 --global.LCIOInputFiles=../../../Version_G4_check/BIBAE_DDML_6by6_split_position2/BIBAE_6by6_split_poisition_2_sim-Irreg-DDML_G4_VERSION_CHECK-E5050A-theta9090A-phi9090-p1.slcio --constant.RunBeamCalReco=false;