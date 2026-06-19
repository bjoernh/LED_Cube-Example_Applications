module TestApp

using Sockets
using LEDCube

include("fixes.jl")

function on_frame(client, t, screenInfo)
    if screenInfo === nothing
        return
    end
    # Implement the animation logic here
    screen_data = map(screenInfo) do s
        surface = Surface(s)

        for i in 1:surface.width
            for j in 1:surface.height
                # sin curves varying over time and space
                x = (i - 1) / surface.width
                y = (j - 1) / surface.height
                τ = t # time scale

                r = max(0.0, sin(2π * x + τ))^2
                g = max(0.0, sin(2π * y + τ * 1.3))^2
                b = max(0.0, sin(2π * (x + y) + τ * 0.7))^2
                surface.frame[i, j] = RGB(r, g, b)
            end
        end
        return surface
    end

    send_frame(client, screen_data)
    yield()
end

function @main(ARGS)
    println(Core.stdout, "LEDCube AppTest starting...")

    if length(ARGS) > 0
        host = ARGS[1]
    else
        host = "127.0.0.1"
    end
    if length(ARGS) > 1
        port = parse(Int, ARGS[2])
    else
        port = 2017
    end
    host = parse(IPAddr, host)

    println(Core.stdout, "Connecting to LEDCube server at $host:$port")
    client = LEDCube._cube_connect(host, port, LEDCube.on_message, LEDCube.on_start, on_frame)
    LEDCube.run(client)
    return 0
end

end # module AppTest