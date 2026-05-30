# Ascent Design System

The Ascent Design System is a tool for calculating Space Shuttle ascent trajectory parameters, mainly for use with the Space Shuttle Vessel (SSV) addon for the Orbiter Space Flight Simulator.

## Usage

For the most part all insertion and OMS burn parameters are inputs in the ADS. The "Calculate" button then simulates through ascent and the two insertion OMS burns. The "Calculate" button is used to show a variety of trajectory output parameters. In addition to this there are two automated methods to converge PEG-4A targets for OMS-1 and OMS-2.

With standard insertion, the "Converge HA" button varies the HT and THETA-T values for OMS-1 PEG-4 to achieve a specific apogee height while keeping the burn vector along the velocity vector. If the parameters have been successfully converged then the output window will contain the required OMS-1 targets. If they are supposed to be used in a subsequent simulation ("Calculate" button") then they have to be manually copied over from the output window to the input boxes on the left side.

With direct insertion the "Converge HA" feature varies the insertion velocity to achieve the desired apogee height. Again this velocity is output and would have to be copied over to the input box for the insertion velocity (VEL) to be used in subsequent calculations.

The "Converge HP" button works the same way with both standard and direct insertion and converges the OMS-2 burn. It uses the post OMS-1 or post MPS dump state vectors to first find the time of apogee. It then iterates on an impulsive, horizontal Delta V increment to achieve the desired height at 180 degrees of orbital travel after this maneuver. It should be noted that this height does not strictly have to be smaller than the HA value, so OMS-2, happening at apogee, can be large enough to set up another apogee instead of a perigee. After this impulsive OMS-2 estimation has been converged an optimum, actual Time of Ignition is calculated using the thrust value of two OMS engines and the mass of the Shuttle before OMS-2. In a last step the PEG-4 targets (HT and THETA-T) are varied to achieve the same velocity change as the impulsive burn.

## Modeled Forces

The ADS uses a gravity model of degree and order four. The 1962 US Standard Atmosphere is used throughout to calculate atmospheric density.

## Future

There was a real program called the Ascent Design System in the 1980s that was used to simulate and calculate the Space Shuttle ascent. It included a lot more features than this software at the moment does. The eventual goal would be to have the ability to generate all I-loads associated with ascent guidance, including support for AOAs and other abort maneuvers.