#!/usr/bin/env bash
set -euo pipefail

# Build and deploy a single example application directly to the Pi without
# Debian packaging, for rapid development and debugging.
#
#   ./scripts/deploy-direct.sh matrixrain
#   ./scripts/deploy-direct.sh breakout3d
#   ./scripts/deploy-direct.sh --host cube@10.0.0.5 matrixrain
#

CUBE_HOST="${CUBE_HOST:-cube@192.168.188.154}"
APP=""
while [[ $# -gt 0 ]]; do
    case "$1" in
        --host)      CUBE_HOST="${2:?--host needs an argument}"; shift ;;
        --host=*)    CUBE_HOST="${1#--host=}" ;;
        -*)          echo "unknown option: $1" >&2; exit 1 ;;
        *)           if [[ -z "${APP}" ]]; then APP="$1"; else echo "only one app can be specified" >&2; exit 1; fi ;;
    esac
    shift
done

if [[ -z "${APP}" ]]; then
    echo "Usage: $0 [options] <app_name>" >&2
    echo "Available apps: breakout3d, cubetestapp, genetic, imutest, matrixrain, picture, pixelflow, pixelflow2, pixelflow3, portalcube, rainbow, snake" >&2
    exit 1
fi

# Map input name to CMake directory / target names
case "${APP}" in
    breakout3d)   DIR="Breakout3D";   TARGET="breakout3d" ;;
    cubetestapp)  DIR="CubeTestApp";  TARGET="cubetestapp" ;;
    genetic)      DIR="Genetic";      TARGET="genetic" ;;
    imutest)      DIR="ImuTest";      TARGET="imutest" ;;
    matrixrain)   DIR="MatrixRain";   TARGET="matrixrain" ;;
    picture)      DIR="Picture";      TARGET="picture" ;;
    pixelflow)    DIR="PixelFlow";    TARGET="pixelflow" ;;
    pixelflow2)   DIR="PixelFlow2";   TARGET="pixelflow2" ;;
    pixelflow3)   DIR="PixelFlow3";   TARGET="pixelflow3" ;;
    portalcube)   DIR="PortalCube";   TARGET="portalcube" ;;
    rainbow)      DIR="Rainbow";      TARGET="rainbow" ;;
    snake)        DIR="Snake";        TARGET="snake" ;;
    *) echo "Unknown app: ${APP}" >&2; exit 1 ;;
esac

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LIBCUBE_ROOT="$(cd "${REPO_ROOT}/../cube-system/libcube" && pwd)"
BUILD_DIR="${REPO_ROOT}/build-arm64"
IMAGE="cube-example-apps-deb-builder:latest"

mkdir -p "${BUILD_DIR}"
mkdir -p "${REPO_ROOT}/.ccache"

# Verify builder image exists
if ! docker image inspect "${IMAGE}" >/dev/null 2>&1; then
    echo "==> Building toolchain image"
    docker build -t "${IMAGE}" "${REPO_ROOT}/docker"
fi

echo "==> Compiling ${TARGET} for arm64 (incremental build)"
docker run --rm \
    -v "${REPO_ROOT}:/src:ro" \
    -v "${REPO_ROOT}/.ccache:/ccache" \
    -v "${LIBCUBE_ROOT}:/libcube:ro" \
    -v "${BUILD_DIR}:/build" \
    "${IMAGE}" bash -euo pipefail -c '
        export CCACHE_DIR=/ccache
        export CCACHE_MAXSIZE=10G

        # Install libcube dependencies inside the container first
        apt-get update
        apt-get install -y --no-install-recommends \
            libsdl2-dev:arm64 \
            libimlib2-dev:arm64 \
            /libcube/dist/arm64/libcube2_*_arm64.deb \
            /libcube/dist/arm64/libcube-dev_*_arm64.deb

        # Run CMake out-of-source configuration if not configured
        if [[ ! -f /build/build.ninja ]]; then
            cmake -S /src -B /build -GNinja \
                -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_C_COMPILER_LAUNCHER=ccache \
                -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
                -DCMAKE_SYSTEM_NAME=Linux \
                -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
                -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
                -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
                -DPKG_CONFIG_EXECUTABLE=/usr/bin/aarch64-linux-gnu-pkg-config
        fi

        # Compile only the specified target
        ninja -C /build "'"${TARGET}"'"
    '

BINARY_PATH="${BUILD_DIR}/${DIR}/${TARGET}"
if [[ ! -f "${BINARY_PATH}" ]]; then
    echo "error: compiled binary not found at ${BINARY_PATH}" >&2
    exit 1
fi

echo "==> Deploying ${TARGET} to ${CUBE_HOST}..."
scp "${BINARY_PATH}" "${CUBE_HOST}:/tmp/${TARGET}"
scp "${REPO_ROOT}/${DIR}/schema.toml" "${REPO_ROOT}/${DIR}/manifest.toml" "${CUBE_HOST}:/tmp/"

ssh "${CUBE_HOST}" bash -s -- "${TARGET}" << 'EOF'
    TARGET="$1"
    sudo mv "/tmp/${TARGET}" "/usr/libexec/cube/apps/${TARGET}"
    sudo chmod +x "/usr/libexec/cube/apps/${TARGET}"
    sudo mkdir -p "/usr/share/cube/apps/${TARGET}"
    sudo mv /tmp/schema.toml /tmp/manifest.toml "/usr/share/cube/apps/${TARGET}/"
    
    # Restart the application if it is currently active
    if cubectl status 2>/dev/null | grep -q "\"active_app\":\"${TARGET}\""; then
        echo "==> Restarting active application ${TARGET}..."
        cubectl stop "${TARGET}" 2>/dev/null || true
        sleep 2
        cubectl launch "${TARGET}" || true
    else
        echo "==> Application ${TARGET} deployed successfully (not currently active)."
    fi
EOF

echo "==> Done!"
