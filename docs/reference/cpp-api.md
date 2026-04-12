# C++ API Reference

This section documents the public interfaces for the two core classes in the `libmatrixapplication` framework: `MatrixApplication` and `CubeApplication`. 

If you are just getting started, please see the [Tutorial](../tutorial/outline.md) first to understand the rendering loop paradigm and 3D coordinate basics.

---

## 1. `MatrixApplication`

`MatrixApplication` is the foundation class that handles the network socket connection, Protobuf serialization, and framerate timing. If you are building a 2D-only flat matrix application, you inherit from this class.

### Lifecycle & Flow Control

* `void start()`
  Initiates the network handshake with the `matrixserver`. Once connected and registered, it spins up a background thread that executes your `loop()` method. This method returns immediately (non-blocking).
* `bool pause()`
  Requests the application loop to pause. Returns `true` if successful.
* `bool resume()`
  Resumes the application loop. Returns `true` if successful.
* `void stop()`
  Politely stops the network connection and kills the background looping thread.
* `virtual bool loop() = 0`
  **Abstract.** You must override this method. It is called continuously at the target framerate. Return `true` to continue looping, or `false` to gracefully exit the application.
* `void renderToScreens()`
  Internally called by the framework after your `loop()` finishes to serialize and broadcast the current frame data.

### Configuration & State

* `int getFps()`
  Returns the currently requested frames-per-second.
* `void setFps(int fps)`
  Adjusts the target frame rate dynamically. Max is 200, Min is 1.
* `int getBrightness()`
  Returns the current global hardware brightness level.
* `void setBrightness(int setBrightness)`
  Requests the server to change the physical LED brightness.
* `float getLoad()`
  Returns a float between `0.0` and `1.0` indicating how much of the frame time budget is currently being utilized by your `loop()` logic.
* `AppState getAppState()`
  Returns the current state enum: `starting`, `running`, `paused`, `ended`, `killed`, or `failure`.

### Public Static State (Sensors)

The server continuously streams sensor data (if available) to the application. You can read these statics from anywhere in your code:

* `float latestSimulatorImuX`, `latestSimulatorImuY`, `latestSimulatorImuZ`
* `float latestSimulatorGyroX`, `latestSimulatorGyroY`, `latestSimulatorGyroZ`

---

## 2. `CubeApplication`

`CubeApplication` inherits from `MatrixApplication` and provides the 3D abstraction layer for a volumetric LED cube. It maintains the internal 3D backbuffer and provides drawing primitives.

### Buffer Manipulation

* `void clear()`
  Wipes the internal backbuffer, resetting all voxels to `Color::black()`. Call this at the start of your `loop()`.
* `void fillAll(Color col)`
  Sets every voxel in the 3D volume to the specified color.
* `void fade(float factor)`
  Multiplies the RGB values of every active voxel by `factor` (0.0 to 1.0), causing the entire cube to dim. Useful for creating fading trails.

### Voxel Reading & Writing

* `void setPixel3D(Vector3i pos, Color col, float intensity = 1.0f, bool add = false)`
  (Also available with `int x, int y, int z`). Sets a specific 3D voxel to a color. If `add` is true, the color values are added to the existing voxel color rather than overwriting it.
* `void setPixelSmooth3D(Vector3f pos, Color color)`
  (Also available with floats `x, y, z`). Attempts to render a floating-point coordinate by distributing the color intensity across the closest integer neighbors, creating an anti-aliased "smooth" effect.
* `Color getPixel3D(Vector3i pos)`
  (Also available with `int x, int y, int z`). Returns the current color of the voxel at the specified coordinates.
* `bool isActivePixel3D(Vector3i pos)`
  (Also available with `int x, int y, int z`). Returns true if the voxel is currently any color other than black.

### 3D Drawing Primitives

* `void drawLine3D(Vector3i start, Vector3i end, Color col)`
  (Also available with `int x1, y1, z1, x2, y2, z2`). Uses a 3D line algorithm to draw a solid line between two coordinates.

### 2D Drawing Primitives (Surface Rendering)

These functions draw standard 2D shapes mapped onto a specific 2D panel (`ScreenNumber`) of the cube. Coordinates here are 2D (`Vector2i`).

* `void drawLine2D(ScreenNumber screenNr, Vector2i start, Vector2i end, Color col)`
* `void drawCircle2D(ScreenNumber screenNr, int x0, int y0, int radius, Color col)`
* `void drawRect2D(ScreenNumber screenNr, int x0, int y0, int x1, int y1, Color col, bool filled = false, Color fillColor = Color::black())`
* `void drawRectCentered2D(ScreenNumber screenNr, int x0, int y0, int w, int h, Color col, bool filled = false, Color fillColor = Color::black())`
* `void drawText(ScreenNumber screenNr, Vector2i topLeftPoint, Color col, std::string text)`
  Draws standard 6px font text. You can use the `CharacterBitmaps::centered` constant for the coordinates to auto-center the text.
* `void drawBitmap1bpp(ScreenNumber screenNr, Vector2i topLeftPoint, Color col, Bitmap1bpp bitmap)`

### Helper & Math Methods

* `bool isOnSurface(Vector3i point)`
  Returns true if the 3D coordinate falls exactly on the outer virtual shell of the cube (e.g. `x=0` or `x=65`).
* `ScreenNumber getScreenNumber(Vector3i point)`
  If the point is on a surface, returns which screen it belongs to.
* `bool isOnEdge(Vector3i point)` / `EdgeNumber getEdgeNumber(Vector3i point)`
  Determines if a coordinate lies on a seam between two panels.
* `bool isOnCorner(Vector3i point)` / `CornerNumber getCornerNumber(Vector3i point)`
  Determines if a coordinate lies on one of the 8 corners of the cube.
* `Vector3i getRandomPointOnScreen(ScreenNumber screenNr)`
  Returns a random 3D coordinate that sits on the specified screen.
* `Vector3i getPointOnScreen(ScreenNumber screenNr, Vector2i point)`
  Converts a 2D coordinate on a specific screen into its absolute 3D coordinate equivalent.

### Enums

**`ScreenNumber`**: 
`front`, `right`, `back`, `left`, `top`, `bottom`, `anyScreen`

**`EdgeNumber`**: 
`frontRight`, `rightBack`, `backLeft`, `leftFront`, `topFront`, `topRight`, `topBack`, `topLeft`, `bottomFront`, `bottomRight`, `bottomBack`, `bottomLeft`, `anyEdge`

**`CornerNumber`**: 
`frontRightTop`, `rightBackTop`, `backLeftTop`, `leftFrontTop`, `frontRightBottom`, `rightBackBottom`, `backLeftBottom`, `leftFrontBottom`, `anyCorner`
