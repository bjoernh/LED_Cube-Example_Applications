module LEDCube

using Sockets
using ProtoBuf
using COBSReduced
using Base.Threads: @spawn

using Colors
using FixedPointNumbers

include("matrixserver/matrixserver.jl")
using .matrixserver


export Surface, cube_connect, send_frame, run_async
# reexports
export ip_str, RGB

# Why does ProtoBuf not create something like this...
import .matrixserver: MatrixServerMessage
function MatrixServerMessage(; kwargs...)
    default_values = ProtoBuf.default_values(MatrixServerMessage)
    kwargs = merge(default_values, kwargs)
    return MatrixServerMessage(
        kwargs.messageType,
        kwargs.appID,
        kwargs.status,
        kwargs.screenData,
        kwargs.joystickData,
        kwargs.imuData,
        kwargs.serverConfig,
        kwargs.appParamSchema,
        kwargs.appParamUpdate,
        kwargs.appParamValues,
        kwargs.audioData,
    )
end

mutable struct LEDCubeClient{M, S, F}
    const sock::TCPSocket
    const send_buff::IOBuffer
    appID::Int32

    @atomic is_running::Bool

    # callbacks
    const on_message::M
    const on_start::S
    const on_frame::F

    screenInfo::Union{Nothing, Vector{ScreenInfo}}

    function LEDCubeClient(ip::IPAddr, port::Int, on_message::M, on_start::S, on_frame::F) where {M, S, F}
        sock = connect(ip, port)
        send_buff = IOBuffer()
        new{M, S, F}(sock, send_buff, 0, false, on_message, on_start, on_frame, nothing)
    end
end

function send_msg(client::LEDCubeClient, msg::MatrixServerMessage)
    e = ProtoEncoder(client.send_buff)
    encode(e, msg)
    return write(client.sock, COBSReduced.cobs_encode(take!(client.send_buff)))
end

function recv_message(client::LEDCubeClient)
    buf = UInt8[]
    byte = read(client.sock, UInt8)
    while byte != 0x00
        push!(buf, byte)
        byte = read(client.sock, UInt8)
    end
    recv_msg = COBSReduced.cobs_decode(buf)
    decoder = ProtoDecoder(IOBuffer(recv_msg))
    return decode(decoder, MatrixServerMessage)
end

function recv_loop(client::LEDCubeClient)
    try
        while client.is_running
            msg = recv_message(client)
            if msg.messageType == MessageType.appKill
                @info "Server sent appKill. Shutting down..."
                @atomic client.is_running = false
                break
            end
            client.on_message(client, msg)
        end
    catch e
        bt = catch_backtrace()
        @error "Message reader error" exception=(e, bt)
        @atomic client.is_running = false
    end
end

function on_message(client, msg::MatrixServerMessage)
    return nothing
end

function on_start(client)
    return nothing
end

function on_frame(client, t, screenInfo)
    return nothing
end

"""
    cube_connect(ip::IPAddr, port::Int)

Establish connection and register with the server.
"""
function cube_connect(ip::IPAddr, port::Int; on_message=on_message, on_start=on_start, on_frame=on_frame)
    client = LEDCubeClient(ip, port, on_message, on_start, on_frame)

    # Register application
    reg_msg = MatrixServerMessage(;
        messageType=MessageType.registerApp
    )
    send_msg(client, reg_msg)

    # Wait for the assigned AppID
    resp = recv_message(client)
    if resp.messageType != MessageType.registerApp
        error("Expected registerApp response, got $(resp.messageType)")
    end
    client.appID = resp.appID

    # Fetch server info
    info_msg = MatrixServerMessage(;
        messageType=MessageType.getServerInfo,
        appID=client.appID,
    )
    send_msg(client, info_msg)

    resp = recv_message(client)
    if resp.messageType != MessageType.getServerInfo
        error("Expected getServerInfo response, got $(resp.messageType)")
    end
    if resp.serverConfig === nothing
        error("Failed to get server info")
    end
    client.screenInfo = resp.serverConfig.screenInfo
    if client.screenInfo === nothing
        error("No active screens found in ServerConfig")
    end
    for s in client.screenInfo
        @info "Found screen: $(s.screenID) with resolution $(s.width)x$(s.height)"
    end
    return client
end

function run(client)
    # TODO: automatic reconnect

    @atomic client.is_running = true
    errormonitor(@spawn recv_loop(client))

    client.on_start(client)

    frame_time = 1.0 / 30.0 # 30 FPS
    t = 0.0

    while client.is_running
        loop_start = time()

        @invokelatest client.on_frame(client, t, client.screenInfo)
        
        t += frame_time
        elapsed = time() - loop_start
        sleep_time = frame_time - elapsed
        
        sleep(max(0, sleep_time))
    end
end 

run_async(client) = errormonitor(@spawn run(client))


const FrameT = Base.ReshapedArray{RGB{N0f8}, 2, Base.ReinterpretArray{RGB{N0f8}, 1, UInt8, Vector{UInt8}, false}, Tuple{}}
struct Surface
    screenID::Int32
    width::Int
    height::Int
    raw_data::Vector{UInt8}
    frame::FrameT

    function Surface(screenID::Int32, width::Int, height::Int)
        raw_data = zeros(UInt8, width * height * 3) # RGB24 format
        frame_data = reinterpret(RGB{N0f8}, raw_data)
        frame = reshape(frame_data, width, height)
        new(screenID, width, height, raw_data, frame)
    end
end

Surface(screenInfo::ScreenInfo) = Surface(screenInfo.screenID, Int(screenInfo.width), Int(screenInfo.height))

function send_frame(client::LEDCubeClient, surfaces::Vector{Surface})
    screenData = map(surfaces) do surface
        ScreenData(
            surface.screenID,
            surface.raw_data,
            var"ScreenData.Encoding".rgb24bbp,
        )
    end

    frame_msg = MatrixServerMessage(;
        messageType=MessageType.setScreenFrame,
        appID=client.appID,
        screenData=screenData
    )
    send_msg(client, frame_msg)
end

end # module LEDCube
