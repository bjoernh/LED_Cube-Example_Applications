# 6. Building, Running, and Testing

In Chapter 4, we wrote the code for our `HelloCube` application. Now, it is time to bring it to life! 

Because the `matrixserver` framework and client SDK physically decouple the rendering engine from the hardware driver, we can compile and execute our C++ logic purely on our host computer and watch it display beautifully inside the `CubeWebapp` simulator we set up in Chapter 2.

---

## 1. Compiling Your Application

Navigate to the directory where you created your `CMakeLists.txt`, `main.cpp`, `HelloCube.h`, and `HelloCube.cpp` files in your terminal.

We will follow the standard CMake build pattern (creating a separate `build` folder keeps our source code directory clean):

```bash
# 1. Create a dedicated build directory and enter it
mkdir -p build && cd build

# 2. Tell CMake to generate the Makefiles based on your CMakeLists.txt
cmake ..

# 3. Compile the actual executable
make -j$(nproc)
```

If everything was set up correctly in Chapter 2, `make` will link against `libmatrixapplication` and produce an executable binary named `HelloCube` inside your `build` directory.

---

## 2. Ensuring the Simulator is Ready

Before we fire up the application, we must ensure there is a server listening to receive its frame data. 

If your Docker container from Chapter 2 is no longer running, spin it up again:

```bash
docker run -it --rm -p 2017:2017 -p 1337:1337 -p 5173:5173 ghcr.io/bjoernh/matrixserver-simulator:latest
```

Next, ensure your web browser is deeply connected:
1. Navigate to `https://localhost:5173`.
2. Enter the WebSocket target `wss://localhost:5173/matrix-ws` in the top bar.
3. Verify the connection symbol is **Green**. Your virtual 3D cube is waiting for instructions!

---

## 3. Running the Client Application

Return to the terminal where you built your code (you should still be in the `build` directory).

Run your application. Our `main.cpp` defaults to `tcp://localhost:2017` gracefully, so we can just execute the binary natively:

```bash
./HelloCube
```

*(Note: If you run your server on a different machine or a Raspberry Pi down the line, you would simply run `./HelloCube tcp://192.168.1.5:2017`).*

---

## 4. Testing and Verification

Look at your browser tab! You should instantly see the data streaming to the 3D model!

**Visual Checklist:**

* **2D Front Panel:** You should see the word **"Hello"** perfectly centered in green.
* **2D Back Panel:** If you click and drag your mouse to spin the 3D Cube around in your browser, the back panel should read **"Cube!"** in blue.
* **Volumetric Core:** Tracing cleanly along the outer bounds of the cube, a solid red square perimeter should be moving vertically up and down from the floor to the ceiling!

Because the framework enforces `#define OVERSAMPLING`, you'll notice the text isn't jagged, but smoothly anti-aliased exactly matching the physical characteristics of the high-density LED hardware!

**Graceful Exit:**
When you are done marveling at your creation, return to the terminal running your `./HelloCube` and hit `Ctrl+C`. The application will tear down the TCP socket safely, and the web simulator will return to a clear state awaiting the next connection.

---

## Conclusion
Congratulations! You have effectively entered the matrix. You established a server abstraction, mastered the 3D volumetric math boundaries, set up a C++ development framework, and rendered live data to a virtual matrix. 

In our final chapter, **Chapter 7: Going Further**, we will review advanced features like adding hardware interactivity, dynamic Web-UI parameter tweaking, and how to eventually compile your software to run autonomously on physical Raspberry Pi arrays!
