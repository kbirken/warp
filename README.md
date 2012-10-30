warp
====

Warp is a discrete event simulation engine written in C++. 
It can do simulations of multi-threaded systems incl. schedulers
and CPUs very efficiently.

The input file format is textual, though quite dense. Thus, it
is not really feasible to create one manually. For the simulation
workbench SIMBENCH there is an output generator which produces
warp input files. Hence, warp is a backend for SIMBENCH.

Warp can also create output files in various formats, e.g.,
load over time (in csv format) and a state transition diagram
in dot format.

Warp can simulate event-based systems quite fast. A single
simulation can be completed in a few milliseconds (depending
on the duration of the simulated process in terms of number
of parallel activities and events).

Warp has been developed by Klaus Birken at Harman/Becker.
It is used for simulation of Automotive/Infotainment systems.

Warp is subject to the Mozilla Public License v. 2.0.
