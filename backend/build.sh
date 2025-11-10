#!/bin/bash
# Build C++ backend

echo "======================================"
echo "  C++ Backend Build Script"
echo "======================================"
echo ""

# Check for CMake
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

# Check for C++ compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "❌ ERROR: No C++ compiler found!"
    echo ""
    echo "To install:"
    echo "  macOS:   xcode-select --install"
    echo "  Ubuntu:  sudo apt-get install build-essential"
    echo ""
    exit 1
fi

# Check for OpenCV (optional but recommended)
if ! pkg-config --exists opencv4 2>/dev/null && ! pkg-config --exists opencv 2>/dev/null; then
    echo "⚠️  WARNING: OpenCV not found (needed for train & server)"
    echo "   To install: brew install opencv  (or see INSTALL.md)"
    echo ""
fi

# Check for libmicrohttpd (optional)
if ! pkg-config --exists libmicrohttpd 2>/dev/null; then
    echo "⚠️  WARNING: libmicrohttpd not found (needed for server)"
    echo "   To install: brew install libmicrohttpd"
    echo ""
fi

echo "Building C++ backend..."
echo ""

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo ""
    echo "❌ CMake configuration failed!"
    echo "   Check that all dependencies are installed (see INSTALL.md)"
    exit 1
fi

# Build
make -j4

if [ $? -eq 0 ]; then
    echo ""
    echo "======================================"
    echo "  ✅ Build Successful!"
    echo "======================================"
    echo ""
    echo "Executables created:"
    echo "  - ./build/train               (Training application)"
    echo "  - ./build/server              (HTTP Server)"
    echo "  - ./build/test_neural_network (Unit tests)"
    echo ""
    echo "To run tests:     cd build && ./test_neural_network"
    echo "To start server:  cd build && ./server"
    echo ""
else
    echo ""
    echo "❌ Build failed!"
    echo "   Check compiler errors above"
    exit 1
fi
