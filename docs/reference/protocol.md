# Protocol Reference & Custom Clients

The `matrixserver` uses Protocol Buffers (Protobuf) serialized over TCP or UNIX sockets. If you want to write a custom client (in Python, Rust, Julia, or any other language) rather than using the C++ `libmatrixapplication` framework, this guide explains the communication flow.

---

## 1. Transport Layer: COBS Encoding

Before transmitting over the socket, every Protobuf byte array is framed using **COBS (Consistent Overhead Byte Stuffing)**. 

Because Protobuf itself doesn't define message boundaries, COBS ensures that the receiver knows exactly where a message starts and ends by guaranteeing that a `0x00` byte *only* ever appears at the very end of a transmission.

**The Workflow:**

1. Serialize your `MatrixServerMessage` using your language's Protobuf library into a byte array.
2. Encode the byte array using a COBS algorithm.
3. Append a single `0x00` byte to the end of the encoded array.
4. Send over the TCP Socket (usually `tcp://localhost:2017`).

When receiving, wait for the `0x00` byte, COBS-decode the buffer, and then Protobuf-decode it into a `MatrixServerMessage`.

---

## 2. The Connection Handshake

When your custom client connects to the `matrixserver`, it must follow this registration flow:

### Step 1: Register the Application
Immediately after opening the TCP connection, construct a `MatrixServerMessage` and send it to the server:

```protobuf
messageType: registerApp
```

### Step 2: Receive the `appID`
The server will respond with a `MatrixServerMessage`. You must store the `appID`, as you will need to attach it to every subsequent message you send.

```protobuf
messageType: registerApp
status: success
appID: <Your Unique ID>
```

### Step 3 (Optional): Send Parameter Schema
If your app has dynamic variables that you want to expose to the WebUI for users to control, send an `appParamSchema` message containing an `AppParamSchema` object.

### Step 4: Request Server Hardware Info
Ask the server what screens/panels are currently available to draw on:

```protobuf
messageType: getServerInfo
appID: <Your Unique ID>
```

### Step 5: Receive `ServerConfig`
The server responds with a payload containing a `serverConfig` object. You should iterate through `serverConfig.screenInfo` to discover the `screenID`, `width`, and `height` of every panel connected.

---

## 3. The Render Loop

Once registered, your application enters an infinite loop, generating frame data at your desired FPS and pushing it to the server.

To send a frame, you construct a `setScreenFrame` message containing a `ScreenData` object for *each* screen you want to update.

```protobuf
messageType: setScreenFrame
appID: <Your Unique ID>
screenData: [
  {
    screenID: 0,
    encoding: rgb24bbp,
    frameData: <Raw Byte Array>
  },
  {
    screenID: 1,
    encoding: rgb24bbp,
    frameData: <Raw Byte Array>
  }
]
```

### Frame Data Encoding
When using `rgb24bbp`, the `frameData` bytes must be a contiguous 1D array of `RGB` values. 

If a screen is $64 \times 64$, the byte array will be $64 \times 64 \times 3$ bytes ($12,288$ bytes) long.

`[R, G, B, R, G, B, R, G, B...]`

---

## 4. Summary of Protobuf Messages

*(Refer to `matrixserver.proto` in the source code for the exact schema definitions)*

**Key Message Types:**

* `registerApp`: Sent by client to connect; returned by server with `appID`.
* `getServerInfo`: Sent by client to request hardware layout; returned by server with `ServerConfig`.
* `setScreenFrame`: Sent continuously by the client to update pixel data.
* `appAlive`: A simple ping mechanism.
* `appKill`: Sent by the server when the system is shutting down or forcefully terminating the app.
* `imuData` / `joystickData`: Streamed constantly by the server if sensor/controller inputs are connected.
* `setAppParam`: Sent by the server (via WebUI) to inform your app that a user changed a variable.
