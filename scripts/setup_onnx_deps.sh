#!/usr/bin/env bash
set -euo pipefail

VERSION="1.26.0"
EXPECTED_SHA256="1254da24fb389cf39dc0ff3451ab48301740ffbfcbaf646849df92f80ee92c57"
COMPONENT_DIR="onnxruntime-linux-x64"
ARCHIVE="${COMPONENT_DIR}-${VERSION}.tgz"
URL="https://github.com/microsoft/onnxruntime/releases/download/v${VERSION}/${ARCHIVE}"
TARGET_DIR="third_party"
FINAL_DIR="${TARGET_DIR}/${COMPONENT_DIR}"
EXPECTED_ARCHIVE="${TARGET_DIR}/${ARCHIVE}"

check_hash() {
  if [ -f "${EXPECTED_ARCHIVE}" ]; then
    echo "${EXPECTED_SHA256} ${EXPECTED_ARCHIVE}" | sha256sum --check --status
    return $?
  fi
  return 1
}

mkdir -p "$TARGET_DIR"

if ! check_hash ; then
    echo "Downloading ONNX Runtime ${VERSION}..."
    wget -O "${EXPECTED_ARCHIVE}" "$URL"
    if ! check_hash; then
              echo "ERROR: Downloaded file is corrupted (hash mismatch)!"
      exit 1
    fi
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
