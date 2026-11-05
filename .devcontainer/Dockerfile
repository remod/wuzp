# Development and CI environment for wuzp.
#
# One Dockerfile, two variants:
#   min:    Ubuntu 22.04, the oldest supported dependency versions.
#   latest: Ubuntu 26.04, the newest supported dependency versions.
#
# CI publishes the images to GHCR, tagged with a hash of this directory (see ci/image.sh). Local builds reuse the
# published layers, so local development and CI run in the same environment.

ARG UBUNTU_VERSION=26.04
FROM ubuntu:${UBUNTU_VERSION}

ARG DEBIAN_FRONTEND=noninteractive

# Toolchain, backends, coverage and tooling.
# - git, gpg, curl: required by the Codecov upload, and convenient for development.
# - libclang-rt-dev: Clang's profiling runtime, a separate package since Ubuntu 24.04.
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        bash-completion \
        ca-certificates \
        ccache \
        clang \
        clang-tidy \
        clangd \
        cmake \
        curl \
        g++ \
        gdb \
        git \
        gpg \
        less \
        libboost-dev \
        libeigen3-dev \
        libgtest-dev \
        llvm \
        ninja-build \
        python3 \
        python3-venv \
        sudo \
    && if apt-cache show libclang-rt-dev > /dev/null 2>&1; then \
        apt-get install -y --no-install-recommends libclang-rt-dev; \
    fi \
    && rm -rf /var/lib/apt/lists/*

# Blitz++ is built from source: the Ubuntu package lacks the configuration header for Clang
# (https://github.com/blitzpp/blitz/issues/177). The library works with both GCC and Clang.
# CMAKE_POLICY_VERSION_MINIMUM: Blitz++ 1.0.2 declares a CMake minimum that CMake 4 no longer accepts.
ARG BLITZ_VERSION=1.0.2
RUN work_dir=$(mktemp -d) \
    && curl -fsSL "https://github.com/blitzpp/blitz/archive/refs/tags/${BLITZ_VERSION}.tar.gz" | tar -xz -C "${work_dir}" \
    && cmake -S "${work_dir}/blitz-${BLITZ_VERSION}" -B "${work_dir}/build" -G Ninja -Wno-dev \
        -D CMAKE_BUILD_TYPE=Release -D CMAKE_CXX_COMPILER=g++ -D CMAKE_POLICY_VERSION_MINIMUM=3.5 \
        -D CMAKE_WARN_DEPRECATED=OFF > /dev/null \
    && cmake --build "${work_dir}/build" --target lib blitz-static > /dev/null \
    && cmake --install "${work_dir}/build" > /dev/null \
    && ldconfig \
    && rm -rf "${work_dir}"

# Python tools with pinned versions, identical in both variants:
# - clang-format: formatting must not depend on the distribution.
# - pre-commit: runs clang-format before each commit.
ARG CLANG_FORMAT_VERSION=21.1.8
ARG PRE_COMMIT_VERSION=4.3.0
RUN python3 -m venv /opt/tools \
    && /opt/tools/bin/pip install --no-cache-dir --quiet \
        "clang-format==${CLANG_FORMAT_VERSION}" \
        "pre-commit==${PRE_COMMIT_VERSION}"
ENV PATH=/opt/tools/bin:${PATH}

# ccache speeds up rebuilds, including the projects compiled by the compile-fail tests.
ENV CMAKE_CXX_COMPILER_LAUNCHER=ccache \
    CCACHE_DIR=/ccache
RUN mkdir -p /ccache && chmod 777 /ccache

# Non-root user for local development. The dev container tools map its UID to the host user's UID.
# CI runs as root, which GitHub Actions container jobs expect.
RUN if id -u ubuntu > /dev/null 2>&1; then userdel --remove ubuntu; fi \
    && useradd --create-home --shell /bin/bash --uid 1000 dev \
    && echo "dev ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/dev \
    && chmod 0440 /etc/sudoers.d/dev
