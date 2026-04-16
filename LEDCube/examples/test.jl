using LEDCube
using Sockets

function on_frame(client, t, screenInfo)
    # Implement the animation logic here
    # @show screenInfo
    screen_data = map(screenInfo) do s
        surface = Surface(s)
        if isodd(surface.screenID) 
            # Fill with red color
            fill!(surface.frame, RGB(1.0, 0.0, 0.0))
        else
            # Fill with blue color
            fill!(surface.frame, RGB(0.0, 0.0, 1.0))
        end 
        surface
    end

    send_frame(client, screen_data)
end

client = cube_connect(ip"127.0.0.1", 2017; on_frame=on_frame)
run_async(client)
