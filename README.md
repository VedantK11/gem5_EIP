# QPoints

QPoints is a tool to generate a gem5 compatible checkpoints 
using QEMU. This tool currently works for only ARM system. 
This tool was created to reduce time spent in reaching
a state state of long running applcations.

## Credits

This repository is forked and adapted from the following repositories:

- https://github.com/PrincetonUniversity/QPoints  
- https://github.com/bgodala/gem5\_ARM\_FDIP  

We would like to thank the authors of **EMISSARY (ISCA'23)** and **Bhargav Reddy Godala** for their valuable contributions and for making their code publicly available.

## Requirements:

### Ubuntu 22.04 (gem5 ≥ v21.1)

If compiling gem5 on **Ubuntu 22.04** or related Linux distributions, install the required dependencies using:
```
apt install build-essential git m4 scons zlib1g zlib1g-dev \
    libprotobuf-dev protobuf-compiler libprotoc-dev libgoogle-perftools-dev \
    python3-dev libboost-all-dev pkg-config python3-tk
```

## Ubuntu 20.04 (gem5 ≥ v21.0)

If compiling gem5 on **Ubuntu 20.04** or related Linux distributions, install the required dependencies using:
```
apt install build-essential git m4 scons zlib1g zlib1g-dev \
    libprotobuf-dev protobuf-compiler libprotoc-dev libgoogle-perftools-dev \
    python3-dev python-is-python3 libboost-all-dev pkg-config gcc-10 g++-10 \
    python3-tk
```

### Setup:

 Run setup script which sets up directoris and builds a docker image
   ```
   bash setup.sh
   ```
### Build and Run:
1. To test checkpoint gem5 needs to be built. Use the following command to build gem5.
   ```
   cd gem5
   scons -j8 build/ARM/gem5.opt
   ```
   If you encounter the following error while building gem5:
   ```
   No such file or directory: "/qpoints/gem5/fatal: detected dubious ownership in repository at '/qpoints/gem5'\nTo add an exception for this directory, call:\n\n\tgit config --global --add safe.directory /qpoints/gem5/hooks"
   ```
   You can fix this by following the information reported above:
   ```
   git config --global --add safe.directory /qpoints/gem5
   ```

2. Running Simulations

#### Run for a Single Benchmark

```
bash run_gem5.sh <path/to/checkpoint_dir> <output_dir_name> 
```

#### Run for all Benchmarks
```
python3 sim.py <path/to/dir_containing_all_checkpoint_dirs>
```

