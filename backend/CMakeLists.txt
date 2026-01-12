cmake_minimum_required(VERSION 3.10)
project(ImageClassifier)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MICROHTTPD REQUIRED libmicrohttpd)
find_package(OpenMP)

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/lib/libtorch")
find_package(Torch QUIET)
if(Torch_FOUND)
    message(STATUS "LibTorch found - GPU acceleration enabled")
    set(LIBTORCH_ENABLED ON)
else()
    message(STATUS "LibTorch not found - using CPU-only implementation")
    set(LIBTORCH_ENABLED OFF)
endif()

if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    message(STATUS "OpenMP enabled for parallel processing")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/model
    ${CMAKE_SOURCE_DIR}/include/model/activation
    ${CMAKE_SOURCE_DIR}/include/optimizer
    ${CMAKE_SOURCE_DIR}/include/utils
    ${OpenCV_INCLUDE_DIRS}
    ${MICROHTTPD_INCLUDE_DIRS}
)

link_directories(
    ${MICROHTTPD_LIBRARY_DIRS}
)

set(MODEL_SOURCES
    src/model/activation/activation_function.cpp
    src/model/conv_layer.cpp
    src/model/pool_layer.cpp
    src/model/cnn.cpp
    src/model/dropout_layer.cpp
    src/model/avgpool_layer.cpp
    src/optimizer/optimizer.cpp
    src/optimizer/adam_optimizer.cpp
    src/utils/augmentation.cpp
)

add_executable(train_quick
    src/training/train_quick.cpp
    ${MODEL_SOURCES}
)

if(NOT APPLE)
    target_link_libraries(train_quick ${OpenCV_LIBS} stdc++fs pthread)
else()
    target_link_libraries(train_quick ${OpenCV_LIBS} pthread "-framework Accelerate")
endif()

if(OpenMP_CXX_FOUND)
    target_link_libraries(train_quick OpenMP::OpenMP_CXX)
endif()

if(Torch_FOUND)
    add_executable(train_full
        src/training/train_full.cpp
    )

    target_link_libraries(train_full ${TORCH_LIBRARIES} ${OpenCV_LIBS} pthread)

    set_target_properties(train_full PROPERTIES
        BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/libtorch/lib"
        INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/libtorch/lib"
    )

    add_executable(test_model
        src/training/test_model.cpp
        ${MODEL_SOURCES}
    )

    target_link_libraries(test_model ${TORCH_LIBRARIES} ${OpenCV_LIBS} pthread)

    if(APPLE)
        target_link_libraries(test_model "-framework Accelerate")
    endif()

    set_target_properties(test_model PROPERTIES
        BUILD_RPATH "${CMAKE_SOURCE_DIR}/lib/libtorch/lib"
        INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib/libtorch/lib"
    )

    message(STATUS "Training executables: train_quick, train_full, test_model")
else()
    message(STATUS "Training executables: train_quick")
endif()

# HTTP Server (serves CNN model via REST API)
add_executable(server
    src/server/server.cpp
    ${MODEL_SOURCES}
)

if(NOT APPLE)
    target_link_libraries(server ${OpenCV_LIBS} ${MICROHTTPD_LIBRARIES} stdc++fs pthread)
else()
    target_link_libraries(server ${OpenCV_LIBS} ${MICROHTTPD_LIBRARIES} pthread "-framework Accelerate")
endif()

add_executable(test_tensor
    src/tests/test_tensor.cpp
)

if(NOT APPLE)
    target_link_libraries(test_tensor pthread)
else()
    target_link_libraries(test_tensor pthread)
endif()

add_executable(test_activations
    src/tests/test_activations.cpp
    ${MODEL_SOURCES}
)

if(NOT APPLE)
    target_link_libraries(test_activations ${OpenCV_LIBS} pthread)
else()
    target_link_libraries(test_activations ${OpenCV_LIBS} pthread "-framework Accelerate")
endif()

add_executable(test_optimizer
    src/tests/test_optimizer.cpp
    ${MODEL_SOURCES}
)

if(NOT APPLE)
    target_link_libraries(test_optimizer ${OpenCV_LIBS} pthread)
else()
    target_link_libraries(test_optimizer ${OpenCV_LIBS} pthread "-framework Accelerate")
endif()

add_executable(test_parallel
    src/tests/test_parallel.cpp
)

if(NOT APPLE)
    target_link_libraries(test_parallel pthread)
else()
    target_link_libraries(test_parallel pthread)
endif()

message(STATUS "Unit test executables enabled: test_tensor, test_activations, test_optimizer, test_parallel")

# Print build info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")