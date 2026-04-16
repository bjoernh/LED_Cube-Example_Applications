JULIA  ?= julia +1.12
JULIAC ?= $(JULIA) --project=. -e "using JuliaC; JuliaC.main(ARGS)" --

APPS = TestApp

.PHONY: all clean $(APPS)

all: $(APPS)

TestApp: TestApp/src/TestApp.jl TestApp/Project.toml
	$(JULIAC) --output-exe app --bundle build --trim=unsafe-warn --experimental $@

clean:
	rm -rf build
