#!/usr/bin/env bash
# Builds the example applications for Raspberry Pi (ARM64, all hardware backends)
# inside the matrixserver builder Docker image, then deploys the binaries to cub.local.

set -euo pipefail

# ── Configuration ──────────────────────────────────────────────────────────
CUBE_HOST="${CUBE_LOCAL:-cub.local}"
CUBE_USER="${CUBE_USER:-cube}"
BUILDER_IMAGE="ghcr.io/bjoernh/matrixserver-builder:latest"
CCACHE_VOLUME="matrixserver-builder-ccache"
REMOTE_DEPLOY_DIR="/tmp/exampleapps-deploy"
REMOTE_BIN_DIR="/opt/cube/bin"

# ── Helpers ────────────────────────────────────────────────────────────────
log() { printf '[build-deploy-apps] %s\n' "$*"; }
die() { printf '[build-deploy-apps] ERROR: %s\n' "$*" >&2; exit 1; }

require_cmd() {
    command -v "$1" >/dev/null 2>&1 || die "'$1' not found in PATH"
}

# ── Prerequisites ──────────────────────────────────────────────────────────
require_cmd docker
require_cmd scp
require_cmd ssh

# ── Repo root (parent of this script's directory) ──────────────────────────
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

# ── Builder image ──────────────────────────────────────────────────────────
if ! docker image inspect "$BUILDER_IMAGE" >/dev/null 2>&1; then
    log "Builder image not found locally — pulling ${BUILDER_IMAGE}"
    docker pull "$BUILDER_IMAGE"
else
    log "Builder image present: ${BUILDER_IMAGE}"
fi

# ── Build matrixserver submodule + example apps inside Docker ──────────────
log "Building matrixserver submodule + example applications inside Docker"

docker run --rm \
    --volume "${REPO_ROOT}:/app" \
    --volume "${CCACHE_VOLUME}:/ccache" \
    --workdir /app \
    --env CCACHE_DIR=/ccache \
    "$BUILDER_IMAGE" \
    bash -c '
        set -euo pipefail

        # Mock Raspberry Pi detection
        mkdir -p /boot && touch /boot/LICENCE.broadcom

        # ── 1. Build + install matrixserver ───────────────────────────────
        # Clean any stale x86 rpi-rgb-led-matrix objects that may have been
        # copied from the host
        make -C matrixserver/renderer/RGBMatrixRenderer/rpi-rgb-led-matrix/lib clean 2>/dev/null || true

        # Use a distinct build dir so it never conflicts with the build-pi
        # dir created by the matrixserver build-and-deploy-pi.sh script (which
        # runs with /app = matrixserver root, leaving a CMakeCache pointing
        # to a different source path).
        cmake -S matrixserver \
              -B matrixserver/build-pi-apps \
              -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_INSTALL_PREFIX=/app/matrixserver/install-pi \
              -DENABLE_FPGA_FTDI=ON \
              -DENABLE_FPGA_RPISPI=ON \
              -DENABLE_RGB_MATRIX=ON \
              -DCMAKE_C_COMPILER_LAUNCHER=ccache \
              -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

        cmake --build matrixserver/build-pi-apps -- -j$(nproc)
        cmake --install matrixserver/build-pi-apps

        # ── 2. Build example applications ─────────────────────────────────
        cmake -S . \
              -B build-pi-apps \
              -DCMAKE_BUILD_TYPE=Release \
              -DMATRIXSERVER_PREFIX=/app/matrixserver/install-pi \
              -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=/app/bin-pi \
              -DCMAKE_C_COMPILER_LAUNCHER=ccache \
              -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

        cmake --build build-pi-apps -- -j$(nproc)
        ccache -s
    '

log "Build complete"

# ── Gather binaries ────────────────────────────────────────────────────────
STAGING_DIR="${REPO_ROOT}/build-pi-apps/deploy-staging"
rm -rf "$STAGING_DIR"
mkdir -p "${STAGING_DIR}/bin"

APPS=(
    cubetestapp Genetic Breakout3D Blackout3D PixelFlow3 PixelBeat
    MatrixRain Snake Picture imutestapp PixelFlow PixelFlow2 Rainbow
)

for app in "${APPS[@]}"; do
    src="${REPO_ROOT}/bin-pi/${app}"
    if [ -f "$src" ]; then
        cp "$src" "${STAGING_DIR}/bin/"
        log "Staged: ${app}"
    else
        log "WARNING: binary not found, skipping: ${app}"
    fi
done

log "Staging directory ready: ${STAGING_DIR}"

# ── Stage shared library ───────────────────────────────────────────────────
MATRIXAPP_SO="${REPO_ROOT}/matrixserver/install-pi/lib/libmatrixapplication.so"
if [ -f "$MATRIXAPP_SO" ]; then
    mkdir -p "${STAGING_DIR}/lib"
    cp "$MATRIXAPP_SO" "${STAGING_DIR}/lib/"
    log "Staged: libmatrixapplication.so"
else
    log "WARNING: libmatrixapplication.so not found at ${MATRIXAPP_SO}"
fi

# ── Deploy to Pi ───────────────────────────────────────────────────────────
log "Deploying to ${CUBE_USER}@${CUBE_HOST}:${REMOTE_BIN_DIR} …"

ssh -i ~/.ssh/private "${CUBE_USER}@${CUBE_HOST}" "mkdir -p ${REMOTE_DEPLOY_DIR}/bin ${REMOTE_DEPLOY_DIR}/lib"

scp -i ~/.ssh/private "${STAGING_DIR}/bin/"* "${CUBE_USER}@${CUBE_HOST}:${REMOTE_DEPLOY_DIR}/bin/"
[ -f "${STAGING_DIR}/lib/libmatrixapplication.so" ] && \
    scp -i ~/.ssh/private "${STAGING_DIR}/lib/libmatrixapplication.so" "${CUBE_USER}@${CUBE_HOST}:${REMOTE_DEPLOY_DIR}/lib/"

ssh -i ~/.ssh/private "${CUBE_USER}@${CUBE_HOST}" bash -s <<REMOTE_SCRIPT
set -euo pipefail
sudo mkdir -p ${REMOTE_BIN_DIR}
sudo cp -f ${REMOTE_DEPLOY_DIR}/bin/* ${REMOTE_BIN_DIR}/
sudo chmod +x ${REMOTE_BIN_DIR}/*
if [ -f ${REMOTE_DEPLOY_DIR}/lib/libmatrixapplication.so ]; then
    sudo cp -f ${REMOTE_DEPLOY_DIR}/lib/libmatrixapplication.so /usr/lib/libmatrixapplication.so
    echo "Updated: libmatrixapplication.so"
fi
sudo rm -rf ${REMOTE_DEPLOY_DIR}
echo "Remote installation complete — \$(ls -1 ${REMOTE_BIN_DIR} | wc -l | tr -d ' ') apps in ${REMOTE_BIN_DIR}"
sudo systemctl --no-block restart matrix_server.service
echo "matrixserver will restart..."
REMOTE_SCRIPT

log "Deployed successfully to ${CUBE_USER}@${CUBE_HOST}"

# ── Cleanup ────────────────────────────────────────────────────────────────
rm -rf "$STAGING_DIR"
log "Staging directory cleaned up"
