ENV["JULIA_DEBUG"] = "LEDCube"

using WaterLily
using FixedPointNumbers

function circle(n,m;Re=100,U=1)
    # signed distance function to circle
    radius, center = m/8, m/2-1
    sdf(x,t) = √sum(abs2, x .- center) - radius

    Simulation((n,m),   # domain size
               (U,0),   # domain velocity (& velocity scale)
               2radius; # length scale
               ν=U*2radius/Re,     # fluid viscosity
               body=AutoBody(sdf)) # geometry
end

using LEDCube
using Sockets
using Colors

const sim = Ref(circle(4*2^6, 2^6))

function vorticity(sim)
    ω = zeros(size(sim.flow.u)[1:end-1]...);
    @inside ω[I] = WaterLily.curl(3,I,sim.flow.u)*sim.L/sim.U
    return ω
end

function map_color(cm, x, clims = (-10.0, 10.0))
    scale = max(abs(clims[1]), abs(clims[2]))
    t = clamp(x / scale, -1.0, 1.0)
    # Signed sqrt: boosts contrast for small vorticity values near zero
    t = sign(t) * sqrt(abs(t))
    cm_index = clamp(round(Int, (t + 1) / 2 * (length(cm)-1) + 1), 1, length(cm))
    return cm[cm_index]
end

function on_frame(client, t, screenInfo)
    # Implement the animation logic here
    sim_step!(sim[], t)
    ω = vorticity(sim[])' #  2^6+2,4*2^6+2,


    clims = (-10, 10)
    # LCHab diverging colormap: red→yellow→black→green→blue
    # Each half passes through a bright intermediate to boost contrast near the extremes
    cm = vcat(range(LCHab(53, 100,  40), stop=LCHab(97, 94,  90), length=64),  # red → yellow
              range(LCHab(97, 94,  90),  stop=LCHab( 0,  0,   0), length=64),  # yellow → black
              range(LCHab( 0,  0,   0),  stop=LCHab(60, 70, 136), length=64),  # black → green
              range(LCHab(60, 70, 136),  stop=LCHab(32, 130, 306), length=64)) # green → blue

    screen_data = map(screenInfo) do s
        surface = Surface(s)
        if s.screenOrientation == LEDCube.matrixserver.var"ScreenInfo.ScreenOrientation".top ||
           s.screenOrientation == LEDCube.matrixserver.var"ScreenInfo.ScreenOrientation".bottom
           return surface # skip top/bottom screens for now
        end


        # Map front, right, back, left to the corresponding slices of the vorticity field
        # (4*64, 64) to (64, 64) for each screen
        I = if s.screenOrientation == LEDCube.matrixserver.var"ScreenInfo.ScreenOrientation".front
            CartesianIndices((1:64, 1:64))
        elseif s.screenOrientation == LEDCube.matrixserver.var"ScreenInfo.ScreenOrientation".right
            CartesianIndices((1:64, 65:128))
        elseif s.screenOrientation == LEDCube.matrixserver.var"ScreenInfo.ScreenOrientation".back
            CartesianIndices((1:64, 129:192))
        elseif s.screenOrientation == LEDCube.matrixserver.var"ScreenInfo.ScreenOrientation".left
            CartesianIndices((1:64, 193:256))
        else
            error("Unknown screen orientation: $(s.screenOrientation)")
        end

        for J in eachindex(surface.frame)
            surface.frame[J] = map_color(cm, ω[I[J]], clims)
        end
        surface
    end
    send_frame(client, screen_data)
end

sim[] = circle(4*2^6, 2^6)
#client = cube_connect(ip"127.0.0.1", 2017; on_frame=on_frame)
client = cube_connect(ip"10.99.0.1", 2017; on_frame=LEDCube.on_frame_latest(on_frame))
run_async(client)
