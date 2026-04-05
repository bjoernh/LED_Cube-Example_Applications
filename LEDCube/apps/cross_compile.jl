# Needed to install:
# - aarch64-linux-gnu-glibc
# - qemu-user
# - qemu-user-binfmt
qemu_aarch64() = `qemu-aarch64`

function run_aarch64(cmd)
    qemu = addenv(qemu_aarch64(), 
        "QEMU_LD_PREFIX" =>"/usr/aarch64-linux-gnu/",
        "JULIA_CC" => "aarch64-linux-gnu-gcc",
    )
    run(`$(qemu) $(cmd)`)
end

const julia_aarch64 = `julia-1.12.5/bin/julia`
run_aarch64(`$(julia_aarch64) -e "using InteractiveUtils; versioninfo()"`)
run_aarch64(`$(julia_aarch64) --project=. -e "import Pkg; Pkg.instantiate(); Pkg.precompile()"`)


juliac(args) = run_aarch64(`$(julia_aarch64) --project=. -e "using JuliaC; JuliaC.main(ARGS)" -- $(args)`)
cc(app) = juliac(`--output-exe app --bundle build_aarch64 --trim=unsafe-warn --experimental $(app)`)

cc("TestApp")