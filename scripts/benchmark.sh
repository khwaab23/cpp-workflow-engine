#!/usr/bin/env sh
set -eu
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target workflow_benchmark --parallel
./build/workflow_benchmark
