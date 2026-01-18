FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN echo 'Acquire::http::Pipeline-Depth "0";' > /etc/apt/apt.conf.d/99fixbadproxy && \
    echo 'Acquire::http::No-Cache "true";' >> /etc/apt/apt.conf.d/99fixbadproxy && \
    echo 'Acquire::BrokenProxy "true";' >> /etc/apt/apt.conf.d/99fixbadproxy && \
    apt-get update && apt-get install -y --no-install-recommends \
    openjdk-17-jdk \
    xvfb \
    x11vnc \
    x11-utils \
    fluxbox \
    novnc \
    websockify \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app
RUN mkdir -p build/out \
    && find . -name '*.java' -not -path '*/tests/*' -print0 | xargs -0 javac -d build/out
RUN chmod +x /app/docker-entrypoint.sh
ENV DISPLAY=:1 \
    SCREEN_RES=1280x800x24 \
    VNC_PORT=5900 \
    NOVNC_PORT=6080
EXPOSE 5900 6080
ENTRYPOINT ["/app/docker-entrypoint.sh"]