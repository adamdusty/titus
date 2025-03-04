set windows-shell := ["pwsh.exe", "-NoLogo", "-Command"]

alias b := build
alias c := configure
alias p := packaged
alias t := test

[windows]
build:
    (Measure-Command {Start-Process "cmake" -ArgumentList "--build --preset dev" -Wait -NoNewWindow}).TotalSeconds

[linux]
build:
    time cmake --build --preset dev

configure:
    cmake --preset dev

clean:
    cmake --build --preset dev --target clean

packaged:
    build/dev/package/titus_rt

test:
    ctest --preset dev