FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

# Install vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git /vcpkg \
    && /vcpkg/bootstrap-vcpkg.sh \
    && /vcpkg/vcpkg integrate install

# Set the working directory
WORKDIR /app

# Copy the project files
COPY . .

# Install dependencies using vcpkg
RUN /vcpkg/vcpkg install $(cat vcpkg.json | jq -r '.dependencies[]')

# Build the project
RUN mkdir build \
    && cd build \
    && cmake .. -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    && cmake --build .

# Set the entrypoint
CMD ["./build/gravity"]