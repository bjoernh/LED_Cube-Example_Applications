# Toolchain image for building cube-example-apps Debian packages.
FROM debian:trixie

ENV DEBIAN_FRONTEND=noninteractive

# Install native amd64 build tools, cross build-essential for arm64, and ccache
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        crossbuild-essential-arm64 \
        debhelper \
        cmake \
        ninja-build \
        pkg-config \
        lintian \
        rsync \
        ca-certificates \
        ccache \
    && rm -rf /var/lib/apt/lists/*

# Add foreign arm64 architecture
RUN dpkg --add-architecture arm64

ENV CCACHE_DIR=/ccache
WORKDIR /build
