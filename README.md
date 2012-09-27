Installation
============

In order to build toolset you will need cmake, boost-devel, gsl, root, git.

    mkdir build
    cd build
    cmake ..
    make


Utilities
=========

There are several utilities shipped with the library. For each and every invocation syntax is ./util_name data_file -g geometry_file

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

### Exported branches

#### Branch "tracks"

Contains recognized tracks. Branch name starts with chamber *prefix* which has form *g{group_id}{axis_id}*. For example: *g1X*, *g1Y*, *g2X*, *g2Y*.

Fields:

* *{prefix}\_track\_count* - size of the following arrays
* *{prefix}\_c0[]* - constant of track line, mm
* *{prefix}\_c1[]* - slope of track line, mm/cm
* *{prefix}\_hits\_count[]* - count of planes where hit was detected
* *{prefix}\_chisq[]* - chi squared of track line fit

#### Branch "drift"

Contains all drift chamber events.

Fields:

* *chamber_id* - chamber id
* *num_wires* - size of the following array
* *wire_pos[]* - wire position
* *time[]* - drift time

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
