# 1. Introduction

Welcome to the LEDCube tutorial! By the time you finish this guide, you will have written, compiled, and run your very own 3D volumetric visualization from scratch.

## What is the LEDCube Project?

The LEDCube ecosystem is a software and hardware stack designed to drive highly dynamic, modular LED matrix displays. Whether you are building a full 3D volumetric display (a cube composed of six 64x64 led-matrix panels forming an enclosed space), this project provides everything required to render high-framerate graphics, text, and games onto them.

Instead of writing tightly coupled, hardware-specific code, you will use the **matrixserver framework**. This abstracts away the complexity of the hardware—like SPI timings, FPGA protocols, and rendering matrices—leaving you free to focus merely on the math, colors, and game logic of your application.

---

## 🏛️ Architecture Explained: The Client-Server Model

Driving an LED Cube requires moving thousands of pixels dozens of times per second. To keep applications lightweight and isolated from the underlying hardware, the environment employs a robust **Client-Server Architecture**:

### 1. `matrixserver` (The Server / Display Driver)
You can think of `matrixserver` as the "Graphics Card Driver" of the LEDCube. It runs continuously as a background process and holds a single, unified buffer of what the LED screens should represent.
Its primary responsibilities are:
* Listening for network connections from applications on port `2017`.
* Receiving pixel updates stream from the connected application.
* Pushing those pixel formats to the physical screens (using various hardware Renderers) or rendering them to a simulator.

### 2. `libmatrixapplication` (The Client Framework)
This is where you come in! `libmatrixapplication` is a C++ library used to write your client programs. Whether you want to write a multi-player Snake game, simulate Conway's Game of Life, or render flowing 3D particles, your application is structurally fully detached from the server.
Your app will simply calculate pixel data locally and push frames to the `matrixserver` over a fast TCP or IPC socket. 

Because of this decoupling, you can build your application in C++, run it on a completely different machine, and stream the pixels over your local network to an LED Cube.

---

## 🌐 The Web Simulator (`CubeWebapp`)

You might be wondering: *"How do I develop if I don't have a 6-sided LED matrix cube hardware sitting on my desk?"*

The `matrixserver` comes bundled with **CubeWebapp** — an entirely frictionless, browser-based 3D LED Simulator. When you start the server in `simulator` mode, it starts a local web server (driven by Nginx) on your machine.
When you navigate to `https://localhost:5173`, your browser will load a ThreeJS interactive 3D model of the Cube. 

**How Simulator Mode Works:**
Instead of `matrixserver` throwing pixels out over a wire to an FPGA or Raspberry Pi GPIO pins, it streams the pixels via WebSocket (`wss://localhost:5173/matrix-ws`) straight to your browser. This means you can write full applications, see your code execute on a virtual LED cube, interact with it, rotate it naturally, and debug all in software before ever deploying to hardware.

In the next chapter, we'll get this simulator up and running on your machine.
