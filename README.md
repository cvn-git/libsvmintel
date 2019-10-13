# libsvmintel
[LIBSVM](https://github.com/cjlin1/libsvm) fork optimized for Intel CPUs. See also the original [README](README) of LIBSVM

## About
In `libsvmintel`, SVM kernel matrix computation is optimized to make use of large memory bandwidth and floating-point SIMD support in modern Intel CPUs:
* Program interfaces (command-line, Matlab, Python) from LIBSVM are kept unchanged
* All basic kernel types (linear/polynomial/GRBF/sigmoid) are supported
* Both sparse and dense feature modes are supported. However, optimization was designed mainly for dense features
* Shrinking is not supported. It needs to be disabled by the argument `-h 0` 

The following table provides speed-up results from the test harness [test_svm_speed.cpp](tests/test_svm_speed.cpp). Selecting between LIBSVM and `libsvmintel` is done using the compiler switch `USE_SVM_INTEL` in [svm_intel.h](intel/svm_intel.h). Floating-point precision for the kernel function is defined by `Dfloat` type in [svm_intel.h](intel/svm_intel.h)   

| SVM type  | LIBSVM         | libsvmintel    | libsvmintel    |
|:---------:|:--------------:|:--------------:|:--------------:|
|           |double-precision|double-precision|single-precision|
|C-SVC      |   23.31 sec    |    5.47 sec    |    2.69 sec    |
|ʋ-SVC      |   756.6 sec    |   165.9 sec    |   71.96 sec    |
|ɛ-SVR      |   23.59 sec    |   10.52 sec    |    7.80 sec    |
|ʋ-SVR      |   28.82 sec    |   11.07 sec    |    7.22 sec    |
|1-class SVM|   17.44 sec    |    4.85 sec    |    2.09 sec    |

## Requirements
### Linux environment
For Ubuntu 18.04:
```bash
sudo apt-get install build-essential cmake git
```

Install Intel's IPP library as instructed at https://software.intel.com/en-us/articles/installing-intel-free-libs-and-python-apt-repo 

### Windows environment

* Visual C++ compiler, version 2015 or later
* CMake from https://cmake.org/download
* Git from https://git-scm.com/downloads
* Intel's IPP libary from https://software.seek.intel.com/performance-libraries


## Installing libsvmintel for Python

For Python, ``libsvmintel`` can be installed directly from GitHub by running the following command from your virtual environment:
```bash
pip install -e git+https://github.com/cvn-git/libsvmintel.git@master#egg=libsvmintel
```

Alternately, installing from source code can be done by:
```bash
pip install -e <PATH_TO_LIBSVMINTEL_SOURCE>
```

Uninstall can be done as usual:
```bash
pip uninstall libsvmintel
```

## Development builds

CMake is used to build ``libsvmintel`` binaries. By default (per LIBSVM convention), Linux binaries are placed in the root path, while Windows binaries are placed in ``<ROOT>/windows`` subpath.

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
