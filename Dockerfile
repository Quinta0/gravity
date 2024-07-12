# Use an official Ubuntu as a parent image
FROM ubuntu:20.04

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    software-properties-common \
    tzdata

# Set timezone to UTC
RUN ln -fs /usr/share/zoneinfo/Etc/UTC /etc/localtime && \
    dpkg-reconfigure --frontend noninteractive tzdata

# Install CMake
RUN apt-get update \
    && apt-get install -y wget \
    && wget https://github.com/Kitware/CMake/releases/download/v3.28.0/cmake-3.28.0-linux-x86_64.sh \
    && sh cmake-3.28.0-linux-x86_64.sh --prefix=/usr/local --skip-license \
    && rm cmake-3.28.0-linux-x86_64.sh

# Install vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git /vcpkg \
    && cd /vcpkg \
    && git pull \
    && ./bootstrap-vcpkg.sh \
    && ./vcpkg integrate install

# Set environment variables for vcpkg
ENV VCPKG_ROOT=/vcpkg
ENV VCPKG_DEFAULT_TRIPLET=x64-linux

# Set the working directory
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . .

# Install project dependencies using vcpkg
RUN /vcpkg/vcpkg update \
    && /vcpkg/vcpkg install $(cat vcpkg.json | jq -r '.dependencies[]')

# Build the project
RUN mkdir build \
    && cd build \
    && cmake .. -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    && cmake --build .

# Specify the command to run the executable
CMD ["./build/gravity"]
