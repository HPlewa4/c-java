#!/bin/bash
set -euo pipefail

MODELS_DIR=${MODELS_DIR:-/app/models}
PORT=${PORT:-8080}

DEFAULT_MODEL="${MODELS_DIR}/cnn_cpu.bin"
MODEL_FILE=${MODEL_FILE:-$DEFAULT_MODEL}

echo "[backend] Starting CNN backend server"
echo "[backend] Models directory: ${MODELS_DIR}"
echo "[backend] Port: ${PORT}"

mkdir -p "${MODELS_DIR}"

if [ ! -f "${MODEL_FILE}" ]; then
    echo ""
    echo "⚠️  WARNING: Model file not found: ${MODEL_FILE}"
    echo ""
    echo "The server requires a trained CNN model to function."
    echo "Please provide a model by:"
    echo "  1. Training a model and mounting it as a volume:"
    echo "     docker run -v ./models:/app/models ..."
    echo ""
    echo "  2. Or train inside container (slow, not recommended):"
    echo "     docker exec -it <container> /app/build/train_quick"
    echo ""
    echo "Available models should be placed at:"
    echo "  - ${MODELS_DIR}/cnn_cpu.bin (CPU model)"
    echo "  - ${MODELS_DIR}/cnn_gpu.pt (GPU model)"
    echo ""
    echo "Server will start but classification will fail without a model."
    echo ""
    
    exec /app/build/server
else
    echo "[backend] Using model: ${MODEL_FILE}"
    echo "[backend] Server starting on port ${PORT}"
    echo ""
    
    exec /app/build/server "${MODEL_FILE}" "${PORT}"
fi