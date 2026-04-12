# 3. Understanding the C++ Application Framework

Now that your development environment is set up and the simulator is running, let's explore how the `libmatrixapplication` C++ toolkit is designed to make rendering to an LED cube incredibly intuitive.

When creating an application, you won't be managing TCP sockets, manually serializing pixel buffers, or dealing with network timings. The framework abstracts all of this into a straightforward Object-Oriented design.

---

## 1. The `CubeApplication` Base Class

Every 3D LED application you build will start by inheriting from a base class called `CubeApplication` (for 3D volumetric software) or `MatrixApplication` (for 2D flat content). 

By inheriting from `CubeApplication`, your class automatically gains the ability to:

* Handle initial handshakes and connection protocols to the `matrixserver`.
* Automatically throttle its compute cycle to a desired Framerate (FPS).
* Keep an internal, thread-safe backbuffer of the Cube's state.

When you instantiate a `CubeApplication`, its constructor takes two main arguments:

1. **Target FPS**: The framerate your logic loop should target (e.g., `30` or `60`).
2. **Server URI**: Where to find the matrixserver (usually `tcp://localhost:2017`).

---

## 2. The Rendering Loop Paradigm

If you have ever done game development, the rendering paradigm here will feel very familiar. Instead of a linear script execution, your application lives inside of an endless, hardware-timed `loop()`.

In your custom class, you will override a single virtual method: `bool loop()`.
Every time this function is executed, your job is to draw the *current frame*.

A standard frame lifecycle looks like this:

```cpp hl_lines="4 7 10"
bool MyCustomApp::loop() {
    // 1. CLEAR the backbuffer from the previous frame
    // This wipes the canvas clean to black.
    clear();

    // 2. DRAW the current state of your logic
    drawLine3D(Vector3i(0,0,0), Vector3i(10,10,10), Color::red());
    
    // 3. RENDER pushes the buffer to the server
    // This serializes the buffer and streams it to the cube visually.
    render();

    // 4. Return true to signal the engine to loop again
    // (Returning false gracefully stops the application)
    return true; 
}
```

By enforcing this structure, you don't have to worry about screen tearing or pushing partial data to the matrix. You construct the entire frame silently, and `render()` blasts it out all at once.

---

## 3. Core Data Types and Primitives

The framework comes with semantic data types built specifically for spatial orientation. To draw effectively, you must understand a few standard structures:

### Coordinates (`Vector2i` and `Vector3i`)
Positions in space are represented by vectors from the **Eigen** mathematics library.

* Use `Vector3i(x, y, z)` for 3D coordinates (used in volumetric manipulation).
* Use `Vector2i(x, y)` when addressing 2D pixel coordinates directly on a specific facial panel.

### Identifying Faces (`ScreenNumber`)
The 3D Cube is composed of six individual 2D screens. When you want to map a 2D effect (like displaying an image or drawing standard text) to a specific side, you use the `ScreenNumber` enum:

* `ScreenNumber::front`
* `ScreenNumber::right`
* `ScreenNumber::back`
* `ScreenNumber::left`
* `ScreenNumber::top`
* `ScreenNumber::bottom`

### `Color`
The framework does not force you to construct raw `uint8_t` RGB chunks by hand.
You will use the `Color` object. It provides helper primitives for pure colors:
```cpp
Color myColor = Color::red();
Color myColor2 = Color::blue();
```
You can also freely define any RGB state: 
```cpp
Color customPurple(128, 0, 128);
```

---

With an understanding of the structure, the Game Loop, and the basic primitives, you are completely ready to write code. In the next chapter, we will open up an IDE and write our "Hello Cube" program from scratch!
