# Memory Allocation Project — Segmentation

A memory allocation simulator implemented in C++ using segmentation, with a Qt graphical interface for visualization.

## Course
CSE 335s: Operating Systems — Computer & Systems Engineering Department, Ain Shams University

## Overview
Segmentation is a memory management scheme that divides a program into logical units called segments (Code, Data, Stack), each of varying size. This project simulates dynamic memory allocation and deallocation using two strategies:

- **First Fit** — places each segment in the first hole large enough to fit it
- **Best Fit** — places each segment in the smallest hole that still fits it

## Features
- Allocate processes with multiple segments using First Fit or Best Fit
- Deallocate processes and automatically merge neighbouring free holes
- Visual memory layout showing segments and holes as colored blocks
- Segment table display showing base address and size for each segment
- Input validation for holes, process names, and segment sizes
- Qt graphical interface built with Qt 6.11 (MinGW 64-bit)

## Project Structure
```
├── segment.h        # Segment and memType definitions
├── process.h        # Process struct definition
├── memory.h         # Memory class declaration
├── memory.cpp       # Memory allocation/deallocation logic
├── mainwindow.h     # Qt GUI class declaration
├── mainwindow.cpp   # Qt GUI implementation
├── main.cpp         # Application entry point
└── MemoryAllocation.pro  # Qt project file
```

## How to Build
### Qt Creator
1. Open `MemoryAllocation.pro` in Qt Creator
2. Press `Ctrl+R` to build and run

### Command Line
```bash
qmake MemoryAllocation.pro
mingw32-make
```

## How to Use
1. Enter total memory size
2. Enter initial holes as `start,size` — one per line
3. Select allocation strategy (First Fit or Best Fit)
4. Click **Allocate Process** to add a process with segments
5. Click **Deallocate Process** to free a process
6. Click **Reset** to start over

## Executable
Download the pre-built executable (Windows 64-bit):
[Google Drive Link](https://drive.google.com/drive/folders/1jYGuazKCo-7fpgK2DZKPO2rsUUpt1HPA?usp=sharing)

## Author
**Marwa Mohamed Hassan**