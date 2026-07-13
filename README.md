# Stove Guard

An intelligent computer vision system for real-time monitoring of gas stoves.

## Quick video demo

Download the latest demo archive from the GitHub Releases page.

Extract it and run:

```bash

./stove_guard_video_demo \
    --video videos/stove_with_cookware_flame1.mp4 \
    --analyzer yolo \
    --model models/yolov8n.onnx
```

For building from source, see the [Getting started](#getting-started)

## Requirements

- Ubuntu 24.04 LTS
- C++17 compatible compiler
- CMake 3.16+
- OpenCV 4.x
- Git and Git LFS

## Features

- Real-time object detection using YOLOv8 + ONNX Runtime
- Detects gas stoves, cookware, flames and people
- Video-based demonstration application
- C++17 implementation

## Getting Started

### Install common deps

```bash
sudo apt update

sudo apt install -y build-essential git git-lfs cmake wget tar
```

### Clone repository

```bash
git clone --recurse-submodules https://github.com/mshandruk/stove-guard.git

cd stove-guard
```

If you cloned repository without submodules

```bash
git submodule update --init --recursive
```

### Download Git LFS assets

Required to download the bundled demo videos and YOLO model.

```bash
git lfs install
git lfs pull
```

### Install opencv

```bash
sudo apt install -y libopencv-dev
```

### Install ONNX Runtime

The script downloads the prebuild ONNX Runtime binaries into the `third_party` directory.

```bash
bash ./scripts/setup_onnx_deps.sh
```

### Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_VIDEO_DEMO=ON && cmake --build build
```

### Run

```bash
./build/apps/video_demo/stove_guard_video_demo \
  --video demo/videos/stove_with_cookware_flame1.mp4 \
  --analyzer yolo \
  --model demo/models/yolov8n.onnx
```

### Command-line options

| Option            | Description                                                                |
|-------------------|----------------------------------------------------------------------------|
| `--video <path>`  | Path to the input video file.                                              |
| `--analyzer fake` | Use the built-in fake detection scenario.                                  |
| `--analyzer yolo` | Use the YOLO object detector via ONNX Runtime.                             |
| `--model <path>`  | Path to the YOLO ONNX model (required when `--analyzer yolo` is selected). |

## Development

### Debug build with tests

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON

cmake --build build

ctest --output-on-failure --test-dir build/
```

## License

[MIT](LICENSE) © 2026 Maxim Shandruk