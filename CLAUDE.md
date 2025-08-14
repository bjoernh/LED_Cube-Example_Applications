# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System and Commands

This repository contains example applications for the LEDCube matrixserver framework. All applications use CMake and depend on the matrixserver framework being installed.

### Building Applications

```bash
# Build all enabled applications
mkdir -p build && cd build && cmake .. && make

# Build individual application
cd build && make <application_name>
```

### Platform-Specific Builds

The build system automatically detects Raspberry Pi platforms via `/boot/LICENCE.broadcom` and enables additional applications:
- **All platforms**: CubeTestApp, PixelFlow3
- **Raspberry Pi only**: ImuTest, PixelFlow, PixelFlow2, Rainbow

### Dependencies

Applications require the matrixserver framework to be installed. Core dependencies:
- matrixapplication library (from matrixserver framework)
- Boost (thread, log, system components)
- Imlib2 (image processing)
- absl (Google's Abseil library)

### Package Creation

Generate Debian packages: `make package`
- Creates .deb files with dependency on matrixserver package
- Applications install to `/home/pi/APPS` on Raspberry Pi

## Architecture Overview

### Application Structure

All example applications follow a common inheritance pattern:

**3D Cube Applications** (inherit from `CubeApplication`):
- **CubeTestApp** - Basic 3D functionality demonstration and testing
- **Snake** - Multi-player 3D Snake game with joystick input and AI opponents
- **Genetic** - Genetic algorithm color evolution visualization
- **Blackout3D** - Minimal interactive 3D application template
- **Breakout3D** - Full-featured 3D Breakout game with physics and multiplayer

**2D Matrix Applications** (inherit from `MatrixApplication`):
- **Rainbow** - Particle system with IMU integration (Raspberry Pi only)
- **Picture** - Image display with animation support and file watching
- **Genetic** - Genetic algorithm running on 2D matrix displays

**Animation/Effects Applications**:
- **PixelFlow/PixelFlow2/PixelFlow3** - Fluid dynamics particle simulations

### Common Application Pattern

```cpp
class MyApp : public CubeApplication {
public:
    MyApp() : CubeApplication(30) {} // 30 FPS
    
    bool loop() override {
        clear();                    // Clear 3D space
        // ... rendering logic ...
        render();                   // Render to displays
        return true;               // Continue running
    }
};
```

### Key Framework Features Used

**3D Volumetric Rendering** (CubeApplication):
- `setPixel3D(Vector3i pos, Color color)` - Set individual voxels
- `drawLine3D(Vector3i start, Vector3i end, Color color)` - 3D line drawing
- `drawText(ScreenNumber screen, Vector2i pos, Color color, string text)` - Text on cube faces
- `clear()` - Clear all voxels
- `render()` - Send frame to server

**Screen Management**:
- Six screens available: front, right, back, left, top, bottom
- `CharacterBitmaps::centered` for automatic text centering
- Screen-specific text rendering for cube faces

**Input Integration**:
- Joystick support for interactive applications
- IMU (MPU6050) integration on Raspberry Pi
- ADC (ADS1000) sensor support

### Connection Architecture

Applications connect to matrixserver instances using:
- **Default**: TCP localhost:2017
- **Fallback**: IPC (boost message queue) for local communication
- **Alternative**: Unix sockets

Multiple servers can run simultaneously with different renderers:
- server_simulator - Software development/testing
- server_FPGA - FTDI USB interface for FPGA boards  
- server_FPGA_RPISPI - Raspberry Pi SPI interface
- server_RGBMatrix - Raspberry Pi GPIO matrix panels

### Development Workflow

1. Start appropriate matrixserver (from matrixserver/ directory):
   ```bash
   ./build/server_simulator  # For development
   ./build/server_FPGA       # For FPGA hardware
   ```

2. Build and run example application:
   ```bash
   cd build && make cubetestapp && ./cubetestapp
   ```

3. Applications automatically connect to running server and begin rendering

### Frame Rate and Performance

- Applications specify target FPS in constructor (1-200 range)
- Framework handles automatic frame rate regulation
- Performance monitoring available through load tracking
- Thread-safe rendering with configurable timing

## Detailed Application Examples

### Blackout3D - Minimal Application Template

A minimal 3D application that demonstrates the basic structure:

```cpp
// Blackout3D.cpp - Minimal template
#include "Blackout3D.h"

Blackout3D::Blackout3D() : CubeApplication(20, "cube10.local") {
    // Constructor: 20 FPS, connect to specific host
}

bool Blackout3D::loop() {
    clear();                    // Clear all voxels
    // Add your 3D rendering logic here
    render();                   // Send frame to server
    return true;                // Continue running
}
```

**Usage**: Perfect starting point for new 3D applications. Modify the loop() method to add custom 3D graphics and interactions.

### Breakout3D - Advanced Game with Physics

A complete 3D Breakout game featuring:
- Multi-player support (2 players with joysticks)
- Physics-based ball movement and collision detection
- Block destruction with scoring system
- AI opponents when joysticks not available
- High score persistence

```cpp
// Key game mechanics from breakoutgame.cpp
class BreakoutGame : public CubeApplication {
private:
    enum GameState { pregame, ingame, postgame };
    std::vector<Player*> players_;
    std::vector<Ball*> balls_;
    std::vector<Block*> blocks_;
    
public:
    BreakoutGame() : CubeApplication(40, "192.168.188.106") {
        // Initialize with 40 FPS, connect to specific IP
        reset();
        updateHighScoreFromToFile();
    }
    
    bool loop() override {
        switch(gameState_) {
            case pregame:
                // Show instructions and wait for start
                drawText(front, Vector2i(CharacterBitmaps::centered, 20), 
                        Color::white(), "PRESS A TO PLAY");
                break;
                
            case ingame:
                clear();
                ballLoop();    // Update ball physics
                blockLoop();   // Handle block collisions
                playerLoop();  // Update player paddles
                
                // Render game elements
                for(auto ball : balls_) ball->render();
                break;
                
            case postgame:
                // Show scores and winner
                break;
        }
        render();
        return true;
    }
};
```

**Key Features**:
- **Ball Physics**: Vector-based movement with collision reflection
- **Player Controls**: Joystick input with AI fallback for missing controllers
- **Scoring System**: Points for hitting blocks, penalties for missed balls
- **Power-ups**: Slow motion (R button), rocket ball (B button)


### Snake - Multi-Player 3D Game

3D Snake game supporting up to 8 players with AI:

```cpp
// Key initialization from snake.cpp
Snake::Snake() : CubeApplication(40, "192.168.188.106") {
    float startSpeed = 0.2;
    
    // Create up to 8 players with different colors
    players.push_back(new Player(this, 0, getRandomPointOnScreen(top).cast<float>(), 
                                Vector3f(0, startSpeed, 0), Color::green(), 10));
    players.push_back(new Player(this, 1, getRandomPointOnScreen(top).cast<float>(), 
                                Vector3f(0, startSpeed, 0), Color::red(), 10));
    // ... more players
    
    // Scatter food across all cube faces
    for (int i = 0; i < 20; i++) {
        food.push_back(new Food(this, getRandomPointOnScreen(front), Color::randomBlue() * 2));
        food.push_back(new Food(this, getRandomPointOnScreen(right), Color::randomBlue() * 2));
        // ... all faces
    }
}
```

### Picture - Image Display Application

Display static images or animations on the cube:

```cpp
// picture.cpp - Image loading and display
Picture::Picture(int argc, char *argv[]) : CubeApplication(40, "192.168.188.106") {
    // Parse command line arguments
    if (argc > 1) {
        filepath = std::string(argv[argc - 1]);  // Last argument is image path
        
        if (argc == 4 && std::string(argv[1]) == "-s") {
            // Speed control: -s <speed> <imagefile>
            animationPrescale = std::stoi(argv[2]);
        }
    }
    
    if (!loadImage(filepath)) {
        error = true;
    }
}

bool Picture::loadImage(std::string path) {
    if (autoload.loadImage(path.data())) {
        // Check for proper cube format: 384px wide (6 faces × 64px)
        if (autoload.getWidth() == 384 && autoload.getHeight() % 64 == 0) {
            lastModificationTime = fs::last_write_time(fs::path(path));
            return true;
        }
    }
    return false;
}
```

**Image Requirements**:
- **Dimensions**: 384×64 pixels (6 cube faces × 64px width)
- **Animation**: Multiple 64px height sections for frame animation
- **Auto-reload**: Watches file modification time for live updates

**Command Line Usage**:
```bash
./picture ~/images/mycube.png                    # Static display
./picture -s 10 ~/images/animation.png          # Slower animation
```

### Genetic Algorithm Visualization

Color evolution using genetic algorithms:

```cpp
// genetic.cpp - Evolutionary color matching
class Genetic : public MatrixApplication {
private:
    struct citizen { uint32_t dna; };
    citizen* children_;
    citizen* parents_;
    uint32_t target_;
    int popSize_;
    
public:
    Genetic() : MatrixApplication(40, "192.168.188.106") {
        popSize_ = 64 * 64;  // Full matrix population
        children_ = new citizen[popSize_];
        parents_ = new citizen[popSize_];
        
        target_ = rand() & 0xFFFFFF;  // Random color target
        
        // Initialize random population
        for (int i = 0; i < popSize_; ++i) {
            children_[i].dna = rand() & 0xFFFFFF;
        }
    }
    
    bool loop() override {
        swap();    // Parents ← Children
        sort();    // Sort by fitness
        mate();    // Create new generation
        
        // Display population on matrix
        for(int i = 0; i < popSize_; i++) {
            int c = children_[i].dna;
            int x = i % 64;
            int y = i / 64;
            
            for(auto screen : screens) {
                screen->setPixel(x, y, R(c), G(c), B(c));
            }
        }
        
        // Check convergence and set new target
        if(is85PercentFit()) {
            target_ = rand() & 0xFFFFFF;
        }
        
        render();
        return true;
    }
};
```

## Build Examples

### Building Specific Applications

```bash
# Build all applications
mkdir -p build && cd build && cmake .. && make

# Build individual applications
make cubetestapp     # Basic 3D test
make snake           # 3D Snake game
make breakout3d      # 3D Breakout game
make picture         # Image display
make picture         # Image display
make genetic         # Genetic algorithm

# Run with specific parameters
./picture ~/images/cube_animation.png
./picture -s 5 ~/images/slow_animation.png
```

### Platform Detection in CMake

The build system automatically enables platform-specific applications:

```cmake
# From CMakeLists.txt
if(EXISTS "/boot/LICENCE.broadcom")
    # Raspberry Pi specific applications
    add_subdirectory(Rainbow)      # IMU particle system
    add_subdirectory(PixelFlow)    # Original fluid simulation
    add_subdirectory(PixelFlow2)   # Enhanced fluid simulation
endif()

# Always available applications
add_subdirectory(CubeTestApp)
add_subdirectory(PixelFlow3)       # Cross-platform fluid simulation
```