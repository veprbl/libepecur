[![Build Status](https://travis-ci.org/veprbl/libepecur.png?branch=master)](https://travis-ci.org/veprbl/libepecur)

Intro
=====

This is the software for alternative analysis of the data taken at EPECUR
experiment conducted at 10 GeV proton synchrotron located at ITEP,
Moscow. The existing mainline analysis provides differential cross sections
for elastic pion-proton scattering. This data shows some peculiarity, which may
signify existence of the N(1685) resonance. The idea of this analysis is to try
to look at inelastic scattering events to see if an additional conformation for
this peculiarity can be obtained. Because the current setup is designed for
elastic event observation there are not many options to approach this. In this
analysis we will look at one track event output as a function of incident
momentum and some angle. Branching of N(1685) to inelastic events is predicted
to be high, so we expect resonance to manifest itself clear enough in the
dependency of output on incident momentum.

Building
========

*This software is useless without the data files. Why would you want to build it?*

In order to compile this software you will need cmake, boost-devel, root and git.

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make

Utilities
=========

There are several utilities shipped with the library. Most of them like
**axial\_shift**, **momentum\_dispersion**, **show\_events** and
**export\_tree** will use **libepecur** to read RAW data files.
**export\_tree** utility will produce a ROOT file, which then can be
analyzed using standard ROOT tools or using the **process** utility.

Having RAW data file is not enough, there also must be a "geometry" file (for
example, contrib/geom_apr10.c). **libepecur** will use it to turn device_id's
and wire_id's to physical chamber_id's and wire positions respectively.

axial_shift
-----------

Recognizes track projections, plots signed differences of the track and it's point
coordinates. Mean values of these distributions are the shifts corrections for the
position in the plane perpendicular to the optical axis. **axial\_shift** utility
outputs shift values in text format ready to be pasted into a "geometry" file.
The calibration procedure should be iterated few times to get satisfying results.

momentum_dispersion
-------------------

Plots trigger count distributions per wires for all axes of all chambers.

show_events
-----------

Visualizes triggered wires on a per-group basis, displays recognized track projections.
Used mostly for debugging.

export_tree
-----------

**export\_tree** does the first pass of the analysis itself. This utility will
read RAW file twice. First to make a drift\_time distribution so it can produce
the calibration curve for drift chambers. Second will actually ask **libepecur**
to recognize both proportional and drift chamber tracks using more accurate
drift\_time-aware drift chamber points.

It is possible to skip drift calibration curve generation using *--rough-drift*
command line option. But in this case resulting drift tracks will be less accurate.

### Exported TTree's

The utility will export following TTree's:

#### info

Branches:

* *key/C*
* *value/C*

This is a basic key/value storage for meta information like path to a raw file,
path to a geometry file and the programs source code revision id.

#### events

Contains reconstructed track projections for proportional chambers and wire\_id's
and drift time's for drift chambers.

##### Recognized track projections

Branch name starts with chamber *{prefix}* which has form *t{group_id}{axis_id}*.
For example: *t1X*, *t1Y*, *t2X*, *t2Y*.

Branches:

* *{prefix}\_track\_count* - size of the following arrays
* *{prefix}\_c0[]* - constant of track line, mm
* *{prefix}\_c1[]* - slope of track line, mm/mm
* *{prefix}\_hits\_count[]* - count of planes where hit was detected
* *{prefix}\_chisq[]* - chi squared of track line fit
* *{prefix}\_prev\_chisq[]* - second minimal value for chi squared of track line fit

##### Drift chamber specific

Branch name starts with a *{prefix}* which has form *d{group_id}{axis_id}{plane}*.
For example: *d3Y1*, *d5X1*.

Branches:

* *{prefix}\_num_wires* - size of the following array
* *{prefix}\_wire_pos[]* - wire position
* *{prefix}\_time[]* - drift time

### Troubleshooting

If you see export_tree showing errors like this

    Error in <TTree::Fill>: Failed filling branch:..., nbytes=-1
     This error is symptomatic of a Tree created as a memory-resident Tree
     Instead of doing:
        TTree *T = new TTree(...)
        TFile *f = new TFile(...)
     you should do:
        TFile *f = new TFile(...)
        TTree *T = new TTree(...)

this might be because of insufficient disk space.

process
-------

Does the second pass of the analysis. Takes the ROOT file produced by
*export_tree* and the geometry file and produces another ROOT file with
additional information about selected events.

New leaves include:

* *theta\_l*, *theta\_r* - scattering angle for the particles that went to the left and right arm correspondingly
* *phi\_l*, *phi\_r*
* *beam\_moemntum* - beam momentum calculated from position in the first focus
* *incident\_momentum\_l*, *incident\_momentum\_r* - mometum of the particle in its scattering point (correction for passage through matter is applied)

**Note:** If there are no particle in the arm then appropriate *\_l* or *\_r* leaves are filled with NAN's.

Following leaves contain track intersection points

* *LR\_x*, *LR\_y*, *LR\_z*
* *RL\_x*, *RL\_y*, *RL\_z*
* *F2R\_x*, *F2R\_y*, *F2R\_z*
* *F2L\_x*, *F2L\_y*, *F2L\_z*
* *RF2\_x*, *RF2\_y*, *RF2\_z*
* *LF2\_x*, *LF2\_y*, *LF2\_z*

*LR* means nearest point to the track from the *R*ight arm lying on the track from *L*eft arm.

Credits
=======

Kalinkin Dmitry
