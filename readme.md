# GRAND to ROOT (GtoT)

The program to convert GRAND hardware binary blobs in raw format to GRAND ROOT TTrees based format

## How to use

Command line options can be seen with:
```
gtot -h
```

Standard usage:
```
gtot <options> hardware_file_name
```

To convert GP13 v1 files, use option ```-g1```:
```
gtot -g1 hardware_file_name
```

* By default, gtot will generate the ROOT file with the hardware_file_name extension replaced with ".root", in the place where the hardware file resides.
* If you want to change the output file name, use option ```-o root_file_name```



## How to build

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
rm -r *
cmake ..
make all
```