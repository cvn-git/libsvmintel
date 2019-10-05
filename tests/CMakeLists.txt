# Targets
add_executable(test_svm_speed
    test_svm_speed.cpp
)
target_link_libraries(test_svm_speed
    svmcore
)
