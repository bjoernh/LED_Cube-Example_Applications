# 2. Setting Up the Development Environment

Before writing any code, we need to spin up our virtual LED cube and install the C++ libraries required to build our applications.

---

## 1. Running the Matrixserver Simulator

The easiest and cleanest way to get the server and simulator running without compiling the entire backend stack from source is to use the official Docker image. This "All-in-One" image contains both the `matrixserver` and the `CubeWebapp` interface, proxied nicely behind an Nginx server.

**Prerequisite:** Ensure you have [Docker](https://docs.docker.com/get-docker/) installed on your machine.

Open your terminal and start the simulator by running:

```bash
docker run -it --rm \
  -p 2017:2017 \
  -p 1337:1337 \
  -p 5173:5173 \
  ghcr.io/bjoernh/matrixserver-simulator:latest
```

This ensures three vital ports are mapped to your host machine:
* **`2017`**: This is standard TCP port your C++ applications will connect to.
* **`1337`**: Raw WebSocket simulator data (internal use).
* **`5173`**: The web interface (HTTPS) serving the 3D cube.

---

## 2. Connecting to the UI

With the Docker container happily running, let's open the 3D interface:

1. Open a modern web browser and navigate to **`https://localhost:5173`**.
2. **Security Warning:** The container generates a self-signed TLS certificate to allow secure WebSocket streaming. Your browser will aggressively warn you about this. You can safely proceed by clicking *Advanced -> Proceed to localhost* (Chrome) or *Accept the Risk and Continue* (Firefox).
3. The `CubeWebapp` interface will instantly boot up. Let's link it to the server:
   * In the top control bar, set the WebSocket address to: `wss://localhost:5173/matrix-ws`
   * Click the **Connect** button.

If successful, the status indicator will turn green. You are now staring at a blank, fully functioning virtual 3D LED Matrix!

---

## 3. Installing the C++ Development Libraries

We have our virtual hardware running, but we still need the `libmatrixapplication` C++ toolkit locally to compile the actual applications.

### Dependencies
Before installing the framework, ensure you have a standard C++ development environment. If you are on Ubuntu/Debian, run:
```bash
sudo apt update
sudo apt install build-essential cmake libeigen3-dev libboost-all-dev libprotobuf-dev protobuf-compiler libimlib2-dev
```

If you are on macOS using Homebrew:
```bash
brew install cmake eigen boost protobuf imlib2
```

### Installing `libmatrixapplication`
You can get the library by compiling the `matrixserver` source repository on your local machine:

```bash
# 1. Clone the repository recursively 
git clone --recursive https://github.com/bjoernh/matrixserver.git
cd matrixserver

# 2. Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)

# 3. Install it system-wide
sudo make install
```

*(Note: If you are on Linux and prefer not to compile, you can also download prebuilt `.deb` framework packages from the [matrixserver GitHub Releases page](https://github.com/bjoernh/matrixserver/releases)).*

---

Everything is now in place! You have a running virtual cube simulator, and your computer possesses the headers and libraries required to compile applications for it. 

In the next chapter, we look at the mechanics of the framework and how you write code for the `CubeApplication` base class over a game loop.
