# 4. Writing Your First App: "Hello Cube"

It is time to organize our project and write our first natively compiled LED Cube application. In this chapter, we will create a simple program that outputs "Hello" and "Cube!" on alternating sides of the cube, while passing a dynamic red laser beam through the interior volume.

Create a new directory for your project, for example `HelloCube`, and let's get started.

---

## Step 1: The Build System (`CMakeLists.txt`)

Because we rely on the `matrixapplication` library and some of its dependencies (like Boost), we must set up our compiler correctly using CMake. 

Create a file named `CMakeLists.txt` in your project folder:

```cmake
cmake_minimum_required(VERSION 3.07)
project(HelloCube)

# Ensure C++20 is used
set(CMAKE_CXX_STANDARD 20)

# Find the Matrix Server Framework and its dependencies
find_package(Protobuf REQUIRED)
find_package(matrixapplication 0.4 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread log)

# Find Eigen3 using PkgConfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(EIGEN3 REQUIRED eigen3)
include_directories(${EIGEN3_INCLUDE_DIRS})

# Compile our C++ files into an executable
add_executable(HelloCube main.cpp HelloCube.cpp)

# Link the matrixapplication libraries
target_link_libraries(HelloCube matrixapplication::matrixapplication)
```

---

## Step 2: The Header (`HelloCube.h`)

Next, we establish the blueprint for our application by inheriting from `CubeApplication`. 

Create `HelloCube.h`:

```cpp
#pragma once
#include <CubeApplication.h>

class HelloCube : public CubeApplication {
public:
    // Constructor allows passing a custom server URI (like tcp://192.168.1.5:2017)
    // DEFAULTSERVERURI automatically points to tcp://localhost:2017
    HelloCube(std::string serverUri = DEFAULTSERVERURI);

    // Our game logic loop
    bool loop() override;
};
```

---

## Step 3: The Implementation (`HelloCube.cpp`)

Here is where the magic happens. We will set our framerate to 30 FPS inside the constructor, and construct the visual layout inside `loop()`. The framework gives us access to a global constant called `CUBESIZE` (usually `64`), representing the width/height/depth bounds.

Create `HelloCube.cpp`:

```cpp
#include "HelloCube.h"

// Initialize the base class with 30 FPS and pass the server string
HelloCube::HelloCube(std::string serverUri) : CubeApplication(30, serverUri) {
}

bool HelloCube::loop() {
    // 1. Wipe previous frame to black
    clear();
    
    // 2. Draw 2D text onto the outer panels using centered parameters
    drawText(ScreenNumber::front, 
             Vector2i(CharacterBitmaps::centered, CharacterBitmaps::centered), 
             Color::green(), "Hello");
             
    drawText(ScreenNumber::back, 
             Vector2i(CharacterBitmaps::centered, CharacterBitmaps::centered), 
             Color::blue(), "Cube!");
    
    // 3. Draw a moving perimeter square that bounces up and down the Z-axis
    // This traces the outer surface of the cube so it's fully visible on the panels!
    static int zPos = 1;
    static int direction = 1;
    
    // Calculate the 4 corners of our slice at the current height
    // We use VIRTUALCUBEMAXINDEX (65) because the framework maps true outer edges beyond the 64 interior
    Vector3i frontLeft(0, 0, zPos);
    Vector3i frontRight(VIRTUALCUBEMAXINDEX, 0, zPos);
    Vector3i backRight(VIRTUALCUBEMAXINDEX, VIRTUALCUBEMAXINDEX, zPos);
    Vector3i backLeft(0, VIRTUALCUBEMAXINDEX, zPos);
    
    // Draw lines connecting the corners to form a square perimeter
    drawLine3D(frontLeft, frontRight, Color::red());
    drawLine3D(frontRight, backRight, Color::red());
    drawLine3D(backRight, backLeft, Color::red());
    drawLine3D(backLeft, frontLeft, Color::red());
    
    // Animate the height and bounce at the top/bottom boundaries
    zPos += direction;
    if(zPos >= CUBESIZE || zPos <= 1) {
        direction *= -1; // Reverse trajectory!
    }
    
    // 4. Send buffer over network
    render();
    
    return true; // continue running
}
```

---

## Step 4: The Entry Point (`main.cpp`)

Every C++ binary needs a `main()` function. This is strictly boilerplate: it parses any command-line arguments to see if you passed a custom server IP, instantiates the app, and triggers `.start()`.

Create `main.cpp`:

```cpp
#include "HelloCube.h"
#include <unistd.h> // For sleep()

int main(int argc, char* argv[]) {
    // Default to tcp://localhost:2017
    std::string serverUri = DEFAULTSERVERURI;
    
    // If user provides an IP via shell (e.g. ./HelloCube tcp://192.168.x.x:2017)
    if(argc > 1) {
        serverUri = argv[1];
    }
    
    // Instantiate our App
    HelloCube myApp(serverUri);
    
    // Start the network handshake and looping thread
    myApp.start();

    // Prevent the main thread from exiting while the application thread runs
    while(true) {
        sleep(1);
    }
    
    return 0;
}
```

---

## Wrap Up

You now have all four files necessary (`CMakeLists.txt`, `HelloCube.h`, `HelloCube.cpp`, `main.cpp`) to build a volumetric application cleanly isolated from hardware complexities! 

In **Chapter 6: Building, Running, and Testing**, we will compile this code and see the red laser sweeping across the blue and green texts in our Web Simulator. Before we do that, we highly recommend reading **Chapter 5: Understanding 3D Voxel Rendering & Math**, which unpacks the specific logic behind `CUBESIZE` bounds and volumetric line drawing utilized above.
