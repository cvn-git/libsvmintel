find_package(Matlab)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-wd4477)
endif()

# Targets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_library(svm_model_matlab
    svm_model_matlab.h svm_model_matlab.c
)
target_include_directories(svm_model_matlab
    PUBLIC ${Matlab_INCLUDE_DIRS}
)
target_link_libraries(svm_model_matlab
    svmcore
)

matlab_add_mex(
    NAME svmtrain
    SRC svmtrain.c
    LINK_TO svm_model_matlab
)

matlab_add_mex(
    NAME svmpredict
    SRC svmpredict.c
    LINK_TO svm_model_matlab
)

matlab_add_mex(
    NAME libsvmread
    SRC libsvmread.c
)

matlab_add_mex(
    NAME libsvmwrite
    SRC libsvmwrite.c
)
