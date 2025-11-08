# Dockerfile.builder

FROM --platform=linux/amd64 ubuntu:20.04 AS build

ENV DEBIAN_FRONTEND=noninteractive

# Update, add toolchain PPA, install GCC + dependencies
RUN apt-get update && \
    apt-get install -y software-properties-common wget gnupg && \
    add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
    apt-get update && \
    apt-get install -y \
        gcc-13 \
        g++-13 \
        make cmake git && \
    rm -rf /var/lib/apt/lists/*

# Make GCC 14 default
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

RUN rm -rf /app    
WORKDIR /app

# Copy source code
COPY ./src /app/src
COPY ./CMakeLists.txt /app/CMakeLists.txt

RUN ls -la /app
RUN ls -la /app/src/

# Build the project
RUN mkdir /app/build && cd /app/build \
    && cmake -DCMAKE_BUILD_TYPE=Release .. \
    && cmake --build . -j2

