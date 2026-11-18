# Development and CI environment for wuzp.
#
# One Dockerfile, two variants:
#   min:    Ubuntu 22.04, the oldest supported dependency versions.
#   latest: Ubuntu 26.04, the newest supported dependency versions, with Eigen 5 and mp-units built from source, and the
#           documentation tools.
#
# ci/image.sh sets the build arguments of each variant. CI publishes the images to GHCR, tagged with a hash of this file
# and the build arguments. Local builds reuse the published layers, so local development and CI run in the same
# environment.

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
        libbenchmark-dev \
        libboost-dev \
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

# Eigen: the Ubuntu package (3.4 in Ubuntu 22.04 and 26.04), or with EIGEN_FROM_SOURCE=true, Eigen 5 built from source,
# which Ubuntu does not package yet. Eigen is header-only, so only its headers and CMake package are installed.
ARG EIGEN_FROM_SOURCE=false
ARG EIGEN_VERSION=5.0.1
ARG EIGEN_SHA256=e9c326dc8c05cd1e044c71f30f1b2e34a6161a3b6ecf445d56b53ff1669e3dec
RUN if [ "${EIGEN_FROM_SOURCE}" = true ]; then \
        work_dir=$(mktemp -d) \
        && curl -fsSL -o "${work_dir}/eigen.tar.gz" \
            "https://gitlab.com/libeigen/eigen/-/archive/${EIGEN_VERSION}/eigen-${EIGEN_VERSION}.tar.gz" \
        && echo "${EIGEN_SHA256}  ${work_dir}/eigen.tar.gz" | sha256sum --check --quiet \
        && tar -xzf "${work_dir}/eigen.tar.gz" -C "${work_dir}" \
        && cmake -S "${work_dir}/eigen-${EIGEN_VERSION}" -B "${work_dir}/build" -G Ninja \
            -D BUILD_TESTING=OFF -D EIGEN_BUILD_BLAS=OFF -D EIGEN_BUILD_LAPACK=OFF -D EIGEN_BUILD_DEMOS=OFF \
            -D EIGEN_BUILD_DOC=OFF > /dev/null \
        && cmake --install "${work_dir}/build" > /dev/null \
        && rm -rf "${work_dir}"; \
    else \
        apt-get update \
        && apt-get install -y --no-install-recommends libeigen3-dev \
        && rm -rf /var/lib/apt/lists/*; \
    fi

# mp-units with MP_UNITS=true, built from source since Ubuntu does not package it. It needs GCC 12 or Clang 16 and CMake 3.25,
# so only the latest variant has it. It is header-only, and needs no other libraries without contract checks.
ARG MP_UNITS=false
ARG MP_UNITS_VERSION=2.5.0
ARG MP_UNITS_SHA256=a6bd48bee699f11f0ed5b04b8c5006d15f76e6d898e058db3880554d2e47a400
RUN if [ "${MP_UNITS}" = true ]; then \
        work_dir=$(mktemp -d) \
        && curl -fsSL -o "${work_dir}/mp-units.tar.gz" \
            "https://github.com/mpusz/mp-units/archive/refs/tags/v${MP_UNITS_VERSION}.tar.gz" \
        && echo "${MP_UNITS_SHA256}  ${work_dir}/mp-units.tar.gz" | sha256sum --check --quiet \
        && tar -xzf "${work_dir}/mp-units.tar.gz" -C "${work_dir}" \
        && cmake -S "${work_dir}/mp-units-${MP_UNITS_VERSION}/src" -B "${work_dir}/build" -G Ninja \
            -D CMAKE_CXX_STANDARD=20 -D MP_UNITS_API_CONTRACTS=NONE > /dev/null \
        && cmake --install "${work_dir}/build" > /dev/null \
        && rm -rf "${work_dir}"; \
    fi

# Blitz++ is built from source: the Ubuntu package lacks the configuration header for Clang
# (https://github.com/blitzpp/blitz/issues/177). The library works with both GCC and Clang.
# CMAKE_POLICY_VERSION_MINIMUM: Blitz++ 1.0.2 declares a CMake minimum that CMake 4 no longer accepts.
ARG BLITZ_VERSION=1.0.2
ARG BLITZ_SHA256=500db9c3b2617e1f03d0e548977aec10d36811ba1c43bb5ef250c0e3853ae1c2
RUN work_dir=$(mktemp -d) \
    && curl -fsSL -o "${work_dir}/blitz.tar.gz" "https://github.com/blitzpp/blitz/archive/refs/tags/${BLITZ_VERSION}.tar.gz" \
    && echo "${BLITZ_SHA256}  ${work_dir}/blitz.tar.gz" | sha256sum --check --quiet \
    && tar -xzf "${work_dir}/blitz.tar.gz" -C "${work_dir}" \
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

# Documentation tools with pinned versions, with DOCS=true (the latest variant), for ci/docs.sh:
# - Doxygen, from its release binary, since the version of the distribution's package depends on the release. The release
#   binary is for x86_64 only: on other architectures, e.g. aarch64, the image has the distribution's package instead
#   (Doxygen 1.15 in Ubuntu 26.04), which builds the same documentation. CI runs on x86_64, with the pinned version.
# - MkDocs and Material for MkDocs, in the venv of the Python tools, with all their dependencies pinned, so that rebuilding the
#   image installs the same versions. Update them together: install the new versions of the two without the other pins, and
#   pin the versions that `pip freeze` lists.
ARG DOCS=false
ARG DOXYGEN_VERSION=1.18.0
ARG DOXYGEN_SHA256=14fa81bdc34171edb5f1f02b1d60e74802f0439b77fa44e592565d517d72df90
ARG MKDOCS_VERSION=1.6.1
ARG MKDOCS_MATERIAL_VERSION=9.8.1
RUN if [ "${DOCS}" = true ]; then \
        if [ "$(dpkg --print-architecture)" = amd64 ]; then \
            work_dir=$(mktemp -d) \
            && curl -fsSL -o "${work_dir}/doxygen.tar.gz" \
                "https://github.com/doxygen/doxygen/releases/download/Release_$(echo "${DOXYGEN_VERSION}" | tr . _)/doxygen-${DOXYGEN_VERSION}.linux.bin.tar.gz" \
            && echo "${DOXYGEN_SHA256}  ${work_dir}/doxygen.tar.gz" | sha256sum --check --quiet \
            && tar -xzf "${work_dir}/doxygen.tar.gz" -C "${work_dir}" \
            && install -m 755 "${work_dir}/doxygen-${DOXYGEN_VERSION}/bin/doxygen" /usr/local/bin/doxygen \
            && rm -rf "${work_dir}"; \
        else \
            apt-get update \
            && apt-get install -y --no-install-recommends doxygen \
            && rm -rf /var/lib/apt/lists/*; \
        fi \
        && /opt/tools/bin/pip install --no-cache-dir --quiet \
            "mkdocs==${MKDOCS_VERSION}" \
            "mkdocs-material==${MKDOCS_MATERIAL_VERSION}" \
            babel==2.18.0 backrefs==8.3 certifi==2026.10.23 charset-normalizer==3.5.2 click==8.5.1 colorama==0.4.6 \
            ghp-import==2.1.0 idna==3.20 Jinja2==3.1.6 Markdown==3.11.3 MarkupSafe==3.0.4 mergedeep==1.3.4 \
            mkdocs-get-deps==0.2.2 mkdocs-material-extensions==1.3.1 packaging==26.3 paginate==0.5.7 pathspec==1.1.1 \
            platformdirs==4.14.5 Pygments==2.21.0 pymdown-extensions==13.1 python-dateutil==4.0.0 PyYAML==6.0.3 \
            pyyaml_env_tag==1.1 requests==2.34.2 tzdata==2026.6 urllib3==2.8.0 watchdog==7.1.0; \
    fi

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
