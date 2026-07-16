#!/usr/bin/env bash
set -euo pipefail

SUCCESS=0
FAILURE=1

VERSION="1.26.0"
EXPECTED_SHA256="1254da24fb389cf39dc0ff3451ab48301740ffbfcbaf646849df92f80ee92c57"

TARGET_DIR="third_party"
COMPONENT_DIR="onnxruntime-linux-x64"

FINAL_DIR="${TARGET_DIR}/${COMPONENT_DIR}"
ARCHIVE="${COMPONENT_DIR}-${VERSION}.tgz"
EXPECTED_ARCHIVE="${TARGET_DIR}/${ARCHIVE}"
URL="https://github.com/microsoft/onnxruntime/releases/download/v${VERSION}/${ARCHIVE}"

check_hash() {
  if [[ ! -f "$EXPECTED_ARCHIVE" ]]; then
    return $FAILURE
  fi

  local actual_sha256
  actual_sha256=$(sha256sum "$EXPECTED_ARCHIVE" 2>/dev/null  | awk '{print $1}' || true)
  if [[ "$actual_sha256" == "$EXPECTED_SHA256" ]]; then
    return $SUCCESS
  fi

  return $FAILURE
}

mkdir -p "$TARGET_DIR"

if ! check_hash ; then
    echo "Downloading ONNX Runtime ${VERSION}..."
    wget -O "$EXPECTED_ARCHIVE" "$URL" || { rm -f "$EXPECTED_ARCHIVE"; exit 1; }
    if ! check_hash; then
      echo "ERROR: Downloaded file is corrupted (hash mismatch)!"
      rm -f "$EXPECTED_ARCHIVE"
      exit 1
    fi
else
    echo "Archive already exists: $EXPECTED_ARCHIVE downloading skipped..."
fi

rm -rf "${FINAL_DIR:?}"
mkdir -p "$FINAL_DIR"

echo "Extracting archive..."
tar -xzf "$EXPECTED_ARCHIVE" -C "$FINAL_DIR" --strip-components=1

echo "Done."
