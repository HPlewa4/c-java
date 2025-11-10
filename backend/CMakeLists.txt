cmake_minimum_required(VERSION 3.10)
project(ImageClassifier)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MICROHTTPD REQUIRED libmicrohttpd)

# Include directories - organized structure
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/model
    ${CMAKE_SOURCE_DIR}/include/model/activation
    ${CMAKE_SOURCE_DIR}/include/utils
    ${OpenCV_INCLUDE_DIRS}
    ${MICROHTTPD_INCLUDE_DIRS}
)

# Link directories for libraries
link_directories(
    ${MICROHTTPD_LIBRARY_DIRS}
)

# Model source files (shared between executables)
set(MODEL_SOURCES
    src/model/neural_network.cpp
    src/model/activation/activation_function.cpp
)

# Training executable
add_executable(train
    src/training/train.cpp
    ${MODEL_SOURCES}
)

# Link filesystem library conditionally (not needed on macOS with modern C++17)
if(NOT APPLE)
    target_link_libraries(train ${OpenCV_LIBS} stdc++fs pthread)
else()
    target_link_libraries(train ${OpenCV_LIBS} pthread)
endif()

# Server executable
add_executable(server
    src/server/server.cpp
    ${MODEL_SOURCES}
)
target_link_libraries(server ${OpenCV_LIBS} ${MICROHTTPD_LIBRARIES} pthread)

# Unit tests executable
add_executable(test_neural_network
    src/tests/test_neural_network.cpp
    ${MODEL_SOURCES}
)
target_link_libraries(test_neural_network pthread)

# Print build info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
