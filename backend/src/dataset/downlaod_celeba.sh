#!/usr/bin/env bash
set -euo pipefail

DATA_ROOT="data/celeba"
SPLIT_ROOT="data/celeba_splits"

mkdir -p "$DATA_ROOT" "$SPLIT_ROOT"

if ! command -v kaggle >/dev/null 2>&1; then
  echo "kaggle CLI not found. Install it and configure your API token first."
  exit 1
fi

echo "Downloading CelebA (this may take a while)..."
kaggle datasets download -d dayiyao/celeba-dataset -p "$DATA_ROOT" --force

ZIP_FILE=$(find "$DATA_ROOT" -maxdepth 1 -name "*.zip" | head -n 1 || true)
if [ -z "${ZIP_FILE:-}" ]; then
  echo "No zip archive downloaded."
  exit 1
fi

echo "Unzipping $ZIP_FILE ..."
unzip -q "$ZIP_FILE" -d "$DATA_ROOT"
rm "$ZIP_FILE"

if ! ls "$DATA_ROOT"/img_align_celeba >/dev/null 2>&1; then
  echo "img_align_celeba directory not found after unzip."
  exit 1
fi

if [ ! -f "$DATA_ROOT/list_attr_celeba.txt" ]; then
  echo "list_attr_celeba.txt not found in $DATA_ROOT."
  exit 1
fi

echo "CelebA ready in $DATA_ROOT"
echo "Splits will be written to $SPLIT_ROOT by the C++ program."
