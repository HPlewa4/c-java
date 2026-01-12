#!/bin/bash
set -e

echo "========================================="
echo "  Backend Server Startup Script"
echo "========================================="
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

if [ ! -d "build" ]; then
    echo "Build directory not found. Running build script..."
    ./build.sh
    echo ""
fi

if [ ! -f "build/server" ]; then
    echo "Server executable not found. Running build script..."
    ./build.sh
    echo ""
fi

MODEL_CPU="models/cnn_cpu.bin"
MODEL_GPU="models/cnn_gpu.pt"

if [ -f "$MODEL_CPU" ]; then
    MODEL_PATH="$MODEL_CPU"
    echo "Found CPU model: $MODEL_CPU"
elif [ -f "$MODEL_GPU" ]; then
    MODEL_PATH="$MODEL_GPU"
    echo "Found GPU model: $MODEL_GPU"
else
    echo "⚠️  WARNING: No trained model found!"
    echo "   Expected locations:"
    echo "   - $MODEL_CPU (CPU model)"
    echo "   - $MODEL_GPU (GPU model)"
    echo ""
    echo "   Please train a model first:"
    echo "   - For CPU: cd build && ./train_full"
    echo "   - For GPU: cd build && ./train_quick"
    echo ""
    read -p "Continue without model? (server will not work properly) [y/N]: " -n 1 -r
    echo ""
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
    MODEL_PATH=""
fi

echo ""
echo "Starting backend server..."
echo "========================================="
echo ""

cd build
if [ -n "$MODEL_PATH" ]; then
    exec ./server "../$MODEL_PATH"
else
    exec ./server
fi