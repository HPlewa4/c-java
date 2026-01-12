#!/bin/bash
# Build C++ backend

if ! command -v cmake &> /dev/null; then
    echo "❌ ERROR: CMake is not installed!"
    echo ""
    echo "To install CMake:"
    echo "  macOS:   brew install cmake"
    echo "  Ubuntu:  sudo apt-get install cmake"
    echo "  Windows: Download from https://cmake.org/download/"
    echo ""
    exit 1
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "❌ ERROR: No C++ compiler found!"
    echo ""
    echo "To install:"
    echo "  macOS:   xcode-select --install"
    echo "  Ubuntu:  sudo apt-get install build-essential"
    echo ""
    exit 1
fi

if ! pkg-config --exists opencv4 2>/dev/null && ! pkg-config --exists opencv 2>/dev/null; then
    echo "⚠️  WARNING: OpenCV not found (needed for train & server)"
    echo "   To install: brew install opencv  (or see INSTALL.md)"
    echo ""
fi

if ! pkg-config --exists libmicrohttpd 2>/dev/null; then
    echo "⚠️  WARNING: libmicrohttpd not found (needed for server)"
    echo "   To install: brew install libmicrohttpd"
    echo ""
fi

echo "Building C++ backend..."
echo ""

mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ CMake configuration failed!"
    echo "   Check that all dependencies are installed (see INSTALL.md)"
    exit 1
fi

make -j4

if [ $? -eq 0 ]; then
    echo ""
    echo "======================================"
    echo "  ✅ Build Successful!"
    echo "======================================"
    echo ""
    echo "Training executables:"
    echo "  - ./build/train_quick         (Quick CPU training)"
    echo "  - ./build/train_full          (Full GPU training with LibTorch)"
    echo "  - ./build/test_model          (Unified model testing)"
    echo ""
    echo "Server:"
    echo "  - ./build/server              (HTTP REST API server)"
    echo ""
    echo "Unit tests:"
    echo "  - ./build/test_tensor         (Generic template tests)"
    echo "  - ./build/test_activations    (Activation function tests)"
    echo "  - ./build/test_optimizer      (Optimizer tests)"
    echo "  - ./build/test_parallel       (Parallel processing tests)"
    echo ""
    echo "Quick start:"
    echo "  Train quick: cd build && ./train_quick"
    echo "  Train full:  cd build && ./train_full"
    echo "  Test model:  cd build && ./test_model ../models/cnn_cpu.bin"
    echo "  Run server:  cd build && ./server"
    echo "  Run tests:   cd build && ./test_tensor"
    echo ""
else
    echo ""
    echo "❌ Build failed!"
    echo "   Check compiler errors above"
    exit 1
fi