# GRAND to ROOT (GtoT)

The program to convert GRAND hardware binary blobs in raw format to GRAND ROOT TTrees based format

## How to use

Command line options can be seen with:
```
gtot -h
```

Standard usage:
```
gtot <options> <hardware_file_names>.dat/.f0*
```

To convert GP13 v1 files, use option ```-g1```:
```
gtot -g1 <hardware_file_names>.dat
```

* By default, gtot will generate the ROOT file with the hardware_file_name extension replaced with ".root", in the place where the hardware file resides.
* If you want to change the output file name, use option ```-o root_file_name```, only if single file is analysed



## How to build

### Requirements
Root version >=6.26.00 is required. Probably it can be built with earlier versions, but the cmake file would have to be adjusted to accommodate different dictionary compilation scheme.

### Building

From the main GtoT directory:
```
mkdir cmake-build-release
cd cmake-build-release
cmake ..
make all
```

## How to rebuild

In cmake-build-release:
```
make clean
make all
```

## How to rebuild including rebuilding of the makefiles

This may be necessary sometimes, if the project configuration has changed:
```
cd cmake-build-release
cmake .. --fresh
make clean; make all
```
If you have cmake older than 3.24, `cmake --target clean; cmake ..` may be necessary instead of `cmake .. --fresh`.