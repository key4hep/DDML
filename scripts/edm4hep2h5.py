"""
Converts an EDM4HEP ROOT file to an HDF5 file, saving the shower energy in a 3D array and shower data in a 2D array.
Units: Energy values are stored in MeV and angles are stored in radians.

"""

#!/bin/env python
import sys
import argparse
import numpy as np
import os
import h5py
import ROOT


def parse_args(argv):
    p = argparse.ArgumentParser()
    p.add_argument(
        "--outputDir", "-o", type=str, default=".", help="Path to the output directory"
    )
    p.add_argument(
        "--inputFile",
        "-i",
        type=str,
        required=True,
        help="Name of the EDM4hep file to translate",
    )
    p.add_argument("--numR", type=int, default=9, help="Number of cells in R")
    p.add_argument("--numPhi", type=int, default=16, help="Number of cells in phi")
    p.add_argument("--numZ", type=int, default=45, help="Number of cells in z")
    p.add_argument(
        "--samplingFraction",
        type=float,
        default=1.0,
        help="Sampling fraction to use to rescale energy. Defined as f=active/(active+absorber)",
    )
    args = p.parse_args()
    return args


def main(argv):
    # Parse commandline arguments
    args = parse_args(argv)
    input_file = args.inputFile
    output_dir = args.outputDir
    # Number of cells in the r, phi & z directions
    num_cells_R = args.numR
    num_cells_phi = args.numPhi
    num_cells_z = args.numZ
    # Sampling fraction that rescales energy of each cell
    sampling_fraction = args.samplingFraction
    if os.stat(input_file).st_size > 0:
        h5_file = h5py.File(
            f"{output_dir}/{os.path.splitext(os.path.basename(input_file))[0]}.h5", "w"
        )
        print(
            f"Creating output file {output_dir}/{os.path.splitext(os.path.basename(input_file))[0]}.h5"
        )
        # Read EDM4HEP root file
        df = ROOT.RDataFrame("events", input_file)
        df2 = (
            df.Define("numMC", "return MCParticles.size();")
            .Filter("numMC==1")
            .Define(
                "energyCell",
                "std::vector<float> result; for (auto&p: ECalBarrelCollection) {result.push_back(p.energy*1.e3);} return result;",
            )
            .Define(
                "rhoCell",
                "std::vector<int> result; for (auto&p: ECalBarrelCollection) {result.push_back(p.position.x);} return result;",
            )
            .Define(
                "zCell",
                "std::vector<int> result; for (auto&p: ECalBarrelCollection) {result.push_back(p.position.z);} return result;",
            )
            .Define(
                "phiCell",
                "std::vector<int> result; for (auto&p: ECalBarrelCollection) {result.push_back(p.position.y);} return result;",
            )
            .Define(
                "energyMC",
                "ROOT::Math::XYZVector mom(MCParticles[0].momentum.x,MCParticles[0].momentum.y,MCParticles[0].momentum.z); return mom.R()*1.e3;",
            )
            .Define(
                "thetaMC",
                "ROOT::Math::XYZVector mom(MCParticles[0].momentum.x,MCParticles[0].momentum.y,MCParticles[0].momentum.z); return mom.Theta();",
            )
            .Define(
                "phiMC",
                "ROOT::Math::XYZVector mom(MCParticles[0].momentum.x,MCParticles[0].momentum.y,MCParticles[0].momentum.z); return mom.Phi();",
            )
        )
        arrays = df2.AsNumpy(
            columns=[
                "energyCell",
                "rhoCell",
                "zCell",
                "phiCell",
                "energyMC",
                "thetaMC",
                "phiMC",
            ]
        )
        print(arrays.keys())
        num_showers = len(arrays["energyMC"])
        #        for key in arrays.keys():
        #            arrays[key] = np.array(arrays[key])
        #            if key == "energyCell" or key == "zCell" or key == "rhoCell" or key == "phiCell":
        #                for event in range(len(arrays[key]):
        all_showers = []
        # loop over events
        for event in range(num_showers):
            # Initialize a 3D array with shape nb_events, nb_cells in x,y,z
            shower = np.zeros((num_cells_R, num_cells_phi, num_cells_z))
            for cell in range(len(arrays["rhoCell"][event])):
                if (
                    (arrays["rhoCell"][event][cell] < num_cells_R)
                    and (arrays["phiCell"][event][cell] < num_cells_phi)
                    and (arrays["zCell"][event][cell] < num_cells_z)
                ):
                    shower[arrays["rhoCell"][event][cell]][
                        arrays["phiCell"][event][cell]
                    ][arrays["zCell"][event][cell]] = (
                        arrays["energyCell"][event][cell] / sampling_fraction
                    )
            all_showers.append(shower)
        # Save dataset
        print(
            f"Creating datasets with shape 3x{np.shape(arrays['energyMC'])} and {np.shape(all_showers)} "
        )
        h5_file.create_dataset(
            "incident_energy",
            data=arrays["energyMC"],
            compression="gzip",
            compression_opts=9,
            dtype="f4",
        )
        h5_file.create_dataset(
            "incident_phi",
            data=arrays["phiMC"],
            compression="gzip",
            compression_opts=9,
            dtype="f4",
        )
        h5_file.create_dataset(
            "incident_theta",
            data=arrays["thetaMC"],
            compression="gzip",
            compression_opts=9,
            dtype="f4",
        )
        h5_file.create_dataset(
            "showers",
            data=all_showers,
            compression="gzip",
            compression_opts=9,
            dtype="f4",
        )
        h5_file.close()


if __name__ == "__main__":
    exit(main(sys.argv[1:]))
