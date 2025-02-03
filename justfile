set windows-shell := ["pwsh.exe", "-NoLogo", "-Command"]

alias b := build
alias c := configure

build:
    Measure-Command {Start-Process "cmake" -ArgumentList "--build --preset dev" -Wait -NoNewWindow}

configure:
    cmake --preset dev

clean:
    cmake --build --preset dev --target clean