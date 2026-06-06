#!/usr/bin/env bash
set -euo pipefail

# Build cube-example-apps Debian packages (.deb) inside a Debian-trixie container,
# mirroring how libcube builds, and optionally deploy them to the cube device.
#
#   ./scripts/build-deb.sh                       # build arm64 (the cube device)
#   ./scripts/build-deb.sh amd64                 # fast local sanity build
#   ./scripts/build-deb.sh arm64 --deploy        # build + install on the cube
#   ./scripts/build-deb.sh --deploy --host cube@10.0.0.5
#

CUBE_HOST="${CUBE_HOST:-cube@192.168.188.154}"
ARCH_ARG=arm64
DEPLOY=0

while [[ $# -gt 0 ]]; do
    case "$1" in
        arm64|aarch64|linux/arm64) ARCH_ARG=arm64 ;;
        amd64|x86_64|linux/amd64)  ARCH_ARG=amd64 ;;
        --deploy)    DEPLOY=1 ;;
        --host)      CUBE_HOST="${2:?--host needs an argument}"; shift ;;
        --host=*)    CUBE_HOST="${1#--host=}" ;;
        -h|--help)   awk 'NR>=4 && /^#/{sub(/^# ?/,"");print;next} NR>=4{exit}' "$0"; exit 0 ;;
        *) echo "unknown argument: $1 (try --help)" >&2; exit 1 ;;
    esac
    shift
done

case "$ARCH_ARG" in
    arm64) PLATFORM=linux/arm64; TAG=arm64 ;;
    amd64) PLATFORM=linux/amd64; TAG=amd64 ;;
esac

if [[ "${DEPLOY}" == 1 && "${TAG}" != arm64 ]]; then
    echo "error: --deploy targets the arm64 cube device; build arm64 (the default)" >&2
    exit 1
fi

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LIBCUBE_ROOT="$(cd "${REPO_ROOT}/../cube-system/libcube" && pwd)"
IMAGE="cube-build:latest"
OUT="${REPO_ROOT}/dist/${TAG}"
mkdir -p "${OUT}"

# Verify that the libcube dependency package exists in the expected location
LIBCUBE_DEV_DEB="$(ls -1t "${LIBCUBE_ROOT}/dist/${TAG}"/libcube-dev_*_"${TAG}".deb 2>/dev/null | head -1 || true)"
LIBCUBE_RUNTIME_DEB="$(ls -1t "${LIBCUBE_ROOT}/dist/${TAG}"/libcube2_*_"${TAG}".deb 2>/dev/null | head -1 || true)"

if [[ -z "${LIBCUBE_DEV_DEB}" || -z "${LIBCUBE_RUNTIME_DEB}" ]]; then
    echo "error: pre-requisite libcube packages not found in ${LIBCUBE_ROOT}/dist/${TAG}/" >&2
    echo "Please build libcube first for ${TAG}." >&2
    exit 1
fi

echo "==> Building toolchain image (native host)"
docker build -t "${IMAGE}" "${REPO_ROOT}/../cube-system/docker"

# Ensure persistent ccache directory exists on the host
mkdir -p "${REPO_ROOT}/.ccache"

echo "==> Building .debs in container (native cross-compilation)"
docker run --rm \
    -v "${REPO_ROOT}:/src:ro" \
    -v "${REPO_ROOT}/.ccache:/ccache" \
    -v "${LIBCUBE_ROOT}:/libcube:ro" \
    -v "${OUT}:/out" \
    "${IMAGE}" bash -euo pipefail -c '
        # Configure ccache environment
        export CCACHE_DIR=/ccache
        export CCACHE_MAXSIZE=10G

        # Copy the source out of the read-only mount into a clean build tree
        rsync -a \
            --exclude=build/ --exclude=dist/ --exclude=.git/ \
            --exclude=.claude/ --exclude=.antigravitycli/ \
            --exclude=matrixserver/ \
            --exclude=.ccache/ \
            /src/ /build/cube-example-apps/

        # Install pre-built libcube packages (build artifacts, not in image)
        if [[ "'"${TAG}"'" == "arm64" ]]; then
            dpkg -i /libcube/dist/arm64/libcube2_*_arm64.deb \
                    /libcube/dist/arm64/libcube-dev_*_arm64.deb
            cd /build/cube-example-apps
            DEB_BUILD_OPTIONS="parallel=$(nproc)" dpkg-buildpackage -aarm64 -b -us -uc
        else
            dpkg -i /libcube/dist/amd64/libcube2_*_amd64.deb \
                    /libcube/dist/amd64/libcube-dev_*_amd64.deb
            cd /build/cube-example-apps
            DEB_BUILD_OPTIONS="parallel=$(nproc)" dpkg-buildpackage -b -us -uc
        fi

        # dpkg-buildpackage writes artifacts to the parent directory
        cp /build/*.deb /out/ 2>/dev/null || true
        echo "--- produced ---"
        ls -1 /out
    '

echo "==> Artifacts in ${OUT}"
ls -l "${OUT}"

if [[ "${DEPLOY}" != 1 ]]; then
    exit 0
fi

# ── Deploy to the cube device ────────────────────────────────────────────────
APPS_DEB="$(ls -1t "${OUT}"/cube-example-apps_*_arm64.deb 2>/dev/null | head -1 || true)"
if [[ -z "${APPS_DEB}" ]]; then
    echo "error: no cube-example-apps .deb found in ${OUT}" >&2
    exit 1
fi

echo "==> Deploying to ${CUBE_HOST}: $(basename "${APPS_DEB}")"
scp "${APPS_DEB}" "${CUBE_HOST}:/tmp/$(basename "${APPS_DEB}")"

ssh "${CUBE_HOST}" "sudo dpkg -i /tmp/$(basename "${APPS_DEB}") || sudo apt-get install -f -y; rm -f /tmp/$(basename "${APPS_DEB}")"

echo "==> Installed on ${CUBE_HOST}:"
ssh "${CUBE_HOST}" "dpkg-query -W -f='  \${Package} \${Version} \${Architecture}\n' cube-example-apps 2>/dev/null"
