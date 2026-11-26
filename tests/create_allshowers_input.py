#!/usr/bin/env python3
"""Create an EDM4hep input file for ddsim that contains one particle per event
for all species that are supported by AllShowers. Each species is shot once into
the ECAL barrel and once into the ECAL endcap of ILD, such that the
AS_BARREL_PY_INTERFACE and AS_ENDCAP_PY_INTERFACE presets should trigger."""

import argparse
import math

import edm4hep
import podio
from podio.root_io import Writer

# PDG code: (mass [GeV], charge) of the species supported by AllShowers (see
# ALLSHOWERS_PARTICLES in python/ddml/configs.py)
ALLSHOWERS_SPECIES = {
    22: (0.0, 0.0),  # gamma
    11: (0.000511, -1.0),  # e-
    -11: (0.000511, 1.0),  # e+
    211: (0.13957, 1.0),  # pi+
    -211: (0.13957, -1.0),  # pi-
    321: (0.493677, 1.0),  # kaon+
    -321: (0.493677, -1.0),  # kaon-
    130: (0.497611, 0.0),  # kaon0L
    2212: (0.938272, 1.0),  # proton
    -2212: (0.938272, -1.0),  # anti_proton
    2112: (0.939565, 0.0),  # neutron
    -2112: (0.939565, 0.0),  # anti_neutron
}

# Polar angles (deg) at which the particles are shot into the ILD ECAL barrel
# (theta > ~38 deg) and endcap (theta < ~35 deg)
THETA_VALUES = [80.0, 20.0]
# Azimuthal angle (deg) at which all particles are shot. This hits the center of
# an ECAL barrel stave (the stave boundaries are at 22.5 deg + k * 45 deg), such
# that the particles do not end up in a crack. Charged particles are bent by
# less than 2 deg before reaching the ECAL at the default energy.
PHI = 90.0


def create_event(event_number, pdg, mass, charge, energy, theta, phi):
    """Create a Frame with a single particle originating from the IP"""
    p = math.sqrt(energy**2 - mass**2)

    headers = edm4hep.EventHeaderCollection()
    header = headers.create()
    header.setEventNumber(event_number)
    header.setRunNumber(0)

    mcparticles = edm4hep.MCParticleCollection()
    mcp = mcparticles.create()
    mcp.setPDG(pdg)
    mcp.setGeneratorStatus(1)
    mcp.setMass(mass)
    mcp.setCharge(charge)
    mcp.setVertex(edm4hep.Vector3d(0.0, 0.0, 0.0))
    mcp.setMomentum(
        edm4hep.Vector3d(
            p * math.sin(theta) * math.cos(phi),
            p * math.sin(theta) * math.sin(phi),
            p * math.cos(theta),
        )
    )

    frame = podio.Frame()
    frame.put(headers, "EventHeader")
    frame.put(mcparticles, "MCParticles")
    return frame


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("outputfile", help="Output file (should end on .edm4hep.root)")
    parser.add_argument(
        "--energy",
        type=float,
        default=30.0,
        help="Energy of the particles in GeV (should be well above the 10 GeV "
        "AllShowers trigger threshold)",
    )
    args = parser.parse_args()

    writer = Writer(args.outputfile)
    n_events = 0
    for theta_deg in THETA_VALUES:
        for pdg, (mass, charge) in ALLSHOWERS_SPECIES.items():
            frame = create_event(
                n_events,
                pdg,
                mass,
                charge,
                args.energy,
                math.radians(theta_deg),
                math.radians(PHI),
            )
            writer.write_frame(frame, "events")
            n_events += 1
    writer.finish()

    print(f"Wrote {n_events} events to {args.outputfile}")


if __name__ == "__main__":
    main()
