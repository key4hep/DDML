#!/usr/bin/env python3
"""Script that does some very basic checks on the outputs of the tests of DDML."""

import sys

import ROOT
from podio.reading import get_reader


def check_any_mc_fastsim(filename):
    """Check if any of the mc particles in the file has been handled by fast simulation"""
    reader = get_reader(filename)
    events = reader.get("events")
    for event in events:
        mcps = event.get("MCParticles")
        for mcp in mcps:
            if mcp.isHandledByFastSim():
                return True

    print(f"ERROR: Could not find any fast simmed MCParticles in {filename}")
    return False


def check_calo_entry_recording(filename):
    """Check that the file recording the calorimeter entry particle information
    is written correctly (at a very basic level)"""
    rfile = ROOT.TFile.Open(filename)

    def _check_tree_present(treename):
        tree = rfile.Get(treename)
        if not tree:
            print(f"ERROR The expected '{treename}' TTree is not present in {filename}")
            return False
        return True

    return all(_check_tree_present(t) for t in ("run", "Fast_Sim_Info", "global"))


checks = (
    check_any_mc_fastsim("cc3_ild_pgun.edm4hep.root"),
    check_calo_entry_recording("Output.root"),  # Comes from run_cc3_ild
    check_any_mc_fastsim("bibae_ild_pgun.edm4hep.root"),
)

if not all(checks):
    sys.exit(1)
