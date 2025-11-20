## Docker Quickstart

1. Install Docker Desktop (or Docker Engine + Compose plugin) and ensure `docker compose version` works.
2. Clone this repo and switch to the project root: `cd c-java`.

### Backend Only

- `cd backend && docker compose up --build`
- Server exposes `http://localhost:8080` and stores model artifacts in `backend/models/`.

### Frontend Only

- `cd frontend && docker compose up --build`
- Connect via browser at `http://localhost:6080/vnc.html` (or any VNC client on `localhost:5900`).

### Full Stack (recommended)

- From the repo root run `docker compose up --build`
- Backend and frontend join the shared `ml_net` network automatically.

### Accessing the UI

- After the root stack starts, open `http://localhost:6080/vnc.html` and click **Connect** to reach the Swing desktop.
- The frontend talks to the backend at `http://backend:8080` inside the Docker network; no extra configuration is required.

Stop any stack with `docker compose down` from the same directory where you launched it.
