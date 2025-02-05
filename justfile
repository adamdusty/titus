set windows-shell := ["pwsh.exe", "-NoLogo", "-Command"]

alias b := build
alias c := configure
alias i := integration
alias e := examples

build:
    Measure-Command {Start-Process "cmake" -ArgumentList "--build --preset dev" -Wait -NoNewWindow}

configure:
    cmake --preset dev

clean:
    cmake --build --preset dev --target clean

integration:
    build/dev/integration/titus_rt

examples:
    build/dev/examples/titus_rt