list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

# Get IPP
if(WIN32)
    set(IPP_STATIC ON CACHE STRING "IPP link mode")
else()
    set(IPP_STATIC OFF CACHE STRING "IPP link mode")
endif()
find_package(IPP)


# Targets
add_library(svmintel
    svm_intel.h svm_intel.cpp
    svm_intel_math.h
)
target_include_directories(svmintel
    PUBLIC .
    PUBLIC ..
    PUBLIC ${IPP_INCLUDE_DIRS}
)
target_link_libraries(svmintel
    PUBLIC ${IPP_LIBRARY}
)
