# Stove Guard

An intelligent computer vision system for real-time monitoring of stoves.
The project aims to prevent domestic accidents by detecting active burners and potential hazards,
such as unattended cooking or forgotten food.

**Note: This project is currently in the early development stage.**

# Dependencies

## Install ONNX Runtime

### On linux

```bash
bash ./scripts/setup_onnx_deps.sh
```

The script downloads and installs the required ONNX Runtime binaries into `third_party` directory.

# Build Instructions

## Configure(DEBUG with tests)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
```

### Build

```
cmake --build build
```

### Run tests

```bash
ctest --output-on-failure --test-dir build/
```

## Build demo application

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_DEMO=ON
```

### Build

```bash
cmake --build build
```

### Run

```bash
./build/apps/demo/stove_guard_demo
```

## Build video demo application

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_VIDEO_DEMO=ON
```

### Build

```bash
cmake --build build
```

### Run

```bash
./build/apps/video_demo/stove_guard_video_demo --video path/to/video.mp4 \
  --analyzer yolo \
  --model path/to/model.onnx
```

## License

[MIT](LICENSE) © 2026 Maxim Shandruk