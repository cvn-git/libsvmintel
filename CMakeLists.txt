project("libsvmIntel")
cmake_minimum_required(VERSION 3.2)
set (CMAKE_CXX_STANDARD 11)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-wd4996 -wd4005)
endif()


# Files
set(SVMCORE_SOURCES
    svm.h svm.cpp
)


# Targets
if(BUILD_SHARED_LIBRARY)
    if(MSVC)
        add_library(libsvm SHARED
            ${SVMCORE_SOURCES} svm.def
        )
    else()
        add_library(svm SHARED
            ${SVMCORE_SOURCES}
        )
    endif()
else()
    add_library(svmcore
        ${SVMCORE_SOURCES}
    )

    add_executable(svm-predict
        svm-predict.c
    )
    target_link_libraries(svm-predict
        svmcore
    )

    add_executable(svm-train
        svm-train.c
    )
    target_link_libraries(svm-train
        svmcore
    )

    add_executable(svm-scale
        svm-scale.c
    )
    target_link_libraries(svm-scale
        svmcore
    )
endif()
