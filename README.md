warp
====

Introduction
------------

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

Installation
------------

There is no binary release and also no special installation procedure.
Just install Eclipse Juno CDT, import the project org.nanosite.warp 
and build it with gcc. Call the resulting executable to get information
on command line options.


Switching OS during the Build process
--------------------------------------

A few manual steps are required to ensure the build process works when shifting to Windows (or from MAC to windows).  
  - The org.nanosite.warp project (C++) has a build target called WARP Jni Lib Windows. The environment of this build target must include the proper paths to a 64 bit Mingw and MSys. 
  ![alt text](https://github.com/kbirken/warp/blob/master/Readme-Images/windows-doc1.PNG)
  
  - If not already set, the build path of the java project (org.nanosite.warp.jni) should include the windows-64 path as the path to the native library. The C++ project builds the jnilib file and as a post-processing step copies the jnilib file into the windows-64 folder. 
  ![alt text](https://github.com/kbirken/warp/blob/master/Readme-Images/windows-doc2.PNG)
  
  
There are a few steps needed to ensure building the jnilib file in 
Open topics / next steps
------------------------

- Currently there is no documentation, even not for the input file format.
- Some refactoring is needed. The source files in 'model' should be split
  into a real model-description part (which is not changed during simulation
  runs) and a simulation-state part (which is changed during simulation runs).
- There are no regression tests yet (only as part of SIMBENCH).
- There should be a more readable input file format.
- The whole thing should probably being rewritten in Java :-)


Impress
-------
Warp has been developed by Klaus Birken at Harman/Becker.
It is used for simulation of Automotive/Infotainment systems.

Warp is subject to the Mozilla Public License v. 2.0.
