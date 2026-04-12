# Tutorial Outline: Getting Started with Matrixserver for LED Cubes

## 1. Introduction
* **Welcome to the LEDCube Project:** A brief overview of what the LEDCube is (a 3D volumetric display or a 2D matrix assembly) and what you will achieve by the end of this tutorial.
* **The Architecture Explained:** Demystifying the client-server model.
  * *`matrixserver` (The Server):* Acts as the display driver, managing connections, and routing pixels to the screen/simulator.
  * *`libmatrixapplication` (The Client):* The C++ framework you will use to write applications (games, visualizations) that send pixel data via TCP/IPC to the server.
* **The Web Simulator:** Introduction to the built-in browser-based 3D simulator (`CubeWebapp`), which lets you build and test your apps without physical hardware.

## 2. Setting Up the Development Environment
* **Prerequisites:** What you need installed (Docker, CMake, standard C++ build tools, Boost, Protobuf).
* **Running the Simulator:** The easiest way to get started.
  * Using Docker Compose to spin up the all-in-one simulator image.
  * Identifying the required ports: `2017` (App -> Server TCP), `1337` (Raw WebSocket), `5173` (HTTPS/WSS Webapp UI Proxy).
* **Connecting to the UI:** Navigating to `https://localhost:5173`, trusting the self-signed certificate, and connecting the built-in simulator view.

## 3. Understanding the C++ Application Framework
* **The `CubeApplication` Base Class:** How an application interacts with the cube. It handles FPS timing and network streaming behind the scenes.
* **The Rendering Loop:** Moving away from standard procedural code to a game-loop paradigm:
  * `clear()`: Wiping the previous frame's buffer.
  * Drawing functions: Executing your logic.
  * `render()`: Flushing the buffer over the network to the server.
* **Core Built-in Primitives:** Introduction to basic utilities like `Vector2i`, `Vector3i` representations, `Color`, and `ScreenNumber` (identifying the 6 faces of the cube).

## 4. Writing Your First App: "Hello Cube"
* **Step 1: Project & Build System (`CMakeLists.txt`)**
  * Writing a basic CMake config using `find_package(matrixapplication)` and linking the libraries correctly.
* **Step 2: The Header (`HelloCube.h`)**
  * Subclassing `CubeApplication`.
  * Defining the constructor and the overridden `bool loop()` method.
* **Step 3: The Implementation (`HelloCube.cpp`)**
  * Putting basic pixels or text on the outer faces using `drawText()`. 
* **Step 4: The Entry Point (`main.cpp`)**
  * Setting up `main()`, taking the server URI from program arguments, instantiating the class, and invoking `App.start()`.

## 5. Understanding 3D Voxel Rendering & Math
* **What is a Voxel?** Explaining volumetric pixels (Voxels) vs. traditional 2D Pixels. How the internal resolution restricts available physical coordinate space.
* **Coordinate Systems in 3D Space:**
  * Understanding the bounds: $X$, $Y$, and $Z$ axes defined by `CUBESIZE` (e.g., coordinates from `0` to `CUBESIZE-1`).
  * Determining the origin point `(0, 0, 0)` within the physical cube matrix layout.
* **Volumetric Translations and Boundaries:** 
  * How to handle math when an object passes the boundary (wrapping vs. clipping vs. bounding box collisions).
* **3D Vectors (`Vector3i / Vector3f`) in Practice:**
  * Using vectors for velocity and direction. 
  * Calculating scalar distances between two 3D vectors to trigger events.
* **Drawing Geometric 3D Primitives:** 
  * Understanding how `drawLine3D(Vector3i start, Vector3i end, Color c)` calculates the linear path between two 3D coordinates using algorithms like Bresenham's line algorithm abstracted inside the framework.
  * Using vectors to dynamically shift lines, draw boxes, or render hollow structures across frames.

## 6. Building, Running, and Testing
* **Compiling Your Code:** Executing `cmake .. && make`.
* **Connecting the Dots:** 
  1. Ensure the Docker Simulator is running.
  2. Run your executable: `./HelloCube localhost:2017`.
* **Success!** Seeing your calculations and pixels rendered beautifully in 3D in your browser.

## 7. Going Further (Advanced Topics)
* **Creating Dynamic Parameters via Web UI:** Exposing customizable application variables (like color or speed) to the CubeWebapp, letting you tweak visualizations on the fly using the Parameter Configuration Panel.
* **Interactivity:** How to listen for input from sensors (like IMUs) or joysticks to make interactive games like *Snake3D* or *Breakout3D*.
* **Migrating to Physical Hardware:** 
  * Compiling applications natively on an ARM device (e.g., Raspberry Pi).
  * Changing out the `matrix_server_simulator` daemon for a real hardware server daemon using FPGA or GPIO backends.
