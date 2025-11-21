#!/bin/bash
set -euo pipefail

DISPLAY=${DISPLAY:-:0}
SCREEN_RES=${SCREEN_RES:-1280x800x24}
VNC_PORT=${VNC_PORT:-5900}
NOVNC_PORT=${NOVNC_PORT:-6080}
APP_CP=${APP_CP:-/app/build/out}

cleanup() {
  for pid in ${JAVA_PID:-} ${NOVNC_PID:-} ${VNC_PID:-} ${FLUXBOX_PID:-} ${XVFB_PID:-}; do
    if [ -n "${pid}" ] && kill -0 "$pid" >/dev/null 2>&1; then
      kill "$pid" >/dev/null 2>&1 || true
      wait "$pid" >/dev/null 2>&1 || true
    fi
  done
}

trap cleanup EXIT

Xvfb "${DISPLAY}" -screen 0 "${SCREEN_RES}" >/var/log/xvfb.log 2>&1 &
XVFB_PID=$!

fluxbox -display "${DISPLAY}" >/var/log/fluxbox.log 2>&1 &
FLUXBOX_PID=$!

x11vnc -display "${DISPLAY}" -forever -shared -nopw -rfbport "${VNC_PORT}" -listen 0.0.0.0 -quiet >/var/log/x11vnc.log 2>&1 &
VNC_PID=$!

websockify --web=/usr/share/novnc "${NOVNC_PORT}" localhost:"${VNC_PORT}" >/var/log/novnc.log 2>&1 &
NOVNC_PID=$!

cat <<EOF
[frontend] Desktop forwarded over VNC on port ${VNC_PORT}
[frontend] Browser access via noVNC at http://localhost:${NOVNC_PORT}/vnc.html
EOF

DISPLAY="${DISPLAY}" java -cp "${APP_CP}" MLAppUI &
JAVA_PID=$!
wait "${JAVA_PID}"
