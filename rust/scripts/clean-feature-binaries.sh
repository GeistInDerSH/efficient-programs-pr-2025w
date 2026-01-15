#!/bin/bash

FEATURES=$(sed -nE 's/(^solve_[a-zA-Z_0-9]*) ?= ?\[.*\]/\1/p' Cargo.toml)

echo "[INFO] Removing:"
for feature in $FEATURES; do
  rm -fv "$feature"
done
