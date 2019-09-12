project("libsvmIntel")
cmake_minimum_required(VERSION 3.2)
set (CMAKE_CXX_STANDARD 11)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-wd4996)
endif()


# Targets
add_library(svmcore
    svm.h svm.cpp
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
