Installation
============

In order to build this software you will need cmake, boost-devel, root, git.

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make

Scientific Linux
----------------

Few workarounds for problems encoutered while compiling libepecur on Scientific Linux 6.4

    sudo sed -i 's/GET_FILENAME_COMPONENT(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)/\0\n\0/' /usr/lib64/boost/Boost-relwithdebinfo.cmake
    sudo ln -s /usr/lib64/libicuuc.so.42.1 /usr/lib64/libicuuc.so
    sudo ln -s /usr/lib64/libicui18n.so.42 /usr/lib64/libicui18n.so


Utilities
=========

There are several utilities shipped with the library.

axial_shift
-----------

Recognizes tracks, plots shift histogram for each proportional chamber, outputs configuration data ready to be pasted into geometry file (This one is intended to be a replacement for configuration that may be already present in your geometry file).

momentum_dispersion
-------------------

Plots event histograms for all axes/chambers. This tool much copies functionality of original python code.

show_events
-----------

Visualizes whole events, displays recognized tracks.

export_tree
-----------

Exports various data from EPECUR data file to a ROOT tree.

### Exported TTree's

#### info

Branches:

* *key/C*
* *value/C*

This is a basic key/value storage for meta information like path to raw file, geometry file and unique source code revision id.

#### events

Contains reconstructed tracks for proportional chambers and wire id's for drift chambers.

##### Proportional

Branch name starts with chamber *prefix* which has form *t{group_id}{axis_id}*. For example: *t1X*, *t1Y*, *t2X*, *t2Y*.

Branches:

* *{prefix}\_track\_count* - size of the following arrays
* *{prefix}\_c0[]* - constant of track line, mm
* *{prefix}\_c1[]* - slope of track line, mm/mm
* *{prefix}\_hits\_count[]* - count of planes where hit was detected
* *{prefix}\_chisq[]* - chi squared of track line fit
* *{prefix}\_prev\_chisq[]* - second minimal value for chi squared of track line fit

##### Drift

Branch name starts with *prefix* which has form *d{group_id}{axis_id}{plane}*. For example: *d3Y1*, *d5X1*.

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

Credits
=======

Kalinkin Dmitry
