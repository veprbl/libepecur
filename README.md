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

Recognizes tracks and writes them to ROOT tree.

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
