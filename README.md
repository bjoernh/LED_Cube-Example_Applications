# LEDCube Example Applications

This repository contains example applications for the LEDCube matrixserver framework, demonstrating various 3D volumetric graphics, 2D matrix effects, and interactive games.

## About This Fork

This is a fork of the original [squarewavedot/exampleApplications](https://github.com/squarewavedot/exampleApplications) repository with the following enhancements:

- **Claude-generated documentation**: Comprehensive documentation generated with Claude AI assistance
- **macOS compatibility**: CMake build system tweaks to compile on macOS
- **Enhanced project structure**: Improved organization and build configuration

## Documentation

📖 **[Complete Documentation](https://placeholder-link-to-github-pages-docs)**

The full project documentation is hosted on GitHub Pages and includes API references, tutorials, and examples.

## Overview

This collection showcases the capabilities of the LEDCube matrixserver framework through various application types:

### 3D Volumetric Applications
- **CubeTestApp** - Basic 3D functionality demonstration and testing
- **Snake** - Multi-player 3D Snake game with joystick input and AI opponents
- **Genetic** - Genetic algorithm color evolution visualization in 3D space
- **Blackout3D** - Minimal interactive 3D application template
- **Breakout3D** - Full-featured 3D Breakout game with physics and multiplayer support

### 2D Matrix Applications
- **Rainbow** - Particle system with IMU integration (Raspberry Pi only)
- **Picture** - Image display with animation support and file watching
- **Genetic** - Genetic algorithm visualization on 2D matrix displays

### Animation & Effects
- **PixelFlow/PixelFlow2/PixelFlow3** - Fluid dynamics particle simulations

## Architecture

All applications inherit from either:
- `CubeApplication` for 3D volumetric rendering
- `MatrixApplication` for 2D matrix effects

The framework provides:
- **3D Volumetric Rendering**: Set voxels, draw 3D lines, render text on cube faces
- **Multi-Screen Support**: Six screens (front, right, back, left, top, bottom)
- **Input Integration**: Joystick, IMU (MPU6050), and ADC (ADS1000) sensor support
- **Multiple Connection Types**: TCP, IPC, and Unix socket communication with matrixserver

## Quick Start

### Prerequisites

#### Matrixserver Framework Installation

The matrixserver framework must be compiled and installed before building these example applications. The linker requires access to the `libmatrixapplication` library and related components.

**Installation Steps:**

1. **Clone the matrixserver framework:**
   ```bash
   git clone https://github.com/squarewavedot/matrixserver.git
   cd matrixserver
   ```

2. **Build the framework:**
   ```bash
   mkdir -p build && cd build
   cmake ..
   make
   ```

3. **Install system-wide (recommended):**
   ```bash
   sudo make install
   # This installs libraries to /usr/local/lib and headers to /usr/local/include
   ```

   **Alternative - Local installation:**
   ```bash
   make install DESTDIR=/path/to/local/install
   # Then set CMAKE_PREFIX_PATH when building examples
   ```

4. **Verify installation:**
   ```bash
   # Check that libraries are installed
   ls /usr/local/lib/libmatrixapplication*

   # Update library cache (Linux)
   sudo ldconfig
   ```

**Required matrixserver components:**
- `libmatrixapplication` - Core application framework
- Header files for `CubeApplication` and `MatrixApplication` base classes
- Server executables (server_simulator, server_FPGA, etc.)

**Troubleshooting:**
- If CMake cannot find matrixserver, set `CMAKE_PREFIX_PATH` to your installation directory
- On macOS, you may need to set `DYLD_LIBRARY_PATH` to include the library location
- Ensure all matrixserver dependencies are installed (Boost, OpenGL, etc.)

#### Additional prerequisites

- CMake
- Boost libraries (thread, log, system)
- Imlib2 (image processing)
- Google's Abseil library

### Building

```bash
# Clone the repository
git clone https://github.com/your-username/exampleApplications.git
cd exampleApplications

# Build all applications
mkdir -p build && cd build
cmake ..
make

# Or build specific applications
make cubetestapp     # Basic 3D test
make snake           # 3D Snake game
make breakout3d      # 3D Breakout game
make picture         # Image display
```

### Running Applications

1. Start a matrixserver instance:
   ```bash
   # From matrixserver directory
   ./build/server_simulator  # For development
   ./build/server_FPGA       # For FPGA hardware
   ```

2. Run an example application:
   ```bash
   ./cubetestapp            # Basic 3D demo
   ./snake                  # 3D Snake game
   ./picture image.png      # Display image
   ```

## Platform Support

- **All Platforms**: CubeTestApp, PixelFlow3, basic applications
- **Raspberry Pi**: Additional IMU-based applications (Rainbow, PixelFlow, PixelFlow2)
- **macOS**: Enhanced compatibility with updated CMake configuration

## Hardware Compatibility

Applications work with various matrixserver backends:
- **server_simulator** - Software development and testing
- **server_FPGA** - FTDI USB interface for FPGA boards
- **server_FPGA_RPISPI** - Raspberry Pi SPI interface
- **server_RGBMatrix** - Raspberry Pi GPIO matrix panels

## Package Creation

Generate Debian packages for easy deployment:
```bash
make package
```

Creates .deb files with automatic matrixserver dependency resolution.

## Contributing

This project welcomes contributions! Please see the upstream repository [squarewavedot/exampleApplications](https://github.com/squarewavedot/exampleApplications) for the original codebase and contribution guidelines.

## License

This project follows the same license as the upstream repository. [License of upstream matrixserver](https://github.com/squarewavedot/matrixserver/blob/master/LICENSE)

## Links

- **Upstream Repository**: [squarewavedot/exampleApplications](https://github.com/squarewavedot/exampleApplications)
- **Documentation**: [GitHub Pages Documentation](https://placeholder-link-to-github-pages-docs)
- **matrixserver Framework**: Core framework for LED cube applications
