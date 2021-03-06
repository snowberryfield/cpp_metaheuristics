cmake_minimum_required(VERSION 3.10)
project(test)

###############################################################################
# switch compiler options according to CMAKE_BUILD_TYPE
###############################################################################
message(${CMAKE_BUILD_TYPE})
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(
        -std=c++14
        -Wall
        -O3
        -Wextra
        -Wno-error=missing-field-initializers
        -Wno-deprecated-copy
        -pthread
        #-march=native
    )
else() # Debug
    add_compile_options(
        -std=c++17
        -Wall
        -Wextra
        -Wno-error=missing-field-initializers
        -pthreadls
    )
endif()

###############################################################################
## tests
###############################################################################
enable_testing()

link_directories(
    ${TOP_DIR}/external/lib
)

include(test_model.cmake)
include(test_solver.cmake)
include(test_utility.cmake)

###############################################################################
## END
###############################################################################