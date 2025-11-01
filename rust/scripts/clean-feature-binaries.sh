#!/bin/bash

FEATURES=$(sed -nE 's/([a-zA-Z_]*) ?= ?\[.*\]/\1/p' Cargo.toml)

echo "[INFO] Removing:"
for feature in $FEATURES; do
  if [[ "$feature" == "default" ]]; then
    continue
  fi
  rm -fv "$feature"
done
