#!/usr/bin/env bash
set -euo pipefail

VERSION="1.26.0"
COMPONENT_DIR="onnxruntime-linux-x64"
ARCHIVE="${COMPONENT_DIR}-${VERSION}.tgz"
URL="https://github.com/microsoft/onnxruntime/releases/download/v${VERSION}/${ARCHIVE}"
TARGET_DIR="third_party"
FINAL_DIR="${TARGET_DIR}/${COMPONENT_DIR}"

mkdir -p "$TARGET_DIR"

if [ ! -f "${TARGET_DIR}/${ARCHIVE}" ]; then
    echo "Downloading ONNX Runtime ${VERSION}..."
    wget -O "${TARGET_DIR}/${ARCHIVE}" "$URL"
else
    echo "Archive already exists: ${TARGET_DIR}/${ARCHIVE} downloading skipped..."
fi

echo "Extracting archive..."
tar -xzf "${TARGET_DIR}/${ARCHIVE}" -C "$TARGET_DIR"

extracted_dir="${TARGET_DIR}/${COMPONENT_DIR}-${VERSION}"

if [ ! -d "$extracted_dir" ]; then
  echo "ERROR: unexpected archive structure"
  exit 1

fi

rm -rf "$FINAL_DIR"
mv "$extracted_dir" "$FINAL_DIR"

echo "Done."
