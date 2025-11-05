#!/bin/bash

FEATURES=$(sed -nE 's/(solve_[a-zA-Z_0-9]*) ?= ?\[.*\]/\1/p' Cargo.toml)
TEST_FAILURES=()

echo "[INFO] Running all tests for all features individually:"
for feature in $FEATURES; do
  echo "[INFO] Running tests for $feature"
  cargo test --no-default-features -F "$feature" || {
    TEST_FAILURES+=("$feature")
  }
done


if (( ${#TEST_FAILURES[@]} != 0 )); then
    echo "[ERROR] The following features had test failures:"
    for feature in "${TEST_FAILURES[@]}"; do
      echo "* $feature"
    done
fi
