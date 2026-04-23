# 7. Going Further (Advanced Topics)

You have successfully learned the architecture of the LEDCube, parsed the differences between interior voxel space and the exterior bounds, linked dependencies via CMake, and rendered an application to a virtual simulation. 

Where do you go from here? 

Here is a brief overview of the advanced features baked directly into the `matrixserver` framework that you can explore next.

---

## 1. Interactive Parameters & the Web UI

Hardcoding values like `Color::red()` or setting animation velocities in C++ means recompiling your binary every time you want to make an artistic tweak.

The framework supports **Dynamic Parameter Exposure**. Because the `matrixserver` maintains a WebSockets connection with the `CubeWebapp`, your client C++ application can actually define dynamic UI controls (Sliders, Color Pickers, Dropdowns) on the fly!

Inside any `CubeApplication`, you have access to a `params` object:
```cpp
// Expose an integer slider to the web interface (1 to 10)
params.exposeVariable("Animation Speed", &mySpeedVar, 1, 10);

// Expose a color picker to the web interface
params.exposeVariable("Laser Color", &myColorVar);
```

When your application connects to the server, the Web Simulator will dynamically generate a settings UI panel on the left side of the screen featuring these exact sliders. If a user tweaks a slider in their web browser, the values seamlessly sync back through the WebSocket directly into your C++ `/HelloCube` variables in real-time!

---

## 2. Hardware Interactivity

Visualizations are fun, but games and responsive designs are better. The runtime handles standard inputs robustly:

### Joysticks
If you plug supported gamepads or build out arcade cabinets, you can attach controllers directly:
```cpp
// Inside constructor
Joystick* player1 = new Joystick(0);

// Inside loop()
if (player1->getButtonPress(0)) {
    // Jump or shoot lasers!
}
```

On non-Raspberry Pi platforms (macOS, Linux desktop), the framework provides a **software joystick fallback** — keyboard or simulated input is mapped to joystick buttons automatically. This means you can develop and test interactive applications in the simulator without any physical gamepad connected.

### Sensor Hardware (IMUs & Gyroscopes)
If you deploy your app onto a physical Raspberry Pi, the framework natively hooks into `MPU6050` gyroscopes via I2C. The base matrix application continually updates static variables depicting physical tilting:
```cpp
// Grab the real-time physical tilt of the hardware
float tiltX = MatrixApplication::latestSimulatorImuX;
```
*(Checkout the `Rainbow` example app to see a liquid physics simulation driven entirely by tilting a Raspberry Pi).*

**Configurable sensor orientation:** Because the MPU6050 can be mounted at different angles, you can correct for its physical orientation without recompiling. Add an `imuOrientation` block to `matrixServerConfig.json`:
```json
{
  "imuOrientation": {
    "xyRotationDeg": 0.0,
    "xzRotationDeg": 45.0,
    "yzRotationDeg": 0.0
  }
}
```
The three fields rotate the raw sensor axes before values reach your application code. All values default to `0.0` when absent.

---

## 3. Deploying to Real Hardware

The web simulator is an incredible tool, but eventually, you want to see your volumetric math running on blindingly bright LEDs. 

The beauty of the `matrixserver` decoupling is that **your `HelloCube` client binary code requires absolutely zero changes to run on real hardware.**

All you change is the server you connect to:
1. Move your code onto an ARM device (like a Raspberry Pi 4). 
2. Compile the `matrixserver` natively, defining a hardware backend (like RGB Matrix standard GPIO pins, or an IceBreaker FPGA module).
```bash
# Example building the real FPGA server instead of the Simulator
cmake -DHARDWARE_BACKEND=FPGA_RPISPI .. && make
```

3. Run the hardware server daemon: `./matrix_server`
4. Run your exact same client code: `./HelloCube`

The exact same TCP protocol fires, but this time, real pixels illuminate the room!

*Note: After prototyping you can change the connection method to IPC for best performance.*


---

### End of Tutorial

Thank you for following along! You are now fully equipped to build multi-threaded, robust volumetric and 2D panel applications. Be sure to explore the `LED_Cube-Example_Applications/` directory to see complex algorithms (like fluid dynamics, 3D Snake, and Genetic algorithm painters) leveraging everything you have learned!
