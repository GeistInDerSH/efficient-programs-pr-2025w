#!/bin/bash

FEATURES=$(sed -nE 's/(solve_[a-zA-Z_]*) ?= ?\[.*\]/\1/p' Cargo.toml)

echo "[INFO] Removing:"
for feature in $FEATURES; do
  rm -fv "$feature"
done
