# Toolchain image for building cube-example-apps Debian packages.
FROM debian:trixie

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        debhelper \
        cmake \
        ninja-build \
        pkg-config \
        libsdl2-dev \
        libimlib2-dev \
        lintian \
        rsync \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
