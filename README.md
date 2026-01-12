# C++ & Java ML Application

A machine learning application featuring a CNN for hairline classification, built with C++ (backend) and Java Swing (frontend).

## Project Structure

```
c-java/
├── backend/                 # C++ backend with CNN implementation
│   ├── include/            # Header files
│   │   ├── model/          # CNN layers, activations, dropout, pooling
│   │   ├── optimizer/      # SGD and Adam optimizers
│   │   └── utils/          # Tensor, augmentation, parallel processing, tests
│   ├── src/
│   │   ├── model/          # CNN implementation
│   │   ├── optimizer/      # Optimizer implementations
│   │   ├── training/       # Training scripts (CPU & GPU)
│   │   ├── server/         # HTTP REST API server
│   │   ├── tests/          # Unit tests
│   │   └── dataset/        # Data preparation scripts
│   ├── models/             # Trained models (cnn_cpu.bin, cnn_gpu.pt)
│   └── build.sh            # Build script
│
└── frontend/               # Java Swing UI
    ├── src/
    │   ├── ui/             # Swing panels and components
    │   └── api/            # Backend API client
    └── run_f.sh            # Run script

```

## Features

**Backend (C++):**
- Pure C++ CNN implementation for CPU training (no external ML libraries)
- LibTorch-based GPU training for faster training
- Two optimizers: SGD and Adam (polymorphic design)
- Generic template classes for tensor operations
- Parallel processing with std::thread
- Unit tests for all components
- HTTP REST API server for inference

**Frontend (Java):**
- Swing-based desktop UI
- Drawing canvas for user input
- Image upload from file
- Real-time inference via backend API

## Quick Start

### 1. Prerequisites

**macOS:**
```bash
brew install cmake opencv libmicrohttpd
```

**Ubuntu:**
```bash
sudo apt-get install cmake build-essential libopencv-dev libmicrohttpd-dev
```

**Java:**
- Java 17 or higher (for frontend)

### 2. Build Backend

```bash
cd backend
./build.sh
```

This creates executables in `backend/build/`:
- `train_quick` - Fast GPU training (requires LibTorch)
- `train_full` - Full CPU training (pure C++)
- `test_model` - Model testing and evaluation
- `server` - HTTP REST API server
- `test_*` - Unit tests

### 3. Training

**Option A: Quick Training (5-10 minutes)**

Requires LibTorch. Downloads dataset and trains with GPU acceleration:

```bash
cd backend/build
./train_quick
```

Outputs: `../models/cnn_gpu.pt`

**Option B: Full CPU Training (30-60 minutes)**

Trains a model using pure C++ (no GPU needed):

```bash
cd backend/build
./train_full
```

Outputs: `../models/cnn_cpu.bin`

**Training Details:**
- Dataset: CelebA (gender classification)
- Architecture: 4-layer CNN with dropout
- Input: 64x64 RGB images
- Output: Binary classification of receding hairline

### 4. Test Model

```bash
cd backend/build

# Test CPU model
./test_model ../models/cnn_cpu.bin

# Test GPU model
./test_model ../models/cnn_gpu.pt
```

Shows accuracy, precision, recall, F1-score, and confusion matrix.

### 5. Run Server

```bash
cd backend/build
./server
```

Server runs on `http://localhost:8080`

API Endpoint:
- `POST /predict` - Binary image data → JSON response with prediction

### 6. Run Frontend

```bash
cd frontend
./run_f.sh
```

The UI connects to the backend at `http://localhost:8080`

Features:
- Upload images from file
- Clear and retry

## Unit Tests

Run all unit tests:

```bash
cd backend/build

# Test generic templates
./test_tensor

# Test activation functions (ReLU, Sigmoid, Tanh)
./test_activations

# Test optimizers (SGD, Adam)
./test_optimizer

# Test parallel processing (ThreadPool)
./test_parallel
```

## LibTorch Setup (Optional - for GPU Training)
LibTorch is only needed for `train_quick` (GPU training). The rest of the project works without it.

1. **Download LibTorch:**
   - Visit https://pytorch.org/get-started/locally/
   - Select:
     - PyTorch Build: Stable
     - Your OS: Linux/Mac/Windows
     - Package: LibTorch
     - Language: C++
     - Compute Platform: CPU or CUDA (for GPU)

2. **Extract to `backend/lib/`:**
   The build system expects LibTorch to be located specifically at `backend/lib/`. 

#### macOS (CPU, Apple Silicon)
```bash
cd backend/lib

curl -L https://download.pytorch.org/libtorch/cpu/libtorch-macos-arm64-2.5.1.zip \
  -o libtorch.zip

unzip libtorch.zip
rm libtorch.zip
```

### Linux (CPU):
```bash
cd backend/lib

wget https://download.pytorch.org/libtorch/cpu/libtorch-shared-with-deps-2.5.1%2Bcpu.zip \
  -O libtorch.zip

unzip libtorch.zip
rm libtorch.zip
```

Verify the structure: `ls backend/lib/libtorch/share/cmake/Torch/TorchConfig.cmake` should exist.

   # Ensure the structure is:
   # backend/lib/libtorch/bin
   # backend/lib/libtorch/include
   # backend/lib/libtorch/lib
   # backend/lib/libtorch/share
   ```

3. **Verify Setup:**
   ```bash
   ls backend/lib/libtorch/share/cmake/Torch  # Should contain TorchConfig.cmake
   ```

## Docker Deployment

### Backend Only

```bash
cd backend
docker compose up --build
```

Server: `http://localhost:8080`

### Frontend Only

```bash
cd frontend
docker compose up --build
```

VNC: `http://localhost:6080/vnc.html` or `vnc://localhost:5900`

### Full Stack

```bash
docker compose up --build
```

Access UI via VNC at `http://localhost:6080/vnc.html`

Stop with `docker compose down`

**CNN Architecture:**
```
Input (64x64x3)
→ Conv1 (32 filters, 5x5) → ReLU → MaxPool(2x2)
→ Conv2 (64 filters, 5x5) → ReLU → MaxPool(2x2) → Dropout(0.25)
→ Conv3 (128 filters, 3x3) → ReLU → MaxPool(2x2)
→ Conv4 (256 filters, 3x3) → ReLU → Dropout(0.5)
→ AvgPool → FC(256 → 1) → Sigmoid
```