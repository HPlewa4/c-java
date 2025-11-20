FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
  build-essential \
  cmake \
  pkg-config \
  libopencv-dev \
  libmicrohttpd-dev \
  python3 \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

RUN mkdir -p build && cd build \
  && cmake .. -DCMAKE_BUILD_TYPE=Release \
  && cmake --build . --target server -j"$(nproc)"

RUN chmod +x /app/docker-entrypoint.sh

ENV MODELS_DIR=/app/models \
  PORT=8080

EXPOSE 8080

ENTRYPOINT ["/app/docker-entrypoint.sh"]
