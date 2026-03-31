FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git ca-certificates pkg-config \
    libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev \
    libxfixes-dev libxss-dev libxtst-dev libwayland-dev libxkbcommon-dev \
    libegl-dev libgl-dev libgles-dev \
    libasound2-dev libpulse-dev libpipewire-0.3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
