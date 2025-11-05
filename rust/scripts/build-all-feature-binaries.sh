#!/bin/bash

FEATURES=$(sed -nE 's/(solve_[a-zA-Z_]*) ?= ?\[.*\]/\1/p' Cargo.toml)

echo "[INFO] Building binaries"
for feature in $FEATURES; do
  cargo build --release --no-default-features --features "$feature" 2> /dev/null
  mv ./target/release/sudoku-solver "$feature"
  echo "[INFO] ./$feature"
done