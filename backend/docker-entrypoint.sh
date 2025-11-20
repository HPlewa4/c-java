#!/bin/bash
set -euo pipefail

MODELS_DIR=${MODELS_DIR:-/app/models}
MODEL_FILE=${MODEL_FILE:-${MODELS_DIR}/trained_model.bin}
CLASSES_FILE=${CLASSES_FILE:-${MODELS_DIR}/classes.txt}
INPUT_DIM=${INPUT_DIM:-1024}
HIDDEN_DIM=${HIDDEN_DIM:-128}
DEFAULT_CLASS_COUNT=${DEFAULT_CLASS_COUNT:-5}
PORT=${PORT:-8080}

mkdir -p "${MODELS_DIR}"

if [ ! -s "${CLASSES_FILE}" ] || [ ! -s "${MODEL_FILE}" ]; then
    echo "[backend] Creating stub artifacts in ${MODELS_DIR}"
    python3 /app/scripts/generate_stub_model.py \
        --model "${MODEL_FILE}" \
        --classes "${CLASSES_FILE}" \
        --input-dim "${INPUT_DIM}" \
        --hidden-dim "${HIDDEN_DIM}" \
        --num-classes "${DEFAULT_CLASS_COUNT}"
else
    echo "[backend] Using existing model artifacts from ${MODELS_DIR}"
fi

exec /app/build/server "${MODEL_FILE}" "${CLASSES_FILE}" "${PORT}"
