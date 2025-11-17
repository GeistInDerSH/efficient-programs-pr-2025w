#!/bin/bash

BENCHMARK_NAME=${BENCHMARK_NAME:-"^full*"}
FEATURES=$(sed -nE 's/(^solve_[a-zA-Z_0-9]*) ?= ?\[.*\]/\1/p' Cargo.toml)

echo "[INFO] Running Benchmarks"
for feature in $FEATURES; do
  echo "[INFO] Generating for feature: $feature"
  cargo bench ${BENCHMARK_NAME} --bench benchmarks --no-default-features -F "${feature}" -- --save-baseline ${feature}
done