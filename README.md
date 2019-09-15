# libsvmintel
[LIBSVM](https://github.com/cjlin1/libsvm) fork for Intel SIMD.

See also the original [README](README) of LIBSVM

## Requirements
### Linux environment
For Ubuntu 18.04:
```bash
sudo apt-get install build-essential cmake git
```

### Windows environment

* Visual C++ compiler, version 2015 or later
* CMake from https://cmake.org/download
* Git from https://git-scm.com/downloads


## Installing libsvmintel for Python

For Python, libsvmintel can be installed directly from GitHub by running the following command from your virtual environment:
```bash
pip install -e git+https://github.com/cvn-git/libsvmintel.git@master#egg=libsvmintel
```

Uninstall can be done as usual:
```bash
pip uninstall libsvmintel
```

## Development builds

CMake is used to build libsvmintel binaries. By default (per LIBSVM convention), Linux binaries are placed in the root path, while Windows binaries are placed in ``<ROOT>/windows`` subpath.

### Linux build
#### Build from command line
```bash
cmake -H. -Bbuild_linux -DCMAKE_BUILD_TYPE=Release
cd build_linux
make
```

#### Eclipse build
Create Eclipse project:
```bash
cmake -H. -B../tests_eclipse -DCMAKE_BUILD_TYPE=Debug -G "Eclipse CDT4 - Unix Makefiles"
```

Import the Eclipse project generated in `../tests_eclipse` and build it in Eclipse IDE.

---
**NOTE**
To parse source files properly in Eclipse (e.g. for debugging purpose), it is recommended to generate Eclipse project files in a sibling path (to the source directory).

---

### Windows build
#### Build from command line
Open the terminal "VS2015 x64 Native Tools Command Prompt" (for 64-bit build) and run:
```dos
cmake -H. -Bbuild_nmake -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles"
cd build_nmake
nmake
```

#### Visual Studio build
Open Windows terminal "Command Prompt" and run the following command to generate a Visual Studio 2015 solution:
```dos
cmake -H. -Bbuild_msvc -G "Visual Studio 14 2015 Win64"
```

### Build MEX targets
To build MEX targets, one can add the option ``-DBUILD_MEX=1`` to any of the above CMake commands. By default (per LIBSVM convention), MEX files are placed in ``<ROOT>/windows`` subpath.

---
**NOTE**
On Windows, CMake 3.15.3 does not build MEX files for Matlab 2019a properly. A workaround is patching the script ``C:\Program Files\CMake\share\cmake-3.15\Modules\FindMatlab.cmake`` as instructed in https://github.com/robotology/blockfactory/issues/44

---
