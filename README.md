# Introduction

*lastro* is a toolkit for *landscape astrophotography*.
I started this project to satisfy my personal needs -- 
free and open-source astrophotogray tools available under linux.
I have no intend to make it a complete postprocessing system.
There are already software for RAW photo preprocessing & conversion
and general image editing (e.g. RawTherapee, GIMP).
I only want to fill whatever is missing.

Current features
* Star mask extraction
* Star detection from masks

Features I am working on 
* Image alignment based on stars
* Software based soft-focus filter

Features I am planning on
* Auto-stacking

# Requirement

* Linux
* C++ Compiler with C++14 support
* CMake
* OpenCV
* [Wavelib](https://github.com/zhehangd/wavelib) (forked, with improved CMake build system)
* CLI11
* glog
