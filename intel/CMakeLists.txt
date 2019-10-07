# Targets
add_library(svmintel
    svm_intel.h svm_intel.cpp
    svm_intel_math.h
)
target_include_directories(svmintel
    PUBLIC .
    PUBLIC ..
)
