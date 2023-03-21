# GRAND to ROOT (GtoT)

The program to convert GRAND hardware binary blobs in raw format to GRAND ROOT TTrees based format

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