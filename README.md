# Stove Guard

An intelligent computer vision system for real-time monitoring of stoves.
The project aims to prevent domestic accidents by detecting active burners and potential hazards,
such as unattended cooking or forgotten food.

**Note: This project is currently in the early development stage.**

# Build Instructions

### Build with Tests

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
cmake --build build
```

#### Run tests

```bash
ctest --output-on-failure --test-dir build/
```

## License

[MIT](LICENSE) © 2026 Maxim Shandruk